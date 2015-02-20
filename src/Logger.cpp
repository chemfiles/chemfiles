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

// Singleton instance
Logger Logger::instance = Logger();

Logger::Logger() : current_level(WARNING), os(&std::clog), is_file(false) {}

Logger::~Logger(void){
    close();
}

std::ostream& Logger::out(LogLevel level){
    return instance.get_stream(level);
}

std::ostream& Logger::get_stream(LogLevel level){
    // Don't write anything if the out level is less important than
    // the current level
    if (level > current_level)
        os->clear(std::ios_base::badbit);
    else
        os->clear(std::ios_base::goodbit);

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
    }
    return *os;
}


void Logger::set_level(LogLevel level){
    instance.current_level = level;
}

void Logger::log_to_stdout(void){
    instance.close();
    instance.is_file = false;
    instance.os = &std::cout;
}

void Logger::log_to_stderr(void){
    instance.close();
    instance.is_file = false;
    instance.os = &std::cerr;
}

void Logger::set_log_file(const std::string &filename){
    return set_log_file(filename.c_str());
}

void Logger::set_log_file(const char* filename){
    instance.close();
    instance.is_file = true;
    instance.os = new std::ofstream(filename, std::ofstream::out);
}

void Logger::close(void){
    if(is_file){
        static_cast<std::ofstream *>(os)->close();
        delete os;
    }
}
