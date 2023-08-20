#include <boost/system/error_code.hpp>

#include "spdlog/spdlog.h"
#include "utils.hpp"

using boost::asio::buffer;
using boost::asio::read;
using boost::asio::streambuf;
using boost::asio::transfer_exactly;
using boost::asio::write;
using boost::system::error_code;

std::string get_peer_ip(const tcp::socket &socket) noexcept
{
    return socket.remote_endpoint().address().to_string();
}

unsigned short get_peer_port(const tcp::socket &socket) noexcept
{
    return socket.remote_endpoint().port();
}

message_t recv(tcp::socket &socket) noexcept
{
    spdlog::debug("receiving message");

    message_t::header_t header;

    error_code ec;
    read(socket, buffer(&header, sizeof(header)), transfer_exactly(sizeof(header)), ec);
    if (ec)
    {
        spdlog::error("header read error, {}", ec.message());
        return {};
    }

    spdlog::debug("header: {}", header.dump());

    if (!header.is_valid())
    {
        spdlog::error("invalid header");
        return {};
    }

    if (header.get_payload_size() == 0)
    {
        return {header, ""};
    }
 
    streambuf payload_buffer;
    read(socket, payload_buffer, transfer_exactly(header.get_payload_size()), ec);
    if (ec)
    {
        spdlog::error("payload read error, {}", ec.message());
        return {};
    }

    const std::string payload((std::istreambuf_iterator<char>(&payload_buffer)),
                              std::istreambuf_iterator<char>());

    const auto message = message_t{header, payload};
    spdlog::debug("message received: {}", message.dump());
    return message;
}

bool send(tcp::socket &socket, const message_t &message) noexcept
{
    spdlog::debug("sending message: {}", message.dump());

    const auto header = message.get_header();
    error_code ec;
    write(socket, buffer(&header, sizeof(header)), transfer_exactly(sizeof(header)), ec);
    if (ec)
    {
        spdlog::error("header send error, {}", ec.message());
        return false;
    }

    if (header.get_payload_size() > 0)
    {
        const auto payload = message.get_payload();
        write(socket, buffer(payload, payload.size()), transfer_exactly(payload.size()), ec);
        if (ec)
        {
            spdlog::error("payload send error, {}", ec.message());
            return false;
        }
    }

    spdlog::debug("message sent [{}:{}]", get_peer_ip(socket), get_peer_port(socket));
    return true;
}
