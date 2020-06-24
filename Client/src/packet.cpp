#include "common.hpp"
#include "packet.hpp"
#include "utilities.hpp"

using boost::asio::streambuf;
using boost::asio::read;
using boost::asio::write;
using boost::asio::buffer;
using boost::asio::transfer_exactly;

Packet::Packet() : _header{ MSG_VERSION, MSG_DEFAULT, 0 }, _message{""}
{
}

Packet::Packet( const unsigned int version,
                const unsigned int type,
                const unsigned int length,
                const std::string& message )
                :
                _header { version, type, length },
                _message{ message }
{
}

const unsigned int Packet::getVersion() const { return _header._version;  }
const unsigned int Packet::getMsgType() const { return _header._type;     }
const unsigned int Packet::getLength () const { return _header._length;   }
const unsigned int Packet::getHdrSize() const { return sizeof(_header);   }
const unsigned int Packet::getMsgSize() const { return _message.length(); }
const unsigned int Packet::getPktSize() const { return getHdrSize() + getMsgSize(); }
const std::string& Packet::getMessage() const { return _message;          }


void Packet::setVersion( const unsigned int version ) { _header._version = version; }
void Packet::setType   ( const unsigned int type    ) { _header._type    = type;    }
void Packet::setLength ( const unsigned int length  ) { _header._length  = length;  }
void Packet::setMessage( const std::string& message ) { _message         = message; }

void Packet::set ( const unsigned int version,
                   const unsigned int type,
                   const unsigned int length )
{
    setVersion( version );
    setType   ( type    );
    setLength ( length  );
}

void Packet::set ( const unsigned int version,
                   const unsigned int type,
                   const unsigned int length,
                   const std::string& message )
{
    set( version, type, length );
    setMessage( message );
}

error_code Packet::recv( tcp::socket& socket )
{
    error_code ec;
    read( socket, buffer( &_header, getHdrSize() ), transfer_exactly( getHdrSize() ), ec );
    if ( ec )
    {
        lockStream();
        LOG_INF() << "Header read error! " << ec.message() << std::endl;
        unlockStream();

        return ec;
    }

    // lockStream();
    // LOG_INF() << "Header: "
    //             << std::hex << getVersion() << ", "
    //             << std::hex << getMsgType()    << ", "
    //             << std::dec << getLength()  << std::endl;
    // unlockStream();

    const unsigned int readSize = getLength() - getHdrSize();
    if ( readSize > 0 )
    {
        // lockStream();
        // LOG_INF() << "Read Size: " << readSize << std::endl;
        // unlockStream();

        streambuf readBuf;
        read( socket, readBuf, transfer_exactly( readSize ), ec );
        if ( ec )
        {
            LOG_ERR() << "Message read error! " << ec.message() << std::endl;
            return ec;
        }

        const std::string dataReceived( (std::istreambuf_iterator<char>( &readBuf )),
                                         std::istreambuf_iterator<char>() );

        // Store read message
        setMessage( dataReceived );

        lockStream();
        LOG_INF() << "Received: " << *this << std::endl;
        unlockStream();
    }

    return ec;
}

error_code Packet::send( tcp::socket& socket )
{
    lockStream();
    LOG_INF() << "Sending packet: " << *this << std::endl;
    unlockStream();

    // Send packet header
    error_code ec;
    write( socket, buffer( &_header, getHdrSize() ), transfer_exactly( getHdrSize() ), ec );
    if ( ec )
    {
        lockStream();
        LOG_ERR() << "Could not send header! ["
                  << getPeerIp( socket ) << ":"
                  << getPeerPort( socket )
                  << "]" << std::endl;
        unlockStream();

        return ec;
    }

    // Send packet message
    write( socket, buffer( getMessage(), getMsgSize() ), transfer_exactly( getMsgSize() ), ec );
    if ( ec )
    {
        lockStream();
        LOG_ERR() << "Could not send message! ["
                  << getPeerIp( socket ) << ":"
                  << getPeerPort( socket )
                  << "]" << std::endl;
        unlockStream();

        return ec;
    }

    lockStream();
    LOG_INF() << "Packet sent! ["
              << getPeerIp( socket ) << ":"
              << getPeerPort( socket )
              << "]..." << std::endl;
    unlockStream();

    // Reset packet message
    setMessage( "" );

    return ec;
}

error_code Packet::processPackets( tcp::socket& socket )
{
    // Receive welcome message from server
    error_code ec = recv( socket );
    if ( ec )
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    LOG_INF() << "Received: " << getMessage() << std::endl;

    // Send command packet
    std::string cmd = "ls ~/Test.cpp";

    setMessage( cmd );
    set( MSG_VERSION, MSG_COMMAND, getPktSize() );

    ec = send( socket );
    if ( ec )
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    // Receive command response packet
    ec = recv( socket );
    if ( ec )
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    LOG_INF() << "Received: " << getMessage() << std::endl;

    // Acknowledge command response packet
    setMessage( "" );
    set( MSG_VERSION, MSG_DATA_ACK, getPktSize() );

    ec = send( socket );
    if ( ec )
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    // Send EXIT command packet
    cmd = "EXIT";

    setMessage( cmd );
    set( MSG_VERSION, MSG_EXIT, getPktSize() );

    ec = send( socket );
    if ( ec )
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    // Session completed
    LOG_INF() << "Client session completed successfully!" << std::endl;

    return ec;
}

std::ostream& operator<<( std::ostream& os, const Packet& packet )
{
    os << "Packet: HDR {V:"
       << std::hex << packet.getVersion() << ", T:0x"
       << std::hex << packet.getMsgType() << ", L:"
       << std::dec << packet.getPktSize() << "} | ";

    os << "MSG {L:" << packet.getMsgSize() << "} "
       << packet.getMessage();

    return os;
}

bool Packet::isValidVersion()
{
    return ( getVersion() == MSG_VERSION );
}

bool Packet::isValidMsgType()
{
    return ( getMsgType() == MSG_COMMAND      ||
             getMsgType() == MSG_DATA_ACK     ||
             getMsgType() == MSG_FIN_DATA_ACK ||
             getMsgType() == MSG_EXIT );
}
