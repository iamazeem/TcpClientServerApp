#ifndef INCLUDE_LOGGER_HPP_
#define INCLUDE_LOGGER_HPP_

#include "common.hpp"

/** Logging Macros **/

#define LOG_INF() \
        Logger::log( Logger::Level::INF, __FILE__, __FUNCTION__, __LINE__ )

#define LOG_WRN() \
        Logger::log( Logger::Level::WRN, __FILE__, __FUNCTION__, __LINE__ )

#define LOG_ERR() \
        Logger::log( Logger::Level::ERR, __FILE__, __FUNCTION__, __LINE__ )

/** Logger Class - Definition **/

class Logger
{
public:
    enum class Level : unsigned int { INF, WRN, ERR };

    static inline std::ostream& log( const Level  level,
                                     const char*  file,
                                     const char*  func,
                                     unsigned int line )
    {
        std::string strLogLvl;

        switch ( level )
        {
            case Level::INF:
                strLogLvl = "[INF]";
                break;

            case Level::WRN:
                strLogLvl = "[WRN]";
                break;

            case Level::ERR:
                strLogLvl = "[ERR]";
                break;
        }

        std::cout << strLogLvl << " : "
                  << file << ": " << func << "(): " << line << ": ";

        return std::cout;
    }
};

#endif /* INCLUDE_LOGGER_HPP_ */
