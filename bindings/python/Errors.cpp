/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp-python.hpp"

void translate_Error(Error const& e) {
    auto err = string("Chemharp error: ") + e.what();
    PyErr_SetString(PyExc_UserWarning, err.c_str());
}

void translate_FileError(FileError const& e) {
    auto err = string("Chemharp file error: ") + e.what();
    PyErr_SetString(PyExc_UserWarning, err.c_str());
}

void translate_MemoryError(MemoryError const& e) {
    auto err = string("Chemharp memory error: ") + e.what();
    PyErr_SetString(PyExc_UserWarning, err.c_str());
}

void translate_FormatError(FormatError const& e) {
    auto err = string("Chemharp format error: ") + e.what();
    PyErr_SetString(PyExc_UserWarning, err.c_str());
}

void register_errors(){
    /* Exception management ***************************************************/
    py::register_exception_translator<Error>(&translate_Error);
    py::register_exception_translator<FileError>(&translate_FileError);
    py::register_exception_translator<MemoryError>(&translate_MemoryError);
    py::register_exception_translator<FormatError>(&translate_FormatError);

    /* LogLevel enum **********************************************************/
    py::enum_<Logger::LogLevel>("LogLevel")
        .value("NONE", Logger::NONE)
        .value("ERROR", Logger::ERROR)
        .value("WARNING", Logger::WARNING)
        .value("INFO", Logger::INFO)
        .value("DEBUG", Logger::DEBUG)
    ;

    /* Logger class ***********************************************************/
    py::class_<Logger, boost::noncopyable>("Logger", py::no_init)
        .add_property("level",
            static_cast<Logger::LogLevel (*)(void)>(&Logger::level),
            static_cast<void (*)(Logger::LogLevel)>(&Logger::level))
        .def("log_to_file", &Logger::log_to_file)
            .staticmethod("log_to_file")
        .def("log_to_stdout", &Logger::log_to_stdout)
            .staticmethod("log_to_stdout")
    ;
}
