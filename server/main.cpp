#include "spdlog/spdlog.h"

#include "constants.hpp"
#include "server.hpp"

int main()
{
    spdlog::default_logger()->set_pattern("%+");

    server server{SERVER_IP, SERVER_PORT, SERVER_THREADS};
    server.start();
    return EXIT_SUCCESS;
}
