/*
* Harp, an efficient IO library for chemistry file formats
* Copyright (C) 2014 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef HARP_FORMAT_FACTORY_HPP
#define HARP_FORMAT_FACTORY_HPP

#include <map>
#include <memory>

#include "Format.hpp"

namespace harp {

//! Create a format reader
template <class FormatClass>
std::unique_ptr<FormatReader> new_format_reader() {
    return std::unique_ptr<FormatReader>(new FormatClass());
}

//! Create a format writer
template <class FormatClass>
std::unique_ptr<FormatWriter> new_format_writer() {
    return std::unique_ptr<FormatWriter>(new FormatClass());
}

/*!
* @class FormatFactory FormatFactory.hpp
* @brief Factory for FormatReader and FormatWriter classes
*
* This class allow to register at compile time various FormatReader and FormatWriter
* classes, giving at runtime the good format (by the associated extension) when
* asked politely.
*/
class FormatFactory{
public:
    //! Get a format reader from an \c extention. Throw an error if the format can not be found
    static std::unique_ptr<FormatReader> get_reader(const std::string& extension){
        // TODO check if the key is present and throw error
        return get_reader_map()[extension]();
    }
    //! Get a format writer from an \c extention. Throw an error if the format can not be found
    static std::unique_ptr<FormatWriter> get_writer(const std::string& extension){
        // TODO check if the key is present and throw error
        return get_writer_map()[extension]();
    }
protected:
    //! Files extensions to format reader associations
    typedef std::map<std::string, std::unique_ptr<FormatReader>(*)()> reader_map_t;
    //! Files extensions to format writer associations
    typedef std::map<std::string, std::unique_ptr<FormatWriter>(*)()> writer_map_t;

    //! Static reader_map instance
    static reader_map_t& get_reader_map(){
        static reader_map_t reader_map = reader_map_t();
        return reader_map;
    }
    //! Static writer_map instance
    static writer_map_t& get_writer_map(){
        static writer_map_t writer_map = writer_map_t();
        return writer_map;
    }
};

template <class ReaderClass>
struct ReaderRegister : public FormatFactory {
    ReaderRegister(const std::string &name){
        // TODO: Check if the key already exist
        get_reader_map().insert(std::make_pair(name, &new_format_reader<ReaderClass>));
    }
};

template <class WriterClass>
struct WriterRegister : public FormatFactory {
    WriterRegister(const std::string &name){
        // TODO: Check if the key already exist
        get_writer_map().insert(std::make_pair(name, &new_format_writer<WriterClass>));
    }
};

//! Add a register as a static class member for FormatReader.
#define READER_REGISTER_MEMBER(type) \
    static ReaderRegister<type> __reg__;
//! Add a register as a static class member for FormatWriter.
#define WRITER_REGISTER_MEMBER(type) \
    static WriterRegister<type> __reg__;

//! Register a FormatReader by associating it to a specific extension.
#define REGISTER_READER(type, extension) \
    ReaderRegister<type> type::__reg__ = ReaderRegister<type>(extension);
//! Register a FormatWriter by associating it to a specific extension.
#define REGISTER_WRITER(type, extension) \
    WriterRegister<type> type::__reg__ = WriterRegister<type>(extension);

} // namespace harp

#endif
