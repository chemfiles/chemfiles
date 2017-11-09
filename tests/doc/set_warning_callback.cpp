// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <iostream>
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

static void os_display_notification(int, const char*) {}
constexpr int OS_WARNING = 1;
void work_with_warning(const std::string&);

void work_with_warning(const std::string& message) {
    // send the warning to the operating system notification
    os_display_notification(OS_WARNING, message.c_str());
}

TEST_CASE() {
    // [example]
    // a function to do some work with the warnings
    void work_with_warning(const std::string&);

    /// Use a function as warning callback
    chemfiles::set_warning_callback(work_with_warning);

    /// Use a lambda as warning callback
    chemfiles::set_warning_callback([](const std::string& message){
        std::cout << "got a warning from chemfiles: " << message << std::endl;
    });
    // [example]
}
