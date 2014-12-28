/*
* Harp, an efficient IO library for chemistry file formats
* Copyright (C) 2014 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

/*! @file logging.hpp
* Log utilities and configuration
*/

#ifndef HARP_LOGGING_H
#define HARP_LOGGING_H

#include <string>
#include <ostream>

namespace harp {
class Logger {

public:

    typedef enum{
        ERROR,
        WARNING,
        INFO,
        DEBUG
    } LogLevel;

    ~Logger();

    //! Set the logging level
    static void set_level(LogLevel level);
    //! Get the current logging level
    static LogLevel get_level(void){return instance.level;}
    //! Set the file for logging
    static void set_log_file(const std::string &filename);
    //! Set the file for logging, C-version
    static void set_log_file(const char* filename);
    //! Make the logger output to stdout
    static void log_to_stdout(void);
    //! Make the logger output to stderr
    static void log_to_stderr(void);

    //! Get the singleton out stream
    static std::ostream& out(LogLevel _level);

private:
    //! Close the log file if it exists.
    void close(void);
    //! Return a stream to write the log
    std::ostream& get_stream(LogLevel _level);
    //! Constructor
    Logger();

    //! Singleton instance
    static Logger instance;

    //! Logging level
    LogLevel level;
    //! Current log stream
    std::ostream *os;
    //! Is the current stream a file ?
    bool is_file;
};

} // namespace harp

//! LOG macro to send a message.
#define LOG(level) harp::Logger::out(harp::Logger::level)

#endif
