#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using boost::shared_ptr;
using boost::asio::io_service;
using boost::asio::ip::tcp;

class session final
{
public:
    session(shared_ptr<io_service> ios) noexcept;

    void start() noexcept;
    void stop() noexcept;

    tcp::socket &get_socket() noexcept { return m_socket; }

private:
    void process() noexcept;
    bool welcome_client() noexcept;
    bool process_command(const std::string& cmd) noexcept;

    shared_ptr<io_service> m_io_service;
    tcp::socket m_socket;
};
