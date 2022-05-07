#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "session.hpp"

using boost::enable_shared_from_this;
using boost::shared_ptr;
using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;

class server final
{
public:
    server(const std::string ip, const unsigned short port,
           const unsigned int num_threads) noexcept;
    ~server() noexcept;

    void start() noexcept;
    void stop() noexcept;

private:
    void worker_thread_callback(shared_ptr<io_service> ios) noexcept;
    void accept_handler(shared_ptr<session> this_session, const error_code &ec) noexcept;
    void accept_new_connection() noexcept;

    shared_ptr<io_service> m_ios_acceptors;
    shared_ptr<io_service::work> m_ios_work_acceptors;
    shared_ptr<io_service> m_ios_executors;
    shared_ptr<io_service::work> m_ios_work_executors;
    boost::thread_group m_executors_thread_group;

    tcp::endpoint m_endpoint;
    tcp::acceptor m_acceptor;

    shared_ptr<session> m_session;

    boost::asio::signal_set m_signals;
};
