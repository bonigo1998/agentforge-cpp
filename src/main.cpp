#include "agent.hpp"

#include <curl/curl.h>

#include <exception>
#include <iostream>

int main() {
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        std::cerr << "Failed to initialize libcurl.\n";
        return 1;
    }

    int exit_code = 0;

    try {
        Agent agent;
        agent.run();
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << '\n';
        exit_code = 1;
    }

    curl_global_cleanup();
    return exit_code;
}