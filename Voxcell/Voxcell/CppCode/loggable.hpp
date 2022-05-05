#ifndef _MAKENA_LOGGABLE_HPP_
#define _MAKENA_LOGGABLE_HPP_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>

#ifdef UNIT_TESTS
#include "gtest/gtest_prod.h"
#endif

/**
 * @file loggable.hpp
 *
 * @brief runtime logging utility
 *
 * @reference
 */
namespace Makena {

class Loggable {

  public:

    enum LogLevel {
        OFF,
        ERROR,
        WARNING,
        INFO,
        ALL
    };

    inline Loggable(std::ostream& os = std::cerr);
    inline ~Loggable();

    /** @brief sets the logging level
     *
     *  @param  lvl (in): Log level
     */
    inline void setLogLevel(enum LogLevel lvl);

    inline void log(
        enum LogLevel lvl,
        const char*   _file, 
        const int     _line,
        const char*   fmt...
    ) const;

  protected:

    /** @brief logging output */
    std::ostream&                          mLogStream;

    /** @brief logging level */
    enum LogLevel                          mLogLevel;

};

Loggable::Loggable(std::ostream& os):mLogStream(os),mLogLevel(OFF){;}
Loggable::~Loggable(){;}


void Loggable::setLogLevel(enum LogLevel lvl)
{
    mLogLevel = lvl;
}


void Loggable::log(
    enum LogLevel lvl,
    const char*   _file,
    const int     _line,
    const char*   fmt...
) const {

    if (mLogLevel>0 && mLogLevel>=lvl) {

        char buffer[1024];

        switch (lvl) {

          case ERROR:
            mLogStream << "ERROR: ";
            break;

          case WARNING:
            mLogStream << "WARNING: ";
            break;

          case INFO:
            mLogStream << "INFO: ";
            break;

          default:
            mLogStream << "<unknown>: ";
            break;

        }

        mLogStream << _file << ":[" << _line << "] ";
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, 1024, fmt, args);
        va_end(args);
        mLogStream << buffer << "\n";

    }
}
   
}// namespace Makena

#endif/*_MAKENA_LOGGABLE_HPP_*/
