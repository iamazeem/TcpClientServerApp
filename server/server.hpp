#ifndef INCLUDE_SERVER_HPP_
#define INCLUDE_SERVER_HPP_

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "session.hpp"

using boost::shared_ptr;
using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::system::error_code;
using boost::enable_shared_from_this;

class Server final
{
public:
    // enum class Defaults : unsigned int { EXECUTOR_THREADS = SERVER_THREADS };

    Server( const std::string    ip,
            const unsigned short port,
            const unsigned int   nExecutorThreads );

    ~Server();

    void start();
    void stop ();

private:
    void WorkerThreadCallback( shared_ptr<io_service> ios );

    void acceptHandler( shared_ptr<Session> thisSession, const error_code& ec );
    void acceptNewConn();

    shared_ptr<io_service>          _iosAcceptors;
    shared_ptr<io_service::work>    _wrkAcceptors;

    shared_ptr<io_service>          _iosExecutors;
    shared_ptr<io_service::work>    _wrkExecutors;

    boost::thread_group             _thgExecutors;

    tcp::endpoint                   _endpoint;
    tcp::acceptor                   _acceptor;

    shared_ptr<Session>             _newSession;

    boost::asio::signal_set         _signals;
};

#endif /* INCLUDE_SERVER_HPP_ */
