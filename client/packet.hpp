#ifndef INCLUDE_PACKET_HPP_
#define INCLUDE_PACKET_HPP_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "message.hpp"

using boost::asio::ip::tcp;
using boost::system::error_code;

class Packet
{
public:
    Packet();

    Packet( const unsigned int version,
            const unsigned int type,
            const unsigned int length,
            const std::string& message );

    const unsigned int getVersion() const;
    const unsigned int getMsgType() const;
    const unsigned int getLength () const;
    const unsigned int getHdrSize() const;
    const unsigned int getMsgSize() const;
    const unsigned int getPktSize() const;
    const std::string& getMessage() const;

    void setVersion( const unsigned int version );
    void setType   ( const unsigned int type    );
    void setLength ( const unsigned int length  );
    void setMessage( const std::string& message );

    void set ( const unsigned int version,
               const unsigned int type,
               const unsigned int length );

    void set ( const unsigned int version,
               const unsigned int type,
               const unsigned int length,
               const std::string& message );

    error_code recv( tcp::socket& socket );
    error_code send( tcp::socket& socket );

    error_code processPackets( tcp::socket& socket );

    friend std::ostream& operator<<( std::ostream& os, const Packet& packet );

private:
    bool isValidVersion();
    bool isValidMsgType();

    struct Header
    {
        enum { VERSION_SIZE = 3, TYPE_SIZE = 5, LENGTH_SIZE = 16 };

        unsigned int    _version : VERSION_SIZE;
        unsigned int    _type    : TYPE_SIZE;
        unsigned int    _length  : LENGTH_SIZE;
    };

    Header              _header;
    std::string         _message;
};

#endif /* INCLUDE_PACKET_HPP_ */
