#include <cstdlib>
#include "spdlog/spdlog.h"
#include "defaults.hpp"
#include "client.hpp"

int main()
{
    spdlog::default_logger()->set_pattern("%+");
    spdlog::default_logger()->set_level(spdlog::level::debug);

    client_t client{defaults::server::ip, defaults::server::port};
    if (!client.start())
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
