#include "spdlog/spdlog.h"

#include "defaults.hpp"
#include "server.hpp"

int main()
{
    spdlog::default_logger()->set_pattern("[SERVER] %+");
    spdlog::default_logger()->set_level(spdlog::level::debug);

    server_t server{defaults::server::ip, defaults::server::port, defaults::server::threads};
    if (!server.start())
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
