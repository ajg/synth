//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_HPP_INCLUDED

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
#include <typeinfo>
#include <exception>
#include <stdexcept>
#include <sys/stat.h>

#if AJG_SYNTH_IS_COMPILER_MSVC
#    include <direct.h> // For MAX_PATH
#endif

#include <boost/assert.hpp>
#include <boost/cstdint.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/throw_exception.hpp>
#include <boost/utility/enable_if.hpp>

#include <ajg/synth/exceptions.hpp>

namespace ajg {
namespace synth {
namespace detail {

using boost::throw_exception;

//
// nonconstructible:
//     Utility class to prevent instantiations of a class meant to be 'static.'
////////////////////////////////////////////////////////////////////////////////////////////////////

struct nonconstructible {
  private:
    nonconstructible();
};

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
        throw_exception(read_error(path, std::strerror(errno)));
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
        throw_exception(read_error(path, e.what()));
    }
}

template <class String, class Stream>
inline String read_stream_to_string
        ( Stream& stream
        , optional<typename Stream::size_type> const size = boost::none
        ) {
    BOOST_STATIC_CONSTANT(typename String::size_type, buffer_size = 4096);

    if (!stream.good()) {
        throw_exception(std::runtime_error("bad stream"));
    }

    String result;
    if (size) result.reserve(*size);
    typename String::value_type buffer[buffer_size];

    while (!stream.eof()) {
        stream.read(buffer, buffer_size);
        result.append(buffer, stream.gcount());
    }

    if (stream.bad()) {
        throw_exception(std::runtime_error("bad stream"));
    }

    return result;
}

#endif // AJG_SYNTH_UNUSED

//
// pipe:
//     Nicer, safer interface to popen/pclose.
////////////////////////////////////////////////////////////////////////////////////////////////////

struct pipe : boost::noncopyable {
  public:

    explicit pipe(std::string const& command, bool const reading = true) {
        if ((file_ = AJG_SYNTH_IF_MSVC(_popen, popen)(command.c_str(), reading ? "r" : "w")) == 0) {
            throw_exception(error("open"));
        }
    }

    ~pipe() {
        if (AJG_SYNTH_IF_MSVC(_pclose, pclose)(file_) == -1) {
            throw_exception(error("close"));
        }
    }

  public:

    struct error : public std::runtime_error {
        error(std::string const& action)
            : std::runtime_error("could not " + action + " pipe (" + std::strerror(errno) + ")") {}
    };

  public:

    template <class Stream>
    void read_into(Stream& stream) {
        read_file_to_stream(file_, stream);
    }

  private:

    FILE* file_;
};

typedef boost::intmax_t integer_type;
// typedef boost::uintmax_t natural_type;

//
// slice:
//     Accepts the indices for a half-open range [lower, upper) and returns said range as a pair of
//     iterators; imitates Python's sequence slicing including negative indices, which are "rotated"
//     into their positive counterparts. The indices are bounds-checked regardless of their sign.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Container>
inline std::pair< typename Container::const_iterator
                , typename Container::const_iterator
                > slice( Container                     const& container
                       , boost::optional<integer_type> const  lower     = boost::none
                       , boost::optional<integer_type> const  upper     = boost::none
                       ) {
    typedef typename Container::size_type size_type;

    size_type const size = container.size();
    integer_type lower_ = lower.get_value_or(0);
    integer_type upper_ = upper.get_value_or(size);

    // Adjust negative indices to the right position.
    if (lower_ < 0) lower_ = static_cast<integer_type>(size) + lower_;
    if (upper_ < 0) upper_ = static_cast<integer_type>(size) + upper_;

    // Check for indices that are out of range.
    if (lower_ < 0 || static_cast<size_type>(lower_) > size) throw_exception(std::out_of_range("lower index"));
    if (upper_ < 0 || static_cast<size_type>(upper_) > size) throw_exception(std::out_of_range("upper index"));
    if (lower_ > upper_)                                     throw_exception(std::logic_error("reversed indices"));

    // Move to the right places.
    typename Container::const_iterator first  = container.begin();
    typename Container::const_iterator second = first;
    std::advance(first, lower_);
    std::advance(second, upper_);
    return std::make_pair(first, second);
}

//
// at:
//     Accepts an index which can be negative, which is "rotated" into its positive counterpart.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Container>
inline typename Container::const_iterator at(Container const& container, integer_type const index) {
    typedef typename Container::size_type size_type;

    size_type    const size   = container.size();
    integer_type const index_ = index < 0 ? static_cast<integer_type>(size) + index : index;

    if (index_ < 0 || static_cast<size_type>(index_) > size) throw_exception(std::out_of_range("index"));

    // TODO: Once we have value_iterator::advance_to consider using return begin() + index,
    //       to be O(1). For now, we must use this O(n) method:
    typename Container::const_iterator       it  = container.begin();
    typename Container::const_iterator const end = container.end();

    for (size_type i = 0; it != end; ++it, ++i) {
        if (i == static_cast<size_type>(index_)) {
            return it;
        }
    }

    AJG_SYNTH_THROW(std::invalid_argument("index"));
}

//
// advance_to:
//     Simulates operator + for iterators which lack it.
//     NOTE: Doesn't do any bounds checking; ensure distance is valid.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Iterator, class Distance>
inline Iterator advance_to(Iterator iterator, Distance const distance) {
    std::advance(iterator, distance);
    return iterator;
}

//
// drop
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Container, class Number>
inline std::pair<typename Container::const_iterator, typename Container::const_iterator>
        drop(Container const& container, Number const number) {
    return std::make_pair(advance_to(container.begin(), number), container.end());
}

//
// get_type_name
////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::string get_type_name(std::type_info const& info) {
    return info.name(); // TODO: Unmangle where needed.
}

//
// is_integer:
//     Determines whether a floating-point number is an integer.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class FloatingPoint>
inline bool is_integer(FloatingPoint const& fp) {
    FloatingPoint integer_part;
    return (std::modf)(fp, &integer_part) == FloatingPoint(0.0);
}

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_HPP_INCLUDED
