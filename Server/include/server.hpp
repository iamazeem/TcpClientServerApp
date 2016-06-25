#ifndef INCLUDE_SERVER_HPP_
#define INCLUDE_SERVER_HPP_

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "session.hpp"

using std::string;
using boost::shared_ptr;
using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::system::error_code;
using boost::enable_shared_from_this;


class Server : public boost::noncopyable
{
public:
    enum class Defaults : unsigned int { EXECUTOR_THREADS = SERVER_THREADS };

    Server( const string       ip,
            const unsigned int port,
            const unsigned int nExecutorThreads );

    ~Server();

    void start( void );
    void stop ( void );

private:
    void WorkerThreadCallback( shared_ptr<io_service> ios );

    void acceptHandler( shared_ptr<Session> thisSession, const error_code& ec );
    void acceptNewConn( void );

    shared_ptr<io_service>          _iosAcceptors;
    shared_ptr<io_service::work>    _wrkAcceptors;

    shared_ptr<io_service>          _iosExecutors;
    shared_ptr<io_service::work>    _wrkExecutors;

    boost::thread_group             _thgExecutors;

    tcp::endpoint                   _endpoint;
    tcp::acceptor                   _acceptor;

    shared_ptr<Session>             _newSession;
};


#endif /* INCLUDE_SERVER_HPP_ */
