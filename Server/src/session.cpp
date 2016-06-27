#include <sstream>

#include "common.hpp"
#include "session.hpp"
#include "packet.hpp"
#include "utilities.hpp"


/** Interface function definitions **/
Session::Session( shared_ptr<io_service> ios ) : _ios{ ios }, _socket{ *_ios }, _packet{}
{
}

tcp::socket& Session::getSocket( void )
{
    return _socket;
}

void Session::start( void )
{
    try
    {
        _packet.processPackets( _socket );

        stop();
    }
    catch ( exception& ex )
    {
        lockStream();
        LOG_ERR() << "Exception: " << ex.what() << endl;
        unlockStream();
    }
}

void Session::stop( void )
{
    _socket.shutdown( boost::asio::socket_base::shutdown_both );
    _socket.close();
}
