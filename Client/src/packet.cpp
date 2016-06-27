#include "common.hpp"
#include "packet.hpp"


Packet::Packet() : _header{ MSG_VERSION, MSG_DEFAULT, 0 }, _message{""}
{
}

Packet::Packet( const unsigned int version,
                const unsigned int type,
                const unsigned int length,
                const string&      message )
                :
                _header { version, type, length },
                _message{ message }
{
}

const unsigned int Packet::getVersion( void ) const { return _header._version;  }
const unsigned int Packet::getMsgType   ( void ) const { return _header._type;     }
const unsigned int Packet::getLength ( void ) const { return _header._length;   }
const Header&      Packet::getHeader ( void ) const { return _header;           }
const string&      Packet::getMessage( void ) const { return _message;          }
const unsigned int Packet::getHdrSize( void ) const { return sizeof(_header);   }
const unsigned int Packet::getMsgSize( void ) const { return _message.length(); }

const unsigned int Packet::getPktSize( void ) const { return getHdrSize() + getMsgSize(); }

void Packet::setVersion( const unsigned int version ) { _header._version = version; }
void Packet::setType   ( const unsigned int type    ) { _header._type    = type;    }
void Packet::setLength ( const unsigned int length  ) { _header._length  = length;  }
void Packet::setMessage( const string&      message ) { _message         = message; }

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
                   const string&      message )
{
    set( version, type, length );
    setMessage( message );
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
