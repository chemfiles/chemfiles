#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Harp.hpp"

TEST_CASE("Basic logging usage", "[logging]"){
    std::stringstream out_buffer;

    // Save clog's buffer here
    std::streambuf *sbuf = std::clog.rdbuf();
    // Redirect clog to the new buffer
    std::clog.rdbuf(out_buffer.rdbuf());

    LOG(ERROR) << "an error" << std::endl;
    REQUIRE("Harp error: an error\n" == out_buffer.str());
    out_buffer.str(std::string()); // Clean the buffer

    LOG(WARNING) << "a warning" << std::endl;
    REQUIRE("Harp warning: a warning\n" == out_buffer.str());
    out_buffer.str(std::string());

    // The level should be WARNING by default
    LOG(INFO) << "an info" << std::endl;
    REQUIRE("" == out_buffer.str());
    out_buffer.str(std::string());

    LOG(DEBUG) << "a debug info" << std::endl;
    REQUIRE("" == out_buffer.str());
    out_buffer.str(std::string());

    // Redirect clog to its old self
    std::clog.rdbuf(sbuf);
}

TEST_CASE("Set the log stream", "[logging]"){
    std::stringstream out_buffer;
    std::streambuf *sbuf;

    SECTION("Redirect log to stdout") {
        harp::Logger::log_to_stdout();
        sbuf = std::cout.rdbuf();
        std::cout.rdbuf(out_buffer.rdbuf());

        LOG(WARNING) << "a warning" << std::endl;
        REQUIRE("Harp warning: a warning\n" == out_buffer.str());

        std::cout.rdbuf(sbuf);
    }

    SECTION("Redirect log to stderr") {
        harp::Logger::log_to_stderr();
        sbuf = std::cerr.rdbuf();
        std::cerr.rdbuf(out_buffer.rdbuf());

        LOG(WARNING) << "a warning" << std::endl;
        REQUIRE("Harp warning: a warning\n" == out_buffer.str());

        std::cerr.rdbuf(sbuf);
    }

    SECTION("Redirect log to a file") {
        harp::Logger::set_log_file("test-logging-tmp.log");

        LOG(WARNING) << "a warning" << std::endl;

        std::ifstream logfile("test-logging-tmp.log");
        std::string log_content;
        std::getline(logfile, log_content);
        logfile.close();

        REQUIRE("Harp warning: a warning" == log_content);

        remove("test-logging-tmp.log");
    }
}
