#include "common.hpp"
#include "packet.hpp"
#include "utilities.hpp"

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
        lock_stream();
        LOG_INF() << "Header read error! " << ec.message() << std::endl;
        unlock_stream();

        return ec;
    }

    // lock_stream();
    // LOG_INF() << "Header: "
    //             << std::hex << get_version() << ", "
    //             << std::hex << get_message_type()    << ", "
    //             << std::dec << get_length()  << std::endl;
    // unlock_stream();

    const auto bytes_to_read = get_length() - get_header_size();
    if (bytes_to_read > 0)
    {
        // lock_stream();
        // LOG_INF() << "Bytes to read: " << bytes_to_read << std::endl;
        // unlock_stream();

        streambuf read_buffer;
        read(socket, read_buffer, transfer_exactly(bytes_to_read), ec);
        if (ec)
        {
            LOG_ERR() << "Message read error! " << ec.message() << std::endl;
            return ec;
        }

        const std::string data_received((std::istreambuf_iterator<char>(&read_buffer)),
                                        std::istreambuf_iterator<char>());
        set_message(data_received);

        lock_stream();
        LOG_INF() << "Received: " << *this << std::endl;
        unlock_stream();
    }

    return ec;
}

error_code packet::send(tcp::socket &socket) noexcept
{
    lock_stream();
    LOG_INF() << "Sending packet: " << *this << std::endl;
    unlock_stream();

    // Send packet header
    error_code ec;
    write(socket, buffer(&m_header, get_header_size()), transfer_exactly(get_header_size()), ec);
    if (ec)
    {
        lock_stream();
        LOG_ERR() << "Could not send header! ["
                  << get_peer_ip(socket) << ":"
                  << get_peer_port(socket)
                  << "]" << std::endl;
        unlock_stream();

        return ec;
    }

    // Send packet message
    write(socket, buffer(get_message(), get_message_size()), transfer_exactly(get_message_size()), ec);
    if (ec)
    {
        lock_stream();
        LOG_ERR() << "Could not send message! ["
                  << get_peer_ip(socket) << ":"
                  << get_peer_port(socket)
                  << "]" << std::endl;
        unlock_stream();

        return ec;
    }

    lock_stream();
    LOG_INF() << "Packet sent! ["
              << get_peer_ip(socket) << ":"
              << get_peer_port(socket)
              << "]..." << std::endl;
    unlock_stream();

    // Reset packet message
    set_message("");

    return ec;
}

error_code packet::process(tcp::socket &socket) noexcept
{
    // Receive welcome message from server
    auto ec = recv(socket);
    if (ec)
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    LOG_INF() << "Received: " << get_message() << std::endl;

    // Send command packet
    set_message("ls ~/Test.cpp");
    set(MSG_VERSION, MSG_COMMAND, get_packet_size());

    ec = send(socket);
    if (ec)
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    // Receive command response packet
    ec = recv(socket);
    if (ec)
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    LOG_INF() << "Received: " << get_message() << std::endl;

    // Acknowledge command response packet
    set_message("");
    set(MSG_VERSION, MSG_DATA_ACK, get_packet_size());

    ec = send(socket);
    if (ec)
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    // Send EXIT command packet
    set_message("EXIT");
    set(MSG_VERSION, MSG_EXIT, get_packet_size());

    ec = send(socket);
    if (ec)
    {
        LOG_ERR() << "Error: " << ec.message() << std::endl;
        return ec;
    }

    LOG_INF() << "Client session completed successfully!" << std::endl;

    return ec;
}

std::ostream &operator<<(std::ostream &os, const packet &packet)
{
    os << "Packet: HDR {V:"
       << std::hex << packet.get_version() << ", T:0x"
       << std::hex << packet.get_message_type() << ", L:"
       << std::dec << packet.get_packet_size() << "} | ";

    os << "MSG {L:" << packet.get_message_size() << "} "
       << packet.get_message();

    return os;
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
