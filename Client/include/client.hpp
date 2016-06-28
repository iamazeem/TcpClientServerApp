#ifndef INCLUDE_CLIENT_HPP_
#define INCLUDE_CLIENT_HPP_

#include <boost/asio.hpp>

#include "common.hpp"
#include "packet.hpp"

using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::system::error_code;


class Client
{
public:
	Client( const string         ip,
	        const unsigned short port );

	~Client();

	void start( void );
	void stop ( void );

private:
	bool connectToServer( void );

	io_service      _ios;
	tcp::endpoint   _endpoint;
	tcp::socket     _socket;

	Packet          _packet;
};


#endif /* INCLUDE_CLIENT_HPP_ */
