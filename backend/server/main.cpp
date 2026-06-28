#include <httplib.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }

    httplib::Server server;

    server.set_error_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"error\": \"Not Found\"}", "application/json");
    });

    std::cout << "Banking server starting on port " << port << std::endl;
    server.listen("0.0.0.0", port);

    return 0;
}
