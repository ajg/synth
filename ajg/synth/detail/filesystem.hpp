//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_FILESYSTEM_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_FILESYSTEM_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <cmath>
#include <string>
#include <limits>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <utility>
#include <exception>
#include <stdexcept>
#include <sys/stat.h>

#if AJG_SYNTH_IS_COMPILER_MSVC
#    include <direct.h> // For MAX_PATH
#endif

#include <boost/assert.hpp>
#include <boost/cstdint.hpp>

#include <ajg/synth/exceptions.hpp>

namespace ajg {
namespace synth {
namespace detail {

//
// is_absolute:
//     Returns whether a path is considered absolute, even on Windows.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Path>
inline bool is_absolute(Path path) {
    BOOST_STATIC_CONSTANT(bool, check_drive_letter = AJG_SYNTH_IF_WINDOWS(true, false));
    return (!path.empty() && path[0] == '/') || (check_drive_letter && path.size() >= 3
        && (std::isalpha)(path[0]) && path[1] == ':' && (path[2] == '/' || path[2] == '\\'));
}

//
// stat_file
////////////////////////////////////////////////////////////////////////////////////////////////////

inline struct stat stat_file(std::string const& path) {
    struct stat stats;

    if (stat(path.c_str(), &stats) != 0) {
        AJG_SYNTH_THROW(read_error(path, std::strerror(errno)));
    }

    return stats;
}

//
// get_current_working_directory
////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::string get_current_working_directory() {
    char buffer[AJG_SYNTH_IF_MSVC(MAX_PATH, PATH_MAX)] = {};
    return AJG_SYNTH_IF_MSVC(_getcwd, getcwd)(buffer, sizeof(buffer));
}

//
// read_buffer_size
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(PIPE_BUF)
std::size_t const read_buffer_size = PIPE_BUF;
#elif defined(BUFSIZ)
std::size_t const read_buffer_size = BUFSIZ;
#else
std::size_t const read_buffer_size = 4096;
#endif

//
// read_file_to_stream:
//     Slurps a whole FILE* into a stream, using a buffer.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Stream> // TODO[c++11]: Make buffer_size a (defaulted) template parameter.
void read_file_to_stream(FILE *const file, Stream& stream) {
    typedef typename Stream::char_type char_type;
    char_type buffer[read_buffer_size];
    BOOST_ASSERT(file != 0);

    while (std::size_t const items = std::fread(buffer, sizeof(char_type), read_buffer_size, file)) {
        stream.write(buffer, items);
    }
}

//
// read_path_to_string:
//     Slurps a whole file into a string.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Char>
inline std::basic_string<Char> read_path_to_string(char const* const path) {
    FILE* const file = (std::fopen)(path, "rb");
    std::basic_ostringstream<Char> stream;
    read_file_to_stream(file, stream);
    (std::fclose)(file); // FIXME: Not exception safe, but unlikely to be a problem.
    return stream.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_UNUSED

template <class Char>
std::basic_string<Char> read_path_to_string(std::string const& path) const {
    std::basic_ifstream<Char> file;

    try {
        file.open(path.c_str(), std::ios::binary);
        return read_stream_to_string<std::basic_string<Char> >(file);
    }
    catch (std::exception const& e) {
        AJG_SYNTH_THROW(read_error(path, e.what()));
    }
}

template <class String, class Stream>
inline String read_stream_to_string
        ( Stream& stream
        , boost::optional<typename Stream::size_type> const size = boost::none
        ) {
    BOOST_STATIC_CONSTANT(typename String::size_type, buffer_size = 4096);

    if (!stream.good()) {
        AJG_SYNTH_THROW(std::runtime_error("bad stream"));
    }

    String result;
    if (size) result.reserve(*size);
    typename String::value_type buffer[buffer_size];

    while (!stream.eof()) {
        stream.read(buffer, buffer_size);
        result.append(buffer, stream.gcount());
    }

    if (stream.bad()) {
        AJG_SYNTH_THROW(std::runtime_error("bad stream"));
    }

    return result;
}

#endif // AJG_SYNTH_UNUSED

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_FILESYSTEM_HPP_INCLUDED
