#include "client.hpp"

using boost::asio::ip::address;
using boost::system::error_code;
using boost::asio::placeholders::error;


Client::Client( const string       ip,
                const unsigned int port )
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

void Client::start( void )
{
    if ( !connectToServer() ) { return; }

    initiateSession();
}

void Client::stop ( void )
{
    _socket.close();
}

bool Client::connectToServer( void )
{
    bool isConnected = true;

    LOG_INF() << "Connecting to server..." << endl;

    try
    {
        error_code ec;

        _socket.connect( _endpoint, ec );
        if ( ec )
        {
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

void Client::initiateSession( void )
{
    LOG_INF() << "Initiated session with the server!" << endl;

    boost::asio::streambuf readBuf;
    string dataReceived;

    boost::asio::read_until( _socket, readBuf, "\n" );

    std::istream is(&readBuf);
    is >> dataReceived;

    LOG_INF() << "Received: " << dataReceived << endl;

    dataReceived += "\n";
    boost::asio::write( _socket, boost::asio::buffer(dataReceived) );

    LOG_INF() << "Session ended with the server!" << endl;
}
