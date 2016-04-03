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

    // Save cerr's buffer here
    std::streambuf *sbuf = std::cerr.rdbuf();
    // Redirect cerr to the new buffer
    std::cerr.rdbuf(out_buffer.rdbuf());

    Logger::error("an error");
    CHECK("Chemfiles error: an error\n" == out_buffer.str());
    out_buffer.str(std::string()); // Clean the buffer

    Logger::warn("a warning");
    CHECK("Chemfiles warning: a warning\n" == out_buffer.str());
    out_buffer.str(std::string());

    // The level should be WARNING by default
    Logger::info("an info");
    CHECK("" == out_buffer.str());
    out_buffer.str(std::string());

    Logger::debug("a debug info");
    CHECK("" == out_buffer.str());
    out_buffer.str(std::string());

    // Redirect cerr to its old self
    std::cerr.rdbuf(sbuf);
}

TEST_CASE("Set the log stream", "[logging]"){
    std::stringstream out_buffer;
    std::streambuf *sbuf;

    SECTION("Redirect log to stdout") {
        Logger::to_stdout();
        sbuf = std::cout.rdbuf();
        std::cout.rdbuf(out_buffer.rdbuf());

        Logger::warn("a warning");
        CHECK("Chemfiles warning: a warning\n" == out_buffer.str());

        std::cout.rdbuf(sbuf);
    }

    SECTION("Redirect log to a file") {
        Logger::to_file("test-logging-tmp.log");

        Logger::warn("a warning");

        std::ifstream logfile("test-logging-tmp.log");
        std::string log_content;
        std::getline(logfile, log_content);
        logfile.close();

        CHECK("Chemfiles warning: a warning" == log_content);
        remove("test-logging-tmp.log");
    }

    SECTION("Silent logs") {
        sbuf = std::cerr.rdbuf();
        std::cerr.rdbuf(out_buffer.rdbuf());
        Logger::silent();

        Logger::error("an error");
        CHECK("" == out_buffer.str());
        out_buffer.str(std::string());

        std::cerr.rdbuf(sbuf);
    }

    SECTION("Use a callback for logs") {
        std::string buffer;
        LogLevel last_level;

        auto callback = [&buffer, &last_level](LogLevel level, const std::string& message) {
            buffer += message;
            last_level = level;
        };

        Logger::callback(callback);

        Logger::error("an error");
        CHECK("an error" == buffer);
        CHECK(last_level == LogLevel::ERROR);
    }

    Logger::to_stderr();
}

TEST_CASE("Set the log level", "[logging]"){
    std::stringstream out_buffer;
    std::streambuf *sbuf = std::cerr.rdbuf();
    std::cerr.rdbuf(out_buffer.rdbuf());

    Logger::set_level(LogLevel::INFO);

    Logger::error("an error");
    CHECK("Chemfiles error: an error\n" == out_buffer.str());
    out_buffer.str(std::string());

    Logger::info("an info");
    CHECK("Chemfiles info: an info\n" == out_buffer.str());
    out_buffer.str(std::string());

    Logger::debug("a debug info");
    CHECK("" == out_buffer.str());
    out_buffer.str(std::string());

    std::cerr.rdbuf(sbuf);
}
