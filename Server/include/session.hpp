#ifndef INCLUDE_SESSION_HPP_
#define INCLUDE_SESSION_HPP_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::asio::streambuf;
using boost::shared_ptr;


class Session
{
public:
    Session( shared_ptr<io_service> ios );

    void start( void );

    tcp::socket& getSocket( void );

private:
    shared_ptr<io_service>      _ios;
    tcp::socket                 _socket;
    streambuf                   _readBuffer;
};


#endif /* INCLUDE_SESSION_HPP_ */
