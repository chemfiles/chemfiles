/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_LOGGING_H
#define CHEMFILES_LOGGING_H

#include <string>
#include <fstream>
#include <functional>

#include "chemfiles/exports.hpp"

namespace chemfiles {

enum class LogLevel {
    //! Only logging errors
    ERROR = 0,
    //! Logging errors and warnings
    WARNING = 1,
    //! Logging errors, warnings and informations
    INFO = 2,
    //! Logging everything and debug informations
    DEBUG = 3
};

/*!
 * @class Logger Logger.hpp Logger.cpp
 * @brief The Logger class is a singleton class providing logging facilities.
 */
class CHFL_EXPORT Logger {
public:
    //! Where the log message should go
    enum LogBackend {
        //! Do not log anything
        SILENT,
        //! Log to stdout
        STDOUT,
        //! Log to stderr
        STDERR,
        //! Log to a file
        FILE,
        //! Log to the C callback
        CALLBACK
    };

    //! Callback function type for logging the `message` at `level`
    using logging_cb = std::function<void(LogLevel level, const std::string& message)>;

    //! Log a `message` if the `level` is lower than the maximal curent logging level
    static void log(LogLevel level, std::string message);
    //! Log `message` at error level.
    static void error(const std::string& message) {
        Logger::log(LogLevel::ERROR, message);
    }
    //! Log `message` at warning level.
    static void warn(const std::string& message) {
        Logger::log(LogLevel::WARNING, message);
    }
    //! Log `message` at info level.
    static void info(const std::string& message) {
        Logger::log(LogLevel::INFO, message);
    }
    //! Log `message` at debug level.
    static void debug(const std::string& message) {
        Logger::log(LogLevel::DEBUG, message);
    }

    //! Set the logging level
    static void set_level(LogLevel);
    //! Get the current logging level
    static LogLevel level() {return instance_.level_;}

    //! Make the logger output to stdout
    static void to_stdout();
    //! Make the logger output to stderr
    static void to_stderr();
    //! Silent the logger
    static void silent();
    //! Make the logger output to the file at `path`. The file will be created and
    //! overwrited if it already exists.
    static void to_file(const std::string& path);
    //! Set a callback that will be called to perform logging
    static void callback(logging_cb);
    //! Get the current logging backend
    static LogBackend backend() {return instance_.backend_;}

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;
private:
    Logger();
    void write_message(LogLevel level, const std::string& message);

    //! Singleton instance
    static Logger instance_;

    //! Current maximal logging level
    LogLevel level_;
    //! Current logging backend
    LogBackend backend_;
    //! Current log file, if `backend_ == FILE`
    std::fstream logfile_;
    //! Current callback, if `backend_ == CALLBACK`
    logging_cb callback_;
};

} // namespace chemfiles

#endif
