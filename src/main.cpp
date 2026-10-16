#include <iostream>
#include <mutex>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "ContactBook.h"
#include "Storage.h"

using json = nlohmann::json;

void sendJson(httplib::Response& res, int status, const json& j) {
    res.status = status;
    res.set_content(j.dump(), "application/json");
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

    std::cout << "Open http://127.0.0.1:8080" << std::endl;
    svr.listen("127.0.0.1", 8080);
    return 0;
}