#include <cstdlib>
#include "spdlog/spdlog.h"
#include "constants.hpp"
#include "client.hpp"

int main()
{
    spdlog::default_logger()->set_pattern("%+");
    spdlog::default_logger()->flush_on(spdlog::level::debug);

    client client{SERVER_IP, SERVER_PORT};
    client.start();
    return EXIT_SUCCESS;
}
