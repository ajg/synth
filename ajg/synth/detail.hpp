//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_HPP_INCLUDED

#include <ajg/synth/config.hpp>

#include <string>
#include <limits>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <typeinfo>
#include <exception>
#include <stdexcept>
#include <sys/stat.h>

#ifndef _WIN32
  #include <unistd.h>
  extern char **environ;
#endif

#include <boost/assert.hpp>
#include <boost/cstdint.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/throw_exception.hpp>
#include <boost/program_options/environment_iterator.hpp>

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
// AJG_SYNTH_UNREACHABLE:
//     Wrapper around BOOST_ASSERT that also invokes __assume on MSVC,
//     which (a) prevents warning C4715 and (b) eliminates wasteful code.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#    define AJG_SYNTH_UNREACHABLE (BOOST_ASSERT(0), (__assume(0)))
#elif defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ >= 405) // GCC 4.5+
#    define AJG_SYNTH_UNREACHABLE (BOOST_ASSERT(0), (__builtin_unreachable()))
#else
#    define AJG_SYNTH_UNREACHABLE (BOOST_ASSERT(0), (std::terminate()))
#endif

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
//     __declspec(noreturn) which in turns triggers warning C4715 or error C4716.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_THROW_EXCEPTION
#define AJG_SYNTH_THROW_EXCEPTION boost::throw_exception
#endif

#define AJG_SYNTH_THROW(e) \
    AJG_SYNTH_IF_MSVC((AJG_SYNTH_THROW_EXCEPTION(e), AJG_SYNTH_UNREACHABLE), AJG_SYNTH_THROW_EXCEPTION(e))

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

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_HPP_INCLUDED
