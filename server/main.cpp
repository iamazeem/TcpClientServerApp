#include "common.hpp"
#include "server.hpp"

int main()
{
    server server{SERVER_IP, SERVER_PORT, SERVER_THREADS};
    server.start();
    return EXIT_SUCCESS;
}
