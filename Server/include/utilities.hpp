#ifndef INCLUDE_UTILITIES_HPP_
#define INCLUDE_UTILITIES_HPP_

#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string    getPeerIp  ( const tcp::socket& socket );
unsigned short getPeerPort( const tcp::socket& socket );

#endif /* INCLUDE_UTILITIES_HPP_ */
