#include "common.hpp"
#include "client.hpp"


int main( void )
{
    Client client{ SERVER_IP, SERVER_PORT };
    client.start();

	return EXIT_SUCCESS;
}


