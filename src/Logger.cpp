/*
* Chemharp, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include <fstream>
#include <iostream>

#include "Logger.hpp"
using namespace harp;

// No-op buffer for no-op stream
class NullBuffer : public std::streambuf {
public:
  int overflow(int c) { return c; }
};

static NullBuffer nullbuff;
// nullstream is a no-op ostream.
static std::ostream nullstream(&nullbuff);

// Singleton instance
Logger Logger::instance{};

Logger::Logger() : current_level(WARNING), os(&std::clog), is_file(false) {}

Logger::~Logger(){
    close();
}

std::ostream& Logger::out(LogLevel level){
    return instance.get_stream(level);
}

std::ostream& Logger::get_stream(LogLevel level){
    // Don't write anything if the out level is less important than
    // the current level, or if the current level is NONE.
    if (level > current_level || current_level == NONE)
        return nullstream;

    switch(level){
        case ERROR:
            *os << "Chemharp error: ";
            break;
        case WARNING:
            *os << "Chemharp warning: ";
            break;
        case INFO:
            *os << "Chemharp info: ";
            break;
        case DEBUG:
            *os << "Chemharp debug: ";
            break;
        case NONE:
            break;
    }
    return *os;
}


void Logger::level(LogLevel level){
    instance.current_level = level;
}

void Logger::log_to_stdout(){
    instance.close();
    instance.is_file = false;
    instance.os = &std::cout;
}

void Logger::log_to_stderr(){
    instance.close();
    instance.is_file = false;
    instance.os = &std::cerr;
}

void Logger::log_to_stdlog(){
    instance.close();
    instance.is_file = false;
    instance.os = &std::clog;
}

void Logger::log_to_file(const std::string &filename){
    instance.close();
    instance.is_file = true;
    instance.os = new std::ofstream(filename, std::ofstream::out);
}

void Logger::close(){
    if(is_file){
        static_cast<std::ofstream *>(os)->close();
        delete os;
    }
}
