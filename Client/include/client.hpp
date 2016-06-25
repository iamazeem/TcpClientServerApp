#ifndef INCLUDE_CLIENT_HPP_
#define INCLUDE_CLIENT_HPP_

#include <boost/asio.hpp>

#include "common.hpp"

using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::system::error_code;


class Client
{
public:
	Client( const string       ip,
	        const unsigned int port );

	~Client();

	void start( void );
	void stop ( void );

private:
	bool connectToServer( void );
	void initiateSession( void );

	io_service      _ios;
	tcp::endpoint   _endpoint;
	tcp::socket     _socket;
};


#endif /* INCLUDE_CLIENT_HPP_ */
