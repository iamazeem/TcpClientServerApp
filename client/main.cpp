#include <cstdlib>
#include "spdlog/spdlog.h"
#include "defaults.hpp"
#include "client.hpp"

int main()
{
    spdlog::default_logger()->set_pattern("%+");
    spdlog::default_logger()->set_level(spdlog::level::debug);

    client client{defaults::server::ip, defaults::server::port};
    client.start();
    return EXIT_SUCCESS;
}
