#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/system/error_code.hpp>

#include "common.hpp"
#include "server.hpp"
#include "utilities.hpp"

using boost::bind;
using boost::shared_ptr;
using boost::make_shared;
using boost::asio::ip::address;
using boost::system::error_code;
using boost::asio::placeholders::error;


/* Interface function definitions */
Server::Server( const string       ip,
                const unsigned int port,
                const unsigned int nExecutorThreads )
                :
                _iosAcceptors{ make_shared<io_service>() },
                _wrkAcceptors{ make_shared<io_service::work>( *_iosAcceptors ) },
                _iosExecutors{ make_shared<io_service>() },
                _wrkExecutors{ make_shared<io_service::work>( *_iosExecutors ) },
                _endpoint    { address::from_string(ip), port },
                _acceptor    { *_iosAcceptors, _endpoint },
                _newSession  { make_shared<Session>( _iosExecutors ) }
{
    LOG_INF() << "Initiating server..." << endl;

    for ( unsigned int i = 0; i < nExecutorThreads; ++i )
    {
        _thgExecutors.create_thread( bind( &Server::WorkerThreadCallback,
                                           this,
                                           _iosExecutors ) );
    }

    _acceptor.async_accept( _newSession->getSocket(),
                            bind( &Server::acceptHandler, this, _newSession, error ) );

    lockStream();
    LOG_INF() << "Server started! [" << _endpoint << "]" << endl;
    unlockStream();
}

Server::~Server()
{
    stop();
}

void Server::start( void )
{
    _iosAcceptors->run();
}

void Server::stop( void )
{
    lockStream();
    LOG_INF() << "Closing server..." << endl;
    unlockStream();

    _iosAcceptors->stop();
    _iosExecutors->stop();
    _thgExecutors.join_all();

    lockStream();
    LOG_INF() << "Server closed successfully! Bye bye! :)" << endl;
    unlockStream();
}

/** Utility function definitions **/

/* WorkerThread Callback Skeleton */
void Server::WorkerThreadCallback( shared_ptr<io_service> ios )
{
    while ( true )
    {
        try
        {
            error_code errorCode;

            ios->run( errorCode );
            if ( errorCode )
            {
                lockStream();
                LOG_ERR() << " Error: " << errorCode.message() << endl;
                unlockStream();
            }
        }
        catch ( exception& ex )
        {
            lockStream();
            LOG_ERR() << " Exception: " << ex.what() << endl;
            unlockStream();
        }
    }
}

void Server::acceptHandler( shared_ptr<Session> thisSession, const error_code& ec )
{
    if ( !ec )
    {
        LOG_INF() << "Connected to client! ["
                  << getPeerIp( thisSession->getSocket() ) << ":"
                  << getPeerPort( thisSession->getSocket() ) << "]" << endl;

        _iosExecutors->post( bind( &Session::start, thisSession ) );

        _newSession = make_shared<Session>( _iosExecutors );

        _acceptor.async_accept( _newSession->getSocket(),
                                bind( &Server::acceptHandler, this, _newSession, error ) );
    }
}
