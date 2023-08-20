#include <sstream>
#include "spdlog/spdlog.h"
#include "session.hpp"
#include "message.hpp"
#include "utils.hpp"

session_t::session_t(shared_ptr<io_service> ios) noexcept
    : m_io_service{ios},
      m_socket{*m_io_service}
{
}

void session_t::start() noexcept
{
    spdlog::info("session started");
    process();
    stop();
    spdlog::info("session completed");
}

void session_t::stop() noexcept
{
    m_socket.shutdown(boost::asio::socket_base::shutdown_both);
    m_socket.close();
}

void session_t::process() noexcept
{
    if (!welcome_client())
    {
        return;
    }

    do
    {
        spdlog::info("receiving command message from client");
        const auto msg = recv(m_socket);
        if ((msg.get_header().get_type() == message_t::type_t::exit) &&
            (msg.get_payload() == "EXIT"))
        {
            spdlog::info("exit message received");
            break;
        }
        else if (msg.get_header().get_type() == message_t::type_t::command_request)
        {
            if (!process_command(msg.get_payload()))
            {
                spdlog::info("error while process command request message");
                break;
            }
            spdlog::info("command processing completed");
        }
        else
        {
            spdlog::error("invalid message type received");
            break;
        }
    } while (m_socket.is_open());
}

bool session_t::welcome_client() noexcept
{
    std::ostringstream oss;
    oss << "Welcome! [" << get_peer_ip(m_socket) << ":" << get_peer_port(m_socket) << "]";

    message_t welcome_msg;
    welcome_msg.set(message_t::type_t::welcome, oss.str());
    spdlog::info("sending welcome message to client");
    if (!send(m_socket, welcome_msg))
    {
        spdlog::error("error while sending welcome message");
        return false;
    }
    spdlog::info("welcome message sent");
    return true;
}

bool session_t::process_command(const std::string& cmd) noexcept
{
    spdlog::info("processing command [{}]", cmd);

    const auto exit_status = std::system(cmd.data());
    const auto cmd_response_payload = (exit_status == 0 ? "SUCCESS" : "FAILURE");

    spdlog::info("sending command response message");
    message_t cmd_response_msg;
    cmd_response_msg.set(message_t::type_t::command_response, cmd_response_payload);
    if (!send(m_socket, cmd_response_msg))
    {
        spdlog::error("error while sending command response message");
        return false;
    }
    spdlog::info("command response message sent");
    return true;
}
