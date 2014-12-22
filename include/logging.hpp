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

    //! Construct a Logger with the specific level
    Logger(LogLevel level = WARNING);
    ~Logger();

    //! Set the logging level
    void set_level(LogLevel level);
    //! Get the current logging level
    LogLevel getLevel(void){return level;}
    //! Set the file for logging
    void set_log_file(const std::string &filename);
    //! Make the logger output to stdout
    void log_to_stdout(void);
    //! Make the logger output to stderr
    void log_to_stderr(void);
    //! Return a stream to write the log
    std::ostream& out(LogLevel _level);

private:
    //! Close the log file if it exists.
    void close(void);

    //! Logging level
    LogLevel level;
    //! Current log stream
    std::ostream *os;
    //! Is the current stream a file ?
    bool is_file;
};


//! Global logger variable
extern Logger logger;

//! LOG macro to send a message.
#define LOG(level) logger.out(Logger::level)

} // namespace harp

#endif
