#pragma once

#include <string>
#include <sstream>
#include <cstdint>

class message_t final
{
public:
    static constexpr uint16_t version = 123;

    enum class type_t : uint16_t
    {
        invalid,
        welcome,
        command_request,
        command_response,
        exit
    };

    class header_t final
    {
    public:
        void set(const uint16_t version,
                 const type_t type,
                 const uint32_t payload_size) noexcept
        {
            m_version = version;
            m_type = type;
            m_payload_size = payload_size;
        }

        uint16_t get_version() const noexcept { return m_version; }
        type_t get_type() const noexcept { return m_type; }
        uint32_t get_payload_size() const noexcept { return m_payload_size; }

        std::string get_type_as_string() const noexcept
        {
            switch (get_type())
            {
            case type_t::invalid:
                return "invalid";
            case type_t::welcome:
                return "welcome";
            case type_t::command_request:
                return "command_request";
            case type_t::command_response:
                return "command_response";
            case type_t::exit:
                return "exit";
            default:
                return "unknown [" + std::to_string(static_cast<uint16_t>(get_type())) + "]";
            }
        }

        bool is_valid_version() const noexcept { return (get_version() == version); }
        bool is_valid_type() const noexcept
        {
            switch (get_type())
            {
            case type_t::welcome:
            case type_t::command_request:
            case type_t::command_response:
            case type_t::exit:
                return true;
            default:
                return false;
            }
        }

        bool is_valid() const noexcept { return is_valid_version() && is_valid_type(); }

        std::string dump() const noexcept
        {
            std::ostringstream oss;
            oss << "{version: "
                << get_version() << ", type: "
                << get_type_as_string() << ", payload size: "
                << get_payload_size() << "}";
            return oss.str();
        }

    private:
        uint16_t m_version{version};
        type_t m_type{type_t::invalid};
        uint32_t m_payload_size{0};
    };

    message_t() = default;

    message_t(const header_t &header, const std::string &payload) noexcept
        : m_header{header},
          m_payload{payload}
    {
    }

    void set(const type_t type, const std::string &payload) noexcept
    {
        m_header.set(version, type, payload.size());
        m_payload = payload;
    }

    const header_t &get_header() const noexcept { return m_header; }
    std::string get_payload() const noexcept { return m_payload; }

    std::string dump() const noexcept
    {
        if (m_payload.empty())
        {
            return m_header.dump();
        }
        return m_header.dump() + " | payload: [" + m_payload + "]";
    }

private:
    header_t m_header{};
    std::string m_payload{};
};
