#include <exception>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/system/error_code.hpp>

#include "spdlog/spdlog.h"

#include "constants.hpp"
#include "server.hpp"
#include "utils.hpp"

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

    spdlog::info("starting [{}:{}]", ip, port);

    for (unsigned int i = 0; i < num_threads; ++i)
    {
        m_executors_thread_group.create_thread(boost::bind(&server::worker_thread_callback,
                                                           this,
                                                           m_ios_executors));
    }

    m_acceptor.async_accept(m_session->get_socket(),
                            boost::bind(&server::accept_handler, this, m_session, error));

    spdlog::info("started");
}

server::~server() noexcept
{
    stop();
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
    spdlog::info("server stopped successfully");
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
                spdlog::error("error: {}", ec.message());
            }
        }
        catch (const std::exception &e)
        {
            spdlog::error("exception: {}", e.what());
        }
    }
}

void server::accept_handler(boost::shared_ptr<session> this_session, const error_code &ec) noexcept
{
    if (!ec)
    {
        const auto client_ip = get_peer_ip(this_session->get_socket());
        const auto client_port = get_peer_port(this_session->get_socket());
        spdlog::info("new client connected [{}:{}]", client_ip, client_port);

        m_ios_executors->post(boost::bind(&session::start, this_session));

        m_session = boost::make_shared<session>(m_ios_executors);
        m_acceptor.async_accept(m_session->get_socket(),
                                boost::bind(&server::accept_handler, this, m_session, error));
    }
}
