#include "spdlog/spdlog.h"

#include "client.hpp"
#include "message.hpp"
#include "utils.hpp"

using boost::asio::ip::address;
using boost::asio::placeholders::error;
using boost::system::error_code;

client_t::client_t(const std::string ip, const unsigned short port) noexcept
    : m_endpoint{address::from_string(ip), port},
      m_socket{m_io_service}
{
}

client_t::~client_t() noexcept
{
    disconnect();
}

bool client_t::start() noexcept
{
    if (!connect())
    {
        return false;
    }
    if (!process())
    {
        return false;
    }
    return true;
}

bool client_t::connect() noexcept
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

bool client_t::process() noexcept
{
    // Receive welcome message
    spdlog::info("receiving welcome message from server");
    const auto welcome_msg = recv(m_socket);
    if (welcome_msg.get_header().get_type() != message_t::type_t::welcome)
    {
        spdlog::info("invalid welcome message type, {}", welcome_msg.dump());
        return false;
    }
    spdlog::info("welcome message received, {}", welcome_msg.dump());

    // Send command request message
    const auto cmd = "ls test";
    message_t cmd_request_msg;
    cmd_request_msg.set(message_t::type_t::command_request, cmd);
    spdlog::info("sending command request [{}]", cmd_request_msg.dump());
    if (!send(m_socket, cmd_request_msg))
    {
        spdlog::error("error while sending command request");
        return false;
    }
    spdlog::info("command request sent");

    // Receive command response message
    spdlog::info("receiving command response from server");
    const auto cmd_response_msg = recv(m_socket);
    if (cmd_response_msg.get_header().get_type() != message_t::type_t::command_response)
    {
        spdlog::info("invalid command response type, {}", cmd_response_msg.dump());
        return false;
    }
    spdlog::info("command response received: {}", cmd_response_msg.dump());

    // Send EXIT message
    message_t exit_msg;
    exit_msg.set(message_t::type_t::exit, "EXIT");
    spdlog::info("sending exit message to server [{}]", exit_msg.dump());
    if (!send(m_socket, exit_msg))
    {
        spdlog::error("error while sending command request");
        return false;
    }
    spdlog::info("exit message sent");

    spdlog::info("session completed");
    return true;
}

void client_t::disconnect() noexcept
{
    if (m_socket.is_open())
    {
        m_socket.close();
        spdlog::info("disconnected");
    }
}
