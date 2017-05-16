// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;
#include <boost/asio.hpp>
namespace ip = boost::asio::ip;

#include "chemfiles/utils.hpp"

TEST_CASE("hostname") {
    auto hostname = ip::host_name();
    CHECK(chemfiles::hostname() == hostname);
}

TEST_CASE("Current directory") {
    auto cwd = fs::current_path();
    CHECK(chemfiles::current_directory() == cwd.string());
}
