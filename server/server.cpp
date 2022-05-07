#include <exception>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/system/error_code.hpp>

#include "common.hpp"
#include "server.hpp"
#include "utilities.hpp"

using boost::shared_ptr;
using boost::asio::ip::address;
using boost::asio::placeholders::error;
using boost::system::error_code;

server::server(const std::string ip, const unsigned short port,
               const unsigned int num_threads) noexcept
    : m_ios_acceptors{boost::make_shared<io_service>()},
      m_ios_work_acceptors{boost::make_shared<io_service::work>(*m_ios_acceptors)},
      m_ios_executors{boost::make_shared<io_service>()},
      m_ios_work_executors{boost::make_shared<io_service::work>(*m_ios_executors)},
      m_endpoint{address::from_string(ip), port},
      m_acceptor{*m_ios_acceptors, m_endpoint},
      m_session{boost::make_shared<session>(m_ios_executors)},
      m_signals{*m_ios_acceptors, SIGINT, SIGTERM}
{
    // Add signal handling for graceful termination (CTRL + C)
    m_signals.async_wait(boost::bind(&server::stop, this));

    LOG_INF() << "Initiating server..." << std::endl;
    for (unsigned int i = 0; i < num_threads; ++i)
    {
        m_executors_thread_group.create_thread(boost::bind(&server::worker_thread_callback,
                                                           this,
                                                           m_ios_executors));
    }

    m_acceptor.async_accept(m_session->get_socket(),
                            boost::bind(&server::accept_handler, this, m_session, error));

    lock_stream();
    LOG_INF() << "Server started! [" << m_endpoint << "]" << std::endl;
    unlock_stream();
}

server::~server() noexcept
{
    stop();

    lock_stream();
    LOG_INF() << "Server closed successfully! Bye bye! :)" << std::endl;
    unlock_stream();
}

void server::start() noexcept
{
    m_ios_acceptors->run();
}

void server::stop() noexcept
{
    if (!m_ios_acceptors->stopped())
    {
        m_ios_acceptors->stop();
    }

    if (!m_ios_executors->stopped())
    {
        m_ios_executors->stop();
        // m_executors_thread_group.interrupt_all();
        // m_executors_thread_group.join_all();
    }
}

// Utility methods

void server::worker_thread_callback(boost::shared_ptr<io_service> ios) noexcept
{
    while (true)
    {
        try
        {
            error_code ec;
            ios->run(ec);
            if (ec)
            {
                lock_stream();
                LOG_ERR() << " Error: " << ec.message() << std::endl;
                unlock_stream();
            }
        }
        catch (const std::exception &e)
        {
            lock_stream();
            LOG_ERR() << " Exception: " << e.what() << std::endl;
            unlock_stream();
        }
    }
}

void server::accept_handler(boost::shared_ptr<session> this_session, const error_code &ec) noexcept
{
    if (!ec)
    {
        LOG_INF() << "Connection established with client! ["
                  << get_peer_ip(this_session->get_socket()) << ":"
                  << get_peer_port(this_session->get_socket()) << "]" << std::endl;

        m_ios_executors->post(boost::bind(&session::start, this_session));

        m_session = boost::make_shared<session>(m_ios_executors);
        m_acceptor.async_accept(m_session->get_socket(),
                                boost::bind(&server::accept_handler, this, m_session, error));
    }
}
