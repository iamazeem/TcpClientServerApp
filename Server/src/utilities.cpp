#include "utilities.hpp"


string getPeerIp( const tcp::socket& socket )
{
    return socket.remote_endpoint().address().to_string();
}

unsigned short getPeerPort( const tcp::socket& socket )
{
    return socket.remote_endpoint().port();
}
