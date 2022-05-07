#include "common.hpp"
#include "client.hpp"

#define SERVER_IP       "127.0.0.1"
#define SERVER_PORT     9900

int main()
{
    Client client{ SERVER_IP, SERVER_PORT };
    client.start();
    return EXIT_SUCCESS;
}
