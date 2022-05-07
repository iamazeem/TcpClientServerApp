#include <sstream>
#include <exception>

#include "common.hpp"
#include "session.hpp"
#include "packet.hpp"
#include "utilities.hpp"

/** Interface function definitions **/
Session::Session( shared_ptr<io_service> ios )
    :
    _ios{ ios },
    _socket{ *_ios },
    _packet{}
{
}

tcp::socket& Session::getSocket()
{
    return _socket;
}

void Session::start()
{
    try
    {
        _packet.processPackets( _socket );

        stop();
    }
    catch ( const std::exception& ex )
    {
        lockStream();
        LOG_ERR() << "Exception: " << ex.what() << std::endl;
        unlockStream();
    }
}

void Session::stop()
{
    _socket.shutdown( boost::asio::socket_base::shutdown_both );
    _socket.close();
}
