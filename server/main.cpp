#include "common.hpp"
#include "server.hpp"

#define SERVER_IP       "127.0.0.1"
#define SERVER_PORT     9900
#define SERVER_THREADS  10

int main()
{
    Server server{ SERVER_IP, SERVER_PORT, SERVER_THREADS };
    server.start();

    return EXIT_SUCCESS;
}
