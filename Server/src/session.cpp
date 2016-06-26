#include <sstream>

#include "common.hpp"
#include "session.hpp"
#include "packet.hpp"
#include "utilities.hpp"


/** Interface function definitions **/

Session::Session( shared_ptr<io_service> ios ) : _ios{ ios }, _socket{ *_ios }
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
        Packet packet;

        std::ostringstream ss;
        ss << "Welcome user! [" << getPeerIp( _socket ) << ":" << getPeerPort( _socket ) << "]\n";

        packet.setMessage( ss.str() );
        packet.set( MSG_VERSION, MSG_WELCOME, packet.getPktSize() );

        lockStream();
        LOG_INF() << packet << endl;
        unlockStream();

        boost::asio::write( _socket, boost::asio::buffer( &packet.getHeader(), packet.getHdrSize() ) );
        boost::asio::write( _socket, boost::asio::buffer( packet.getMessage(), packet.getMsgSize() ) );

//        boost::asio::streambuf readBuf;
//        string command;
//
//        string welcome = "Welcome!\n";

//        boost::asio::write( _socket, boost::asio::buffer(welcome) );
//        boost::asio::read_until( _socket, readBuf, "\n" );
//        boost::asio::write( _socket, boost::asio::buffer(readBuf.data()) );

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
        _socket.shutdown( boost::asio::socket_base::shutdown_both );
        _socket.close();
    }
    catch ( exception& ex )
    {
        lockStream();
        LOG_ERR() << "Exception: " << ex.what() << endl;
        unlockStream();
    }
}
