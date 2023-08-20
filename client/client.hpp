#pragma once

#include <boost/asio.hpp>

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;

class client final
{
public:
    client(const std::string ip, const unsigned short port) noexcept;
    ~client() noexcept;

    void start() noexcept;

private:
    bool connect() noexcept;
    void disconnect() noexcept;
    void process() noexcept;

    io_service m_io_service;
    tcp::endpoint m_endpoint;
    tcp::socket m_socket;
};
