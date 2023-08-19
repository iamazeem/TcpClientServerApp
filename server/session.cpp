#include <exception>
#include "spdlog/spdlog.h"
#include "session.hpp"

session::session(shared_ptr<io_service> ios) noexcept
    : m_io_service{ios},
      m_socket{*m_io_service},
      m_packet{}
{
}

tcp::socket &session::get_socket() noexcept
{
    return m_socket;
}

void session::start() noexcept
{
    try
    {
        m_packet.process(m_socket);
        stop();
    }
    catch (const std::exception &e)
    {
        spdlog::error("exception: {}", e.what());
    }
}

void session::stop() noexcept
{
    m_socket.shutdown(boost::asio::socket_base::shutdown_both);
    m_socket.close();
}
