#pragma once

#include "common.hpp"

#define LOG_INF() \
    logger::log(logger::level::INF, __FILE__, __FUNCTION__, __LINE__)

#define LOG_WRN() \
    logger::log(logger::level::WRN, __FILE__, __FUNCTION__, __LINE__)

#define LOG_ERR() \
    logger::log(logger::level::ERR, __FILE__, __FUNCTION__, __LINE__)

class logger final
{
public:
    enum class level : unsigned int
    {
        INF,
        WRN,
        ERR
    };

    static inline std::ostream &log(const level level,
                                    const char *file,
                                    const char *func,
                                    unsigned int line)
    {
        std::string level_str;
        switch (level)
        {
        case level::INF:
            level_str = "[INF]";
            break;

        case level::WRN:
            level_str = "[WRN]";
            break;

        case level::ERR:
            level_str = "[ERR]";
            break;
        }

        std::cout << level_str << " : "
                  << file << ": "
                  << func << "(): "
                  << line << ": ";

        return std::cout;
    }
};
