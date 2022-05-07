#include "common.hpp"
#include "packet.hpp"
#include "utilities.hpp"

using boost::asio::buffer;
using boost::asio::read;
using boost::asio::streambuf;
using boost::asio::transfer_exactly;
using boost::asio::write;

packet::packet() noexcept
    : m_header{MSG_VERSION, MSG_DEFAULT, 0},
      m_message{}
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

    const unsigned int bytes_to_read = get_length() - get_header_size();
    if (bytes_to_read > 0)
    {
        // lock_stream();
        // LOG_INF() << "Read Size: " << bytes_to_read << std::endl;
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
    // Send welcome message to client
    auto ec = welcome_client(socket);
    if (ec)
    {
        return ec;
    }

    // Communicate with client using defined protocol
    do
    {
        ec = recv(socket);
        if (!ec)
        {
            if (is_valid_version() && is_valid_msg_type())
            {
                if (get_message_type() == MSG_EXIT)
                {
                    lock_stream();
                    LOG_INF() << "EXIT packet received! -> " << *this << std::endl;
                    unlock_stream();
                    break;
                }
                else if (get_message_type() == MSG_COMMAND)
                {
                    ec = process_client_command(get_message(), socket);

                    lock_stream();
                    LOG_INF() << "COMMAND PROCESSING COMPLETED!" << std::endl;
                    unlock_stream();
                }
            }
            else
            {
                lock_stream();
                LOG_ERR() << "Invalid message format! -> " << *this << std::endl;
                unlock_stream();
            }
        }
    } while (!ec && socket.is_open() &&
             get_message_type() != MSG_EXIT &&
             get_message() != "EXIT");

    lock_stream();
    LOG_INF() << "Session ended with client!" << std::endl;
    unlock_stream();

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

error_code packet::welcome_client(tcp::socket &socket) noexcept
{
    std::ostringstream ss;
    ss << "Welcome! [" << get_peer_ip(socket) << ":" << get_peer_port(socket) << "]";

    set_message(ss.str());
    set(MSG_VERSION, MSG_WELCOME, get_packet_size());

    const auto ec = send(socket);
    if (ec)
    {
        lock_stream();
        LOG_ERR() << ec.message() << std::endl;
        unlock_stream();
    }

    lock_stream();
    LOG_INF() << "Welcome message sent! " << *this << std::endl;
    unlock_stream();

    return {};
}

error_code packet::process_client_command(const std::string cmd, tcp::socket &socket) noexcept
{
    // For now, only "ls" command is hard-coded!

    std::string cmd_output;
    unsigned int msg_type;

    // Execute command and form output message
    if (std::system(cmd.data()) == boost::system::errc::success)
    {
        cmd_output = "File is present!";
        msg_type = MSG_FILE_PRESENT;
    }
    else
    {
        cmd_output = "File is NOT present!";
        msg_type = MSG_FILE_NOT_PRESENT;
    }

    // Form output packet
    set_message(cmd_output);
    set(MSG_VERSION, msg_type, get_packet_size());

    // Send command output
    auto ec = send(socket);
    if (ec)
    {
        lock_stream();
        LOG_ERR() << ec.message() << std::endl;
        unlock_stream();
    }

    lock_stream();
    LOG_INF() << "Command output sent!" << std::endl;
    unlock_stream();

    // Acknowledge client response
    ec = acknowledge_client_response(socket);

    return ec;
}

error_code packet::acknowledge_client_response(tcp::socket &socket) noexcept
{
    const auto ec = recv(socket);
    if (ec)
    {
        return ec;
    }

    if (is_valid_version() && is_valid_msg_type())
    {
        if (get_message_type() == MSG_DATA_ACK ||
            get_message_type() == MSG_FIN_DATA_ACK)
        {
            lock_stream();
            LOG_INF() << "Acknowledgment received from client!" << std::endl;
            unlock_stream();
        }
        else
        {
            lock_stream();
            LOG_ERR() << "Invalid message format! -> " << *this << std::endl;
            unlock_stream();
        }
    }

    return ec;
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
