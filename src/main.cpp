#include <iostream>
#include <mutex>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "ContactBook.h"
#include "Storage.h"
#include "VCard.h"
#include <algorithm>

using json = nlohmann::json;
inline void to_json(nlohmann::json& j, const Warning& w) {
    j = nlohmann::json{{"type", w.type}, {"other", w.other}};
}

void sendJson(httplib::Response& res, int status, const json& j) {
    res.status = status;
    res.set_content(j.dump(), "application/json");
}
void respondWith(const OpResult& r, httplib::Response& res, int successStatus, int id = 0) {
    switch (r.status) {
        case OpResult::OK:
            sendJson(res, successStatus, {{"contact", r.contact}, {"warnings", r.warnings}});
            break;
        case OpResult::INVALID:
            sendJson(res, 400, {{"error", "validation"}, {"field", r.field}, {"message", r.message}});
            break;
        case OpResult::DUPLICATE_PHONE:
            sendJson(res, 409, {{"error", "duplicate_phone"}, {"message", "This number already belongs to " + r.contact.name}, {"existing", r.contact}});
            break;
        case OpResult::NOT_FOUND:
            sendJson(res, 404, {{"error", "not_found"}, {"message", "No contact with id " + std::to_string(id)}});
            break;
    }
}
void persist(const ContactBook& book) {
    saveContacts("data/contacts.json", book.listAll());
}
int main() {
    ContactBook book;
    std::mutex bookMutex;

    httplib::Server svr;
    svr.set_mount_point("/", "./web");

    svr.Get("/api/ping", [](const httplib::Request&, httplib::Response& res) {
        sendJson(res, 200, {{"ok", true}});
    });

    svr.Get("/api/contacts", [&](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(bookMutex);
        json j = book.listAll();
        sendJson(res, 200, j);
    });
    svr.Post("/api/contacts", [&](const httplib::Request& req, httplib::Response& res) {
        json body = json::parse(req.body, nullptr, false);
        if (body.is_discarded() || !body.is_object()) {
            sendJson(res, 400, {{"error", "bad_json"}, {"message", "Request body must be a JSON object"}});
            return;
        }
        Contact in;
        try { in = body.get<Contact>(); }
        catch (const json::exception&) {
            sendJson(res, 400, {{"error", "bad_json"}, {"message", "A field has the wrong type"}});
            return;
        }
        std::lock_guard<std::mutex> lock(bookMutex);
        OpResult r = book.add(in);

        if(r.status == OpResult::OK){
            persist(book);
        }
        respondWith(r, res, 201);
    });

    svr.Put(R"(/api/contacts/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1]);
        json body = json::parse(req.body, nullptr, false);
        if (body.is_discarded() || !body.is_object()) {
            sendJson(res, 400, {{"error", "bad_json"}, {"message", "Request body must be a JSON object"}});
            return;
        }
        Contact in;
        try { in = body.get<Contact>(); }
        catch (const json::exception&) {
            sendJson(res, 400, {{"error", "bad_json"}, {"message", "A field has the wrong type"}});
            return;
        }
        std::lock_guard<std::mutex> lock(bookMutex);
        OpResult r = book.edit(id, in);

        if(r.status == OpResult::OK){
            persist(book);
        }
        respondWith(r, res, 200, id);
    });

    svr.Delete(R"(/api/contacts/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1]);
        std::lock_guard<std::mutex> lock(bookMutex);
        if (book.remove(id)) {
            persist(book);
            sendJson(res, 200, {{"deleted", id}});
        } else {
            sendJson(res, 404, {{"error", "not_found"}, {"message", "No contact with id " + std::to_string(id)}});
        }
    });
    

    svr.Get(R"(/api/contacts/(\d+)/vcard)", [&](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1]);
        
        std::lock_guard<std::mutex> lock(bookMutex);
        Contact c;
        if (!book.get(id, c)) {
            sendJson(res, 404, {{"error", "not_found"}, {"message", "No contact with id " + std::to_string(id)}});
            return;
        }

        // Replace non-alphanumeric characters with underscore
        std::string safeName = c.name;
        std::replace_if(safeName.begin(), safeName.end(), [](char ch) { return !std::isalnum(ch); }, '_');
        if (safeName.empty()) safeName = "contact";

        res.set_header("Content-Disposition", "attachment; filename=\"" + safeName + ".vcf\"");
        res.set_content(toVCard(c), "text/vcard; charset=utf-8");
    });

    svr.Get("/api/search", [&](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("q")) {
            sendJson(res, 400, {{"error", "bad_request"}, {"message", "Missing query parameter 'q'"}});
            return;
        }
        std::string q = req.get_param_value("q");
        
        std::lock_guard<std::mutex> lock(bookMutex);
        json j = book.searchExact(q);
        sendJson(res, 200, j);
    });

    // Request Logger
    svr.set_logger([](const httplib::Request& req, const httplib::Response& res) {
        std::cout << req.method << " " << req.path << " -> " << res.status << std::endl;
    });
    
        std::cout << "Open http://127.0.0.1:8080" << std::endl;
        std::vector<Contact> loaded = loadContacts("data/contacts.json");
        if (loaded.empty()) {
            loaded = loadContacts("data/sample_contacts.json");
        }
        book.loadAll(loaded);
        svr.listen("127.0.0.1", 8080);
        return 0;
    }