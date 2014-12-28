#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#include "gtest/gtest.h"

#include "Harp.hpp"

using namespace harp;

TEST(Logging, BasicLogUsage){

    std::stringstream out_buffer;

    // Save clog's buffer here
    std::streambuf *sbuf = std::clog.rdbuf();
    // Redirect clog to the new buffer
    std::clog.rdbuf(out_buffer.rdbuf());

    LOG(ERROR) << "an error" << std::endl;
    EXPECT_STREQ("Harp error: an error\n", out_buffer.str().c_str());
    out_buffer.str(std::string()); // Clean the buffer

    LOG(WARNING) << "a warning" << std::endl;
    EXPECT_STREQ("Harp warning: a warning\n", out_buffer.str().c_str());
    out_buffer.str(std::string());

    // The level should be WARNING by default
    LOG(INFO) << "an info" << std::endl;
    EXPECT_STREQ("", out_buffer.str().c_str());
    out_buffer.str(std::string());

    LOG(DEBUG) << "a debug info" << std::endl;
    EXPECT_STREQ("", out_buffer.str().c_str());
    out_buffer.str(std::string());

    // Redirect clog to its old self
    std::clog.rdbuf(sbuf);
}

TEST(Logging, SetLogStream){

    // redirect log to stdout
    Logger::log_to_stdout();
    std::stringstream out_buffer;

    std::streambuf *sbuf = std::cout.rdbuf();
    std::cout.rdbuf(out_buffer.rdbuf());

    LOG(WARNING) << "a warning" << std::endl;
    EXPECT_STREQ("Harp warning: a warning\n", out_buffer.str().c_str());
    out_buffer.str(std::string());

    std::cout.rdbuf(sbuf);

    // redirect log to stderr
    sbuf = std::cerr.rdbuf();
    std::cerr.rdbuf(out_buffer.rdbuf());

    Logger::log_to_stderr();

    LOG(WARNING) << "a warning" << std::endl;
    EXPECT_STREQ("Harp warning: a warning\n", out_buffer.str().c_str());
    out_buffer.str(std::string());

    std::cerr.rdbuf(sbuf);

    // redirect log to a file
    Logger::set_log_file("test-logging-tmp.log");
    LOG(WARNING) << "a warning" << std::endl;
    std::ifstream logfile("test-logging-tmp.log");
    std::string log_content;
    std::getline(logfile, log_content);
    logfile.close();
    EXPECT_STREQ("Harp warning: a warning", log_content.c_str());

    remove("test-logging-tmp.log");
}
