#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <catch.hpp>

#include "chemfiles/Logger.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Basic logging usage", "[logging]"){
    std::stringstream out_buffer;

    // Save clog's buffer here
    std::streambuf *sbuf = std::clog.rdbuf();
    // Redirect clog to the new buffer
    std::clog.rdbuf(out_buffer.rdbuf());

    LOG(ERROR) << "an error" << std::endl;
    CHECK("Chemfiles error: an error\n" == out_buffer.str());
    out_buffer.str(std::string()); // Clean the buffer

    LOG(WARNING) << "a warning" << std::endl;
    CHECK("Chemfiles warning: a warning\n" == out_buffer.str());
    out_buffer.str(std::string());

    // The level should be WARNING by default
    LOG(INFO) << "an info" << std::endl;
    CHECK("" == out_buffer.str());
    out_buffer.str(std::string());

    LOG(DEBUG) << "a debug info" << std::endl;
    CHECK("" == out_buffer.str());
    out_buffer.str(std::string());

    // Redirect clog to its old self
    std::clog.rdbuf(sbuf);
}

TEST_CASE("Set the log stream", "[logging]"){
    std::stringstream out_buffer;
    std::streambuf *sbuf;

    SECTION("Redirect log to stdout") {
        Logger::log_to_stdout();
        sbuf = std::cout.rdbuf();
        std::cout.rdbuf(out_buffer.rdbuf());

        LOG(WARNING) << "a warning" << std::endl;
        CHECK("Chemfiles warning: a warning\n" == out_buffer.str());

        std::cout.rdbuf(sbuf);
    }

    SECTION("Redirect log to stderr") {
        Logger::log_to_stderr();
        sbuf = std::cerr.rdbuf();
        std::cerr.rdbuf(out_buffer.rdbuf());

        LOG(WARNING) << "a warning" << std::endl;
        CHECK("Chemfiles warning: a warning\n" == out_buffer.str());

        std::cerr.rdbuf(sbuf);
    }

    SECTION("Redirect log to a file") {
        Logger::log_to_file("test-logging-tmp.log");

        LOG(WARNING) << "a warning" << std::endl;

        std::ifstream logfile("test-logging-tmp.log");
        std::string log_content;
        std::getline(logfile, log_content);
        logfile.close();

        CHECK("Chemfiles warning: a warning" == log_content);

        remove("test-logging-tmp.log");
    }
    Logger::log_to_stdlog();
}

TEST_CASE("Set the log level", "[logging]"){
    std::stringstream out_buffer;
    std::streambuf *sbuf = std::clog.rdbuf();
    std::clog.rdbuf(out_buffer.rdbuf());

    SECTION("NONE level") {
        Logger::level(Logger::NONE);

        LOG(ERROR) << "an error" << std::endl;
        CHECK("" == out_buffer.str());
        out_buffer.str(std::string());

        LOG(INFO) << "an info" << std::endl;
        CHECK("" == out_buffer.str());
        out_buffer.str(std::string());
    }

    SECTION("INFO level") {
        Logger::level(Logger::INFO);

        LOG(ERROR) << "an error" << std::endl;
        CHECK("Chemfiles error: an error\n" == out_buffer.str());
        out_buffer.str(std::string());

        LOG(INFO) << "an info" << std::endl;
        CHECK("Chemfiles info: an info\n" == out_buffer.str());
        out_buffer.str(std::string());
    }
    std::clog.rdbuf(sbuf);
}
