/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include <fstream>
#include <iostream>

#include "chemfiles/Logger.hpp"
using namespace chemfiles;

// No-op buffer for no-op stream
class NullBuffer : public std::streambuf {
public:
  int overflow(int c) override { return c; }
};

static NullBuffer nullbuff;
// nullstream is a no-op ostream.
static std::ostream nullstream(&nullbuff);

// Singleton instance_
Logger Logger::instance_{};

Logger::Logger() : current_level_(WARNING), ostream_(&std::clog), is_file_(false) {}

Logger::~Logger(){
    close();
}

std::ostream& Logger::out(LogLevel level){
    return instance_.get_stream(level);
}

std::ostream& Logger::get_stream(LogLevel level){
    // Don't write anything if the out level is less important than
    // the current level, or if the current level is NONE.
    if (level > current_level_ || current_level_ == NONE)
        return nullstream;

    switch(level){
        case ERROR:
            *ostream_ << "Chemfiles error: ";
            break;
        case WARNING:
            *ostream_ << "Chemfiles warning: ";
            break;
        case INFO:
            *ostream_ << "Chemfiles info: ";
            break;
        case DEBUG:
            *ostream_ << "Chemfiles debug: ";
            break;
        case NONE:
            break;
    }
    return *ostream_;
}


void Logger::level(LogLevel level){
    instance_.current_level_ = level;
}

void Logger::log_to_stdout(){
    instance_.close();
    instance_.is_file_ = false;
    instance_.ostream_ = &std::cout;
}

void Logger::log_to_stderr(){
    instance_.close();
    instance_.is_file_ = false;
    instance_.ostream_ = &std::cerr;
}

void Logger::log_to_stdlog(){
    instance_.close();
    instance_.is_file_ = false;
    instance_.ostream_ = &std::clog;
}

void Logger::log_to_file(const std::string &filename){
    instance_.close();
    instance_.is_file_ = true;
    instance_.ostream_ = new std::ofstream(filename, std::ofstream::out);
}

void Logger::close(){
    if(is_file_){
        static_cast<std::ofstream *>(ostream_)->close();
        delete ostream_;
    }
}
