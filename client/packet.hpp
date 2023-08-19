#pragma once

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "message.hpp"

using boost::asio::ip::tcp;
using boost::system::error_code;

class packet final
{
public:
    packet() noexcept;
    packet(const unsigned int version,
           const unsigned int type,
           const unsigned int length,
           const std::string message) noexcept;

    unsigned int get_version() const noexcept;
    unsigned int get_message_type() const noexcept;
    unsigned int get_length() const noexcept;
    unsigned int get_header_size() const noexcept;
    unsigned int get_message_size() const noexcept;
    unsigned int get_packet_size() const noexcept;
    std::string get_message() const noexcept;

    void set_version(const unsigned int version) noexcept;
    void set_type(const unsigned int type) noexcept;
    void set_length(const unsigned int length) noexcept;
    void set_message(const std::string message) noexcept;

    void set(const unsigned int version,
             const unsigned int type,
             const unsigned int length) noexcept;
    void set(const unsigned int version,
             const unsigned int type,
             const unsigned int length,
             const std::string message) noexcept;

    error_code recv(tcp::socket &socket) noexcept;
    error_code send(tcp::socket &socket) noexcept;
    error_code process(tcp::socket &socket) noexcept;

    std::string dump() const noexcept;

private:
    bool is_valid_version() const noexcept;
    bool is_valid_msg_type() const noexcept;

    struct header final
    {
        enum
        {
            VERSION_SIZE = 3,
            TYPE_SIZE = 5,
            LENGTH_SIZE = 16
        };

        unsigned int m_version : VERSION_SIZE;
        unsigned int m_type : TYPE_SIZE;
        unsigned int m_length : LENGTH_SIZE;
    };

    header m_header;
    std::string m_message;
};
