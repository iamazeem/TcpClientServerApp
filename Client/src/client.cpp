#include "client.hpp"
#include "packet.hpp"

using boost::asio::ip::address;
using boost::system::error_code;
using boost::asio::placeholders::error;

Client::Client( const std::string    ip,
                const unsigned short port )
                :
                _endpoint( address::from_string( ip ), port ),
                _socket( _ios )
{
    LOG_INF() << "Initiating client..." << std::endl;
    LOG_INF() << "Got server end-point: [" << _endpoint << "]" << std::endl;
}

Client::~Client()
{
    stop();

    LOG_INF() << "Client exited successfully!" << std::endl;
}

void Client::start()
{
    if ( !connectToServer() ) { return; }

    _packet.processPackets( _socket );

    stop();
}

void Client::stop ()
{
    _socket.close();
}

bool Client::connectToServer()
{
    bool isConnected = true;

    LOG_INF() << "Connecting to server..." << std::endl;

    try
    {
        error_code ec;
        _socket.connect( _endpoint, ec );
        if ( ec )
        {
            LOG_ERR() << "Error: " << ec.message() << std::endl;
            isConnected = false;
        }
    }
    catch ( const std::exception& ex )
    {
        LOG_ERR() << "Exception: " << ex.what() << std::endl;
        isConnected = false;
    }

    if ( isConnected )
    {
        LOG_INF() << "Successfully connected to the server!" << std::endl;
    }
    else
    {
        LOG_ERR() << "Unable to connect to the server!" << std::endl;
    }

    return isConnected;
}
