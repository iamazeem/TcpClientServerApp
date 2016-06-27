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

void Client::initiateSession( void )
{
    LOG_INF() << "Initiated session with the server!" << endl;

    error_code ec;
    boost::asio::streambuf readBuf;

    Header header;

    boost::asio::read( _socket, boost::asio::buffer( &header, sizeof(header) ) );

    LOG_INF() << "Header: " << sizeof(header) << " : "
              << std::hex << header._version  << ", "
              << std::hex << header._type     << ", "
              << std::dec << header._length
              << endl;

    const unsigned int readSize = header._length - sizeof(header);

    LOG_INF() << "Read Size: " << readSize << endl;

    boost::asio::read( _socket, readBuf, boost::asio::transfer_exactly(readSize), ec );
    if ( ec )
    {
        LOG_ERR() << "Error: " << ec.message() << endl;
    }

    string dataReceived( ( istreambuf_iterator< char >( &readBuf ) ), istreambuf_iterator< char >() );

    LOG_INF() << "Data: " << dataReceived.length() << " : " << dataReceived << endl;

    LOG_INF() << "Session ended with the server!" << endl;
}
