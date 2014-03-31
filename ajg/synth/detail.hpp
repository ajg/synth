//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_HPP_INCLUDED

#include <ajg/synth/config.hpp>

#include <limits>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <sys/stat.h>

#ifndef _WIN32
  #include <unistd.h>
  extern char **environ;
#endif

#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/throw_exception.hpp>
#include <boost/program_options/environment_iterator.hpp>

// TODO: Split this out to ./exceptions.hpp.
namespace ajg {
namespace synth {

//
// not_implemented exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct not_implemented : public std::runtime_error {
    not_implemented(std::string const& feature) : std::runtime_error("not implemented: " + feature) {}
    ~not_implemented() throw () {}
};

//
// file_error exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct file_error : public std::runtime_error {
    std::string const filepath, action, reason;

    file_error( std::string const& filepath
              , std::string const& action
              , std::string const& reason
              )
        : std::runtime_error("could not " + action + " file `" + filepath + "' (" + reason + ")")
        , filepath(filepath), action(action), reason(reason) {}

    ~file_error() throw () {}
};

}} // namespace ajg::synth


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
// standard_environment:
//     Safer and iterable interface to the program's environment.
////////////////////////////////////////////////////////////////////////////////////////////////////

struct standard_environment {
  public:

    typedef boost::environment_iterator         iterator;
    typedef boost::environment_iterator         const_iterator;
    typedef iterator::value_type                value_type;
    typedef value_type::first_type              key_type;
    typedef value_type::second_type             mapped_type;

  public:

    const_iterator begin() const {
        return const_iterator(environ);
    }

    const_iterator end() const {
        return const_iterator();
    }

    const_iterator find(key_type const& name) const {
        const_iterator const end = this->end();

        for (const_iterator it = begin(); it != end; ++it) {
            if (it->first == name) {
                return it;
            }
        }

        return end;
    }
};

//
// AJG_UNREACHABLE:
//     Wrapper around BOOST_ASSERT that also invokes __assume on MSVC,
//     which (a) prevents warning C4715 and (b) eliminates wasteful code.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #define AJG_UNREACHABLE (BOOST_ASSERT(0), (__assume(0)))
#elif defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ >= 405) // GCC 4.5+
  #define AJG_UNREACHABLE (BOOST_ASSERT(0), (__builtin_unreachable()))
#else
  #define AJG_UNREACHABLE (BOOST_ASSERT(0), (std::terminate()))
#endif

/*
//
// unreachable:
//     A function to give the last, unreachable, operand to a ternary
//     operator expression the right type, instead of void.
////////////////////////////////////////////////////////////////////////////////////////////////////

inline T unreachable(T const&) {
    AJG_UNREACHABLE;
}
*/

struct unreachable {
    unreachable() {}

    template <class T>
    inline operator T() const { AJG_UNREACHABLE; }
};

//
// AJG_SYNTH_IF_WINDOWS:
//     Picks the first version for Windows environments, otherwise the second.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32) || defined(_WIN64)
#    define AJG_SYNTH_IF_WINDOWS(a, b) a
#else
#    define AJG_SYNTH_IF_WINDOWS(a, b) b
#endif

//
// AJG_SYNTH_IF_MSVC:
//     Picks the first version for Microsoft compilers, otherwise the second.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#    define AJG_SYNTH_IF_MSVC(a, b) a
#else
#    define AJG_SYNTH_IF_MSVC(a, b) b
#endif

//
// AJG_SYNTH_THROW:
//     Indirection layer needed because in some cases (e.g. virtual methods with non-void return
//     types) MSVC won't get it through its head that throw_exception doesn't return, even with
//     __declspec(noreturn) which triggers warning C4715 or error C4716.
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_THROW(e) AJG_SYNTH_IF_MSVC( \
    (::boost::throw_exception(e), AJG_UNREACHABLE), \
    ::boost::throw_exception(e))

//
// stat_file
////////////////////////////////////////////////////////////////////////////////////////////////////

inline struct stat stat_file(std::string const& filepath) {
    struct stat file;

    if (stat(filepath.c_str(), &file) != 0) {
        throw_exception(file_error(filepath, "read", std::strerror(errno)));
    }

    return file;
}

//
// read_file:
//     Slurps a whole file directly into a stream, using a buffer.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PIPE_BUF
  #define PIPE_BUF 4096
#endif

template <class Stream>
void read_file(FILE *const file, Stream& stream) {
    typedef typename Stream::char_type char_type;
    BOOST_STATIC_CONSTANT(std::size_t, buffer_size = PIPE_BUF /*BUFSIZ*/ / sizeof(char_type));
    char_type buffer[buffer_size];
    BOOST_ASSERT(file != 0);

    while (std::size_t const items = std::fread(buffer, sizeof(char_type), buffer_size, file)) {
        stream.write(buffer, items);
    }
}

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
        read_file(file_, stream);
    }

  private:

    FILE* file_;
};

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_HPP_INCLUDED
