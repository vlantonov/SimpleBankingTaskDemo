#pragma once
#include <gtest/gtest.h>
#include <httplib.h>
#include <string>
#include <cstdlib>

class BackendTestFixture : public ::testing::Test {
protected:
    std::string backend_host_{"127.0.0.1"};
    int backend_port_{};

    void SetUp() override {
        const char* port_env = std::getenv("BACKEND_PORT");
        backend_port_ = port_env ? std::atoi(port_env) : BACKEND_PORT_DEFAULT;
    }

    httplib::Client make_client() const {
        return httplib::Client(backend_host_, backend_port_);
    }
};
