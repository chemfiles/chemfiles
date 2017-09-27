// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <iostream>
#include <chemfiles.hpp>

static void os_display_notification(int, const char*) {}
constexpr int OS_WARNING = 1;
void work_with_warning(const std::string&);

// [example]
    void work_with_warning(const std::string& message) {
        // send the warning to the operating system notification
        os_display_notification(OS_WARNING, message.c_str());
    }

    int main() {
        /// Use a function as warning callback
        chemfiles::set_warning_callback(work_with_warning);

        /// Use a lambda as warning callback
        chemfiles::set_warning_callback([](const std::string& message){
            std::cout << "got a warning from chemfiles: " << message << std::endl;
        });

        return 0;
    }
// [example]
