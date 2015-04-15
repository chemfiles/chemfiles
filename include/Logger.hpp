/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
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

/*!
 * @class Logger Logger.hpp Logger.cpp
 * @brief The Logger class is a singleton class providing logging facilities.
 */
class Logger {
public:
    //! The \c LogLevel control what will be logged and what will be dismissed
    enum LogLevel{
        //! No logging at all
        NONE,
        //! Logging only errors
        ERROR,
        //! Logging errors and warnings
        WARNING,
        //! Logging errors, warnings and informations
        INFO,
        //! Logging everything and debug informations
        DEBUG
    };

    ~Logger();

    //! Set the logging level
    static void level(LogLevel);
    //! Get the current logging level
    static LogLevel level() {return instance.current_level;}
    //! Set the file for logging
    static void log_to_file(const std::string &filename);
    //! Make the logger output to stdout
    static void log_to_stdout();
    //! Make the logger output to stderr
    static void log_to_stderr();
    //! Make the logger output to stdlog
    static void log_to_stdlog();

    //! Get the singleton out stream
    static std::ostream& out(LogLevel level);

private:
    //! Close the log file if it exists.
    void close(void);
    //! Return a stream to write the log
    std::ostream& get_stream(LogLevel level);
    //! Constructor
    Logger();

    //! Copy/move is not permited
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    //! Afectation is not permited
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

    //! Singleton instance
    static Logger instance;

    //! Logging level
    LogLevel current_level;
    //! Current log stream
    std::ostream* os; // A raw pointer is needed to hold reference to the standard streams
    //! Is the current stream a file ?
    bool is_file;
};

} // namespace harp

//! LOG macro to send a message.
#define LOG(level) harp::Logger::out(harp::Logger::level)

#endif
