//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DEBUG_HPP_INCLUDED
#define AJG_SYNTH_DEBUG_HPP_INCLUDED

#if !AJG_SYNTH_IS_DEBUG
#    error Debugging instrumentation with debugging disabled
#endif

#define BOOST_ENABLE_ASSERT_HANDLER

#include <boost/config.hpp>
#include <boost/assert.hpp>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#if AJG_SYNTH_HAS_CXXABI_H
#include <cxxabi.h>
#endif
#if AJG_SYNTH_HAS_EXECINFO_H
#include <execinfo.h>
#endif

#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/exception/detail/attribute_noreturn.hpp>


// TODO: In all these functions, eliminate dynamic allocations & minimize potential runtime failures.

namespace ajg {
namespace synth {
namespace debug {

static std::size_t count = 0, level = 0;
inline void dummy() { (void) count; (void) level; }

///
/// AJG_SYNTH_DEBUG_RESET_COUNT
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_IS_DEBUG
#    define AJG_SYNTH_DEBUG_RESET_COUNT(n) (::ajg::synth::debug::count = n)
#else
#    define AJG_SYNTH_DEBUG_RESET_COUNT(n) ((void) 0)
#endif

///
/// AJG_SYNTH_DEBUG_LOG_LEAD, AJG_SYNTH_DEBUG_LOG_TRAIL
///     TODO: Format file/line/col the same as the compiler so that IDEs pick it up.
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_DEBUG_LOG_TRAIL (std::endl)
#define AJG_SYNTH_DEBUG_LOG_LEAD  ( \
    (ajg::synth::debug::count++ ? std::cerr : std::cerr << std::endl) << std::boolalpha \
    << "  in " << __FUNCTION__ << "():\t" \
    << std::string(ajg::synth::debug::level * 4, ' ') \
)

///
/// DUMP, PRINT
////////////////////////////////////////////////////////////////////////////////////////////////////

#define DUMP(e)  (AJG_SYNTH_DEBUG_LOG_LEAD << #e << " = `" << (e) << "`" << AJG_SYNTH_DEBUG_LOG_TRAIL)
#define PRINT(e) (AJG_SYNTH_DEBUG_LOG_LEAD << (e)                        << AJG_SYNTH_DEBUG_LOG_TRAIL)

///
/// abbreviate
////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::string abbreviate(char const* s) {
    std::string result;
    result.reserve(std::strlen(s));

    bool empty = true;
    int n = 0;
    while (char const c = *s++) {
        if ((c == '<' && n++ == 0)
         || (c == '>' && --n == 0)
         || (n == 0)) {
            result += c;
            empty = true;
        }
        else if (empty) {
            result += '_';
            empty = false;
        }
    }

    boost::algorithm::replace_all(result, "boost::",     "");
    boost::algorithm::replace_all(result, "xpressive::", "");
    boost::algorithm::replace_all(result, "detail::",    "");
    boost::algorithm::replace_all(result, "ajg::",       "");
    boost::algorithm::replace_all(result, "synth::",     "");
    boost::algorithm::replace_all(result, "adapters::",  "");
    boost::algorithm::replace_all(result, "bindings::",  "");
    boost::algorithm::replace_all(result, "engines::",   "");
    boost::algorithm::replace_all(result, "templates::", "");
    boost::algorithm::replace_all(result, "tut::",       "");
    boost::algorithm::replace_all(result, "std::",       "");
    boost::algorithm::replace_all(result, "__1::",       "");
    boost::algorithm::replace_all(result, "const&",      "");
    boost::algorithm::replace_all(result, "const ",      "");

    return result;
}

// TODO: Make this usable outside of debugging.
inline std::string unmangle(std::string const& mangled) {

#if AJG_SYNTH_HAS_CXXABI_H

    // TODO[c++11]: unique_ptr.
    int status = 0;
    boost::shared_ptr<char> unmangled(abi::__cxa_demangle(mangled.c_str(), 0, 0, &status), std::free);
    return unmangled && status == 0 ? abbreviate(unmangled.get()) : mangled;

#else

    return mangled;

#endif

}

// TODO: Reimplement using static regexes.
static boost::xpressive::sregex const signature = boost::xpressive::sregex::compile(
    "((?<=[\\s:~])(\\w+)\\s*\\(([\\w\\s,<>\\[\\].=&':/*]*?)\\)\\s*(const)?\\s*(?={))");

inline void fprint_backtrace(FILE* file, std::size_t frames_skipped = 0) {

#if AJG_SYNTH_HAS_EXECINFO_H
    std::size_t const N = AJG_SYNTH_CONFIG_MAX_FRAMES;
    void* frames[N]; // TODO: Make a "thread-local static global".
    std::size_t const n = backtrace(frames, N);

    boost::shared_ptr<char*> symbols(backtrace_symbols(frames, n), std::free);
    if (symbols) { // TODO[c++11]: auto const& symbols = make_unique_ptr().
        for (std::size_t i = frames_skipped; i < n; ++i) {
            int index = 0;
            std::string module, address, mangled;
            std::istringstream in(symbols.get()[i]);

            if (!(in >> index >> module >> address >> mangled)) {
                continue;
            }
            else if (mangled == "start") {
                break;
            }
            else {
                std::string entry = unmangle(mangled);
                boost::xpressive::smatch match;

                if (boost::xpressive::regex_match(entry, match, signature)) {
                    entry = match[2] + "(" + match[1] + ")";
                }

                fprintf(file, "%3d\t%s\t%s\n", index, module.c_str(), entry.c_str());
            }
        }
    }

#else

    fprintf(file, "Backtrace unavailable\n");

#endif

}

inline void signal_handler( int        signum
#if AJG_SYNTH_HAS_SIGACTION_H
                          , siginfo_t* info
                          , void*      context
#endif
                          ) {
    char const* name = 0;

    switch (signum) {
    case SIGABRT: name = "SIGABRT"; break;
    case SIGSEGV: name = "SIGSEGV"; break;
 // case SIGBUS:  name = "SIGBUS";  break;
    case SIGILL:  name = "SIGILL";  break;
    case SIGFPE:  name = "SIGFPE";  break;
 // case SIGPIPE: name = "SIGPIPE"; break;
    default:      name = "?";       break;
    }

    fprintf(stderr, "Caught signal %d (%s)\n", signum, name);
    fprint_backtrace(stderr);
    std::exit(signum);
}

#ifdef AJG_SYNTH_CONFIG_HANDLE_EXCEPTION
#undef AJG_SYNTH_CONFIG_HANDLE_EXCEPTION
#endif

#define AJG_SYNTH_CONFIG_HANDLE_EXCEPTION(e) (::ajg::synth::debug::throw_exception(e))

template <class Exception>
BOOST_ATTRIBUTE_NORETURN
inline void throw_exception(Exception const& e) {
    std::string const name = unmangle(typeid(Exception).name());
    fprintf(stderr, "Exception of type `%s` about to be thrown\n", name.c_str());
    fprint_backtrace(stderr, 1);
    boost::throw_exception(e);
}

inline void terminate_handler() {
    fprintf(stderr, "Terminated\n");
    fprint_backtrace(stderr, 1);
    std::exit(EXIT_FAILURE);
}

inline void unexpected_handler() {
    fprintf(stderr, "Unexpected exception\n");
    fprint_backtrace(stderr, 1);
    std::exit(EXIT_FAILURE);
}

inline void set_handlers() {
#if AJG_SYNTH_HAS_SIGACTION_H

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);

    BOOST_VERIFY(sigaction(SIGABRT, &sa, NULL) == 0);
    BOOST_VERIFY(sigaction(SIGSEGV, &sa, NULL) == 0);
    BOOST_VERIFY(sigaction(SIGBUS,  &sa, NULL) == 0);
    BOOST_VERIFY(sigaction(SIGILL,  &sa, NULL) == 0);
    BOOST_VERIFY(sigaction(SIGFPE,  &sa, NULL) == 0);
    BOOST_VERIFY(sigaction(SIGPIPE, &sa, NULL) == 0);

#else

    BOOST_VERIFY(signal(SIGABRT, signal_handler) != SIG_ERR);
    BOOST_VERIFY(signal(SIGSEGV, signal_handler) != SIG_ERR);
 // BOOST_VERIFY(signal(SIGBUS,  signal_handler) != SIG_ERR);
    BOOST_VERIFY(signal(SIGILL,  signal_handler) != SIG_ERR);
    BOOST_VERIFY(signal(SIGFPE,  signal_handler) != SIG_ERR);
 // BOOST_VERIFY(signal(SIGPIPE, signal_handler) != SIG_ERR);

#endif

    std::set_terminate(terminate_handler);
    std::set_unexpected(unexpected_handler);
}

struct initializer {
  // private:
    inline initializer() {
        static bool initialized = false;
        if (!initialized) {
            initialized = true;
            set_handlers();
        }
    }
} const initializer;

}}} // namespace ajg::synth::debug

namespace boost {

inline void assertion_failed( char const* const expression
                            , char const* const function
                            , char const* const file
                            , long        const line
                            ) {
    std::string const f = ::ajg::synth::debug::abbreviate(function);
    fprintf(stderr, "%s in %s() at %s:%ld\n", expression, f.c_str(), file, line);
    ::ajg::synth::debug::fprint_backtrace(stderr, 1);
    (::std::exit)(EXIT_FAILURE);
}

inline void assertion_failed_msg( char const* const expression
                                , char const* const message
                                , char const* const function
                                , char const* const file
                                , long        const line
                                ) {
    std::string const f = ::ajg::synth::debug::abbreviate(function);
    fprintf(stderr, "%s [%s] in %s() at %s:%ld\n", expression, message, f.c_str(), file, line);
    ::ajg::synth::debug::fprint_backtrace(stderr, 1);
    (::std::exit)(EXIT_FAILURE);
}

} // namespace boost

#endif // AJG_SYNTH_DEBUG_HPP_INCLUDED
