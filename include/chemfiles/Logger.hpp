/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_LOGGING_H
#define CHEMFILES_LOGGING_H

#include <fstream>
#include <sstream>
#include <functional>
#include <string>

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

//! Callback function type for logging the `message` at `level`
using logging_cb_t = std::function<void(LogLevel level, const std::string& message)>;

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

    //! Log a `message` if the `level` is lower than the maximal curent logging
    //! level. The message can be built using a variadic function call, like:
    //! ```cxx
    //! Logger::log(LogLevel::ERROR, "Here ", " and ", 4, " times ", there->call());
    //! ```
    template<typename... Args>
    static void log(LogLevel level, Args const&... args) {
        // Don't write anything if the output level is less important than
        // the current level.
        if (level > Logger::level())
            return;

        std::ostringstream stream;
        // Some black magic using the comma operator to upack the template parameter
        // pack while using it to build the stream.
        auto _ = {0, ((void)(stream << args), 0)... };
        // silent usused variable _ warning
        (void)_;

        instance_.write_message(level, stream.str());
    }
    //! Equivalent to `Logger::log(LogLevel::ERROR, args...)`
    template<typename... Args>
    static void error(Args const&... args) {
        Logger::log(LogLevel::ERROR, args...);
    }
    //! Equivalent to `Logger::log(LogLevel::ERROR, args...)`
    template<typename... Args>
    static void warn(Args const&... args) {
        Logger::log(LogLevel::WARNING, args...);
    }
    //! Equivalent to `Logger::log(LogLevel::ERROR, args...)`
    template<typename... Args>
    static void info(Args const&... args) {
        Logger::log(LogLevel::INFO, args...);
    }
    //! Equivalent to `Logger::log(LogLevel::ERROR, args...)`
    template<typename... Args>
    static void debug(Args const&... args) {
        Logger::log(LogLevel::DEBUG, args...);
    }

    //! Set the logging level
    static void set_level(LogLevel);
    //! Get the current logging level
    static LogLevel level() { return instance_.level_; }

    //! Make the logger output to stdout
    static void to_stdout();
    //! Make the logger output to stderr
    static void to_stderr();
    //! Silent the logger
    static void silent();
    //! Make the logger output to the file at `path`. The file will be created
    //! and
    //! overwrited if it already exists.
    static void to_file(const std::string& path);
    //! Set a callback that will be called to perform logging
    static void callback(logging_cb_t);
    //! Get the current logging backend
    static LogBackend backend() { return instance_.backend_; }

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

private:
    Logger();
    void write_message(LogLevel level, std::string message);

    //! Singleton instance
    static Logger instance_;

    //! Current maximal logging level
    LogLevel level_;
    //! Current logging backend
    LogBackend backend_;
    //! Current log file, if `backend_ == FILE`
    std::fstream logfile_;
    //! Current callback, if `backend_ == CALLBACK`
    logging_cb_t callback_;
};

} // namespace chemfiles

#endif
