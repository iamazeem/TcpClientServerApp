#include "spdlog/spdlog.h"

#include "client.hpp"
#include "packet.hpp"

using boost::asio::ip::address;
using boost::asio::placeholders::error;
using boost::system::error_code;

client::client(const std::string ip, const unsigned short port) noexcept
    : m_endpoint{address::from_string(ip), port},
      m_socket{m_io_service}
{
}

client::~client() noexcept
{
    disconnect();
}

void client::start() noexcept
{
    if (connect())
    {
        m_packet.process(m_socket);
    }
}

bool client::connect() noexcept
{
    spdlog::info("connecting [{}:{}]", m_endpoint.address().to_string(), m_endpoint.port());

    error_code ec;
    m_socket.connect(m_endpoint, ec);
    if (ec)
    {
        spdlog::error("failed to connect, error: {}", ec.message());
        return false;
    }

    spdlog::info("connected");
    return true;
}

void client::disconnect() noexcept
{
    if (m_socket.is_open())
    {
        m_socket.close();
        spdlog::info("disconnected");
    }
}
