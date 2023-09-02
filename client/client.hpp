#pragma once

#include <boost/asio.hpp>

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;

class client_t final
{
public:
    client_t(const std::string ip, const unsigned short port) noexcept;
    ~client_t() noexcept;

    bool start() noexcept;

private:
    bool connect() noexcept;
    bool process() noexcept;
    void disconnect() noexcept;

    io_service m_io_service;
    tcp::endpoint m_endpoint;
    tcp::socket m_socket;
};
