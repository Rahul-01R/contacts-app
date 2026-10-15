#include <iostream>
#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    httplib::Server svr;
    svr.set_mount_point("/", "./web"); // serves web files

    svr.Get("/api/ping", [](const httplib::Request&, httplib::Response& res) {
        json j = {{"ok", true}};
        res.set_content(j.dump(), "application/json");
    });

    std::cout << "Open http://127.0.0.1:8080" << std::endl;
    svr.listen("127.0.0.1", 8080);
}