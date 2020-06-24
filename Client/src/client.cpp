#include "client.hpp"
#include "packet.hpp"

using std::istreambuf_iterator;
using boost::asio::ip::address;
using boost::system::error_code;
using boost::asio::placeholders::error;


Client::Client( const string         ip,
                const unsigned short port )
                :
                _endpoint( address::from_string( ip ), port ),
                _socket( _ios )
{
    LOG_INF() << "Initiating client..." << endl;
    LOG_INF() << "Got server end-point: [" << _endpoint << "]" << endl;
}

Client::~Client()
{
    stop();

    LOG_INF() << "Client exited successfully!" << endl;
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

    LOG_INF() << "Connecting to server..." << endl;

    try
    {
        error_code ec;

        _socket.connect( _endpoint, ec );
        if ( ec )
        {
            LOG_ERR() << "Error: " << ec.message() << endl;
            isConnected = false;
        }
    }
    catch ( exception& ex )
    {
        LOG_ERR() << "Exception: " << ex.what() << endl;
        isConnected = false;
    }

    if ( isConnected )
    {
        LOG_INF() << "Successfully connected to the server!" << endl;
    }
    else
    {
        LOG_ERR() << "Unable to connect to the server!" << endl;
    }

    return isConnected;
}
