#include "spdlog/spdlog.h"

#include "defaults.hpp"
#include "server.hpp"

int main()
{
    spdlog::default_logger()->set_pattern("%+");
    spdlog::default_logger()->set_level(spdlog::level::debug);

    server server{defaults::server::ip, defaults::server::port, defaults::server::threads};
    server.start();
    return EXIT_SUCCESS;
}
