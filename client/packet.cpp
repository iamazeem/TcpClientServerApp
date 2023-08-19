#include <sstream>
#include "spdlog/spdlog.h"

#include "constants.hpp"
#include "packet.hpp"
#include "utils.hpp"

using boost::asio::buffer;
using boost::asio::read;
using boost::asio::streambuf;
using boost::asio::transfer_exactly;
using boost::asio::write;

packet::packet() noexcept
    : m_header{MSG_VERSION, MSG_DEFAULT, 0}, m_message{}
{
}

packet::packet(const unsigned int version,
               const unsigned int type,
               const unsigned int length,
               const std::string message) noexcept
    : m_header{version, type, length},
      m_message{message}
{
}

unsigned int packet::get_version() const noexcept { return m_header.m_version; }
unsigned int packet::get_message_type() const noexcept { return m_header.m_type; }
unsigned int packet::get_length() const noexcept { return m_header.m_length; }
unsigned int packet::get_header_size() const noexcept { return sizeof(m_header); }
unsigned int packet::get_message_size() const noexcept { return m_message.length(); }
unsigned int packet::get_packet_size() const noexcept { return get_header_size() + get_message_size(); }
std::string packet::get_message() const noexcept { return m_message; }

void packet::set_version(const unsigned int version) noexcept { m_header.m_version = version; }
void packet::set_type(const unsigned int type) noexcept { m_header.m_type = type; }
void packet::set_length(const unsigned int length) noexcept { m_header.m_length = length; }
void packet::set_message(const std::string message) noexcept { m_message = message; }

void packet::set(const unsigned int version,
                 const unsigned int type,
                 const unsigned int length) noexcept
{
    set_version(version);
    set_type(type);
    set_length(length);
}

void packet::set(const unsigned int version,
                 const unsigned int type,
                 const unsigned int length,
                 const std::string message) noexcept
{
    set(version, type, length);
    set_message(message);
}

error_code packet::recv(tcp::socket &socket) noexcept
{
    error_code ec;
    read(socket, buffer(&m_header, get_header_size()), transfer_exactly(get_header_size()), ec);
    if (ec)
    {
        spdlog::error("header read error, {}", ec.message());
        return ec;
    }

    spdlog::info("header: {}", dump());

    const auto bytes_to_read = get_length() - get_header_size();
    if (bytes_to_read > 0)
    {
        spdlog::info("bytes to read: {}", bytes_to_read);

        streambuf read_buffer;
        read(socket, read_buffer, transfer_exactly(bytes_to_read), ec);
        if (ec)
        {
            spdlog::error("message read error, {}", ec.message());
            return ec;
        }

        const std::string data_received((std::istreambuf_iterator<char>(&read_buffer)),
                                        std::istreambuf_iterator<char>());
        set_message(data_received);

        spdlog::info("received: {}", dump());
    }

    return ec;
}

error_code packet::send(tcp::socket &socket) noexcept
{
    spdlog::info("sending packet: {}", dump());

    // Send packet header
    error_code ec;
    write(socket, buffer(&m_header, get_header_size()), transfer_exactly(get_header_size()), ec);
    if (ec)
    {
        spdlog::error("could not send header [{}:{}]", get_peer_ip(socket), get_peer_port(socket));
        return ec;
    }

    // Send packet message
    write(socket, buffer(get_message(), get_message_size()), transfer_exactly(get_message_size()), ec);
    if (ec)
    {
        spdlog::error("could not send message [{}:{}]", get_peer_ip(socket), get_peer_port(socket));
        return ec;
    }

    // Reset packet message
    set_message("");

    spdlog::info("packet sent [{}:{}]", get_peer_ip(socket), get_peer_port(socket));
    return ec;
}

error_code packet::process(tcp::socket &socket) noexcept
{
    spdlog::info("receiving welcome message from server");

    auto ec = recv(socket);
    if (ec)
    {
        spdlog::error("error while receiving welcome message, error: {}", ec.message());
        return ec;
    }

    spdlog::info("welcome message received: {}", get_message());

    // Send command packet
    set_message("ls ~/Test.cpp");
    set(MSG_VERSION, MSG_COMMAND, get_packet_size());

    spdlog::info("sending command to server [{}]", dump());

    ec = send(socket);
    if (ec)
    {
        spdlog::error("error while sending command to server, error: {}", ec.message());
        return ec;
    }

    spdlog::info("receiving command output from server");

    ec = recv(socket);
    if (ec)
    {
        spdlog::error("error while receiving command response, error: {}", ec.message());
        return ec;
    }

    spdlog::info("command response received: {}", get_message());

    // Acknowledge command response packet
    set_message("");
    set(MSG_VERSION, MSG_DATA_ACK, get_packet_size());

    spdlog::info("sending command response ack to server [{}]", dump());

    ec = send(socket);
    if (ec)
    {
        spdlog::error("error while sending command response ack, error: {}", ec.message());
        return ec;
    }

    // Send EXIT command packet
    set_message("EXIT");
    set(MSG_VERSION, MSG_EXIT, get_packet_size());

    spdlog::info("sending exit command to server [{}]", dump());

    ec = send(socket);
    if (ec)
    {
        spdlog::error("error while sending EXIT command, error: {}", ec.message());
        return ec;
    }

    spdlog::info("session completed");
    return ec;
}

std::string packet::dump() const noexcept
{
    std::ostringstream oss;
    oss << "Packet: HDR {V:"
        << std::hex << get_version() << ", T:0x"
        << std::hex << get_message_type() << ", L:"
        << std::dec << get_packet_size() << "} | ";

    oss << "MSG {L:" << get_message_size() << "} "
        << get_message();
    return oss.str();
}

bool packet::is_valid_version() const noexcept
{
    return (get_version() == MSG_VERSION);
}

bool packet::is_valid_msg_type() const noexcept
{
    return (get_message_type() == MSG_COMMAND ||
            get_message_type() == MSG_DATA_ACK ||
            get_message_type() == MSG_FIN_DATA_ACK ||
            get_message_type() == MSG_EXIT);
}
