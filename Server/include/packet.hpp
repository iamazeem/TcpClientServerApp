#ifndef INCLUDE_PACKET_HPP_
#define INCLUDE_PACKET_HPP_


class Packet
{
public:
    Packet();

    const unsigned int getVersion( void ) const;
    const unsigned int getType   ( void ) const;
    const unsigned int getLength ( void ) const;
    const string&      getMessage( void ) const;
    const unsigned int getMsgSize( void ) const;
    const unsigned int getHdrSize( void ) const;
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

    void   appendMessage( const string& message );

    string encodePacket( void );
    void   decodePacket( void );

private:
    enum { VERSION_SIZE = 3, TYPE_SIZE = 5, LENGTH_SIZE = 16 };

    struct Header
    {
        unsigned int    _version : VERSION_SIZE;
        unsigned int    _type    : TYPE_SIZE;
        unsigned int    _length  : LENGTH_SIZE;
    };

    Header              _header;
    string              _message;
};


#endif /* INCLUDE_PACKET_HPP_ */
