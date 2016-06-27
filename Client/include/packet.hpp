#ifndef INCLUDE_PACKET_HPP_
#define INCLUDE_PACKET_HPP_

#include "message.hpp"


struct Header
{
    enum { VERSION_SIZE = 3, TYPE_SIZE = 5, LENGTH_SIZE = 16 };

    unsigned int    _version : VERSION_SIZE;
    unsigned int    _type    : TYPE_SIZE;
    unsigned int    _length  : LENGTH_SIZE;
};

class Packet
{
public:
    Packet();

    Packet( const unsigned int version,
            const unsigned int type,
            const unsigned int length,
            const string&      message );

    const unsigned int getVersion( void ) const;
    const unsigned int getMsgType   ( void ) const;
    const unsigned int getLength ( void ) const;
    const Header&      getHeader ( void ) const;
    const string&      getMessage( void ) const;
    const unsigned int getHdrSize( void ) const;
    const unsigned int getMsgSize( void ) const;
    const unsigned int getPktSize( void ) const;

    void setVersion( const unsigned int version );
    void setType   ( const unsigned int type    );
    void setLength ( const unsigned int length  );
    void setMessage( const string&      message );

    void set ( const unsigned int version,
               const unsigned int type,
               const unsigned int length );

    void set ( const unsigned int version,
               const unsigned int type,
               const unsigned int length,
               const string&      message );

    friend std::ostream& operator<<( std::ostream& os, const Packet& packet );

private:
    Header              _header;
    string              _message;
};


#endif /* INCLUDE_PACKET_HPP_ */
