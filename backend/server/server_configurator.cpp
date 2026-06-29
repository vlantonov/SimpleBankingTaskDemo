#include "server_configurator.h"

void configure_server(httplib::Server& server) {
    server.set_error_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"({"error": "Not Found"})", "application/json");
    });
}
