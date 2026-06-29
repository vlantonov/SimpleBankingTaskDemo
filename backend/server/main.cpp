#include <httplib.h>
#include <cstdlib>
#include <iostream>
#include "server_configurator.h"

static int parse_port(int argc, char* argv[]) {
    if (argc > 1) {
        return std::atoi(argv[1]);
    }
    return 8080;
}

int main(int argc, char* argv[]) {
    const int port = parse_port(argc, argv);
    httplib::Server server;
    configure_server(server);
    std::cout << "Banking server starting on port " << port << std::endl;
    server.listen("0.0.0.0", port);
    return 0;
}
