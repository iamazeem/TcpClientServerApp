#include "common.hpp"
#include "server.hpp"


int main( void )
{
    Server server{ SERVER_IP, SERVER_PORT, SERVER_THREADS };
    server.start();

	return EXIT_SUCCESS;
}


