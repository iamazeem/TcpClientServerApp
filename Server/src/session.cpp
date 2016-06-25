#include <sstream>

#include "common.hpp"
#include "session.hpp"


/** Interface function definitions **/

Session::Session( shared_ptr<io_service> ios ) : _ios( ios ), _socket( *_ios )
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
        boost::asio::streambuf readBuf;
        string command;

        string welcome = "Welcome!\n";

        boost::asio::write( _socket, boost::asio::buffer(welcome) );
        boost::asio::read_until( _socket, readBuf, "\n" );
        boost::asio::write( _socket, boost::asio::buffer(readBuf.data()) );

//        do
//        {
//            boost::asio::read_until( _socket, readBuf, "\n" );
//            boost::asio::write( _socket, boost::asio::buffer(readBuf.data()) );
//
//            std::istream is(&readBuf);
//            is >> command;
//
//        } while( command != "EXIT" );
//
//        _socket.shutdown( boost::asio::socket_base::shutdown_both );
//        _socket.close();
    }
    catch ( exception& ex )
    {
        lockStream();
        LOG_ERR() << "Exception: " << ex.what() << endl;
        unlockStream();
    }
}
