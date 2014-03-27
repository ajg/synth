//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DEBUG_HPP_INCLUDED
#define AJG_SYNTH_DEBUG_HPP_INCLUDED

#ifndef AJG_SYNTH_DEBUG_NO_HANDLERS
#define BOOST_ENABLE_ASSERT_HANDLER
#endif

#include <boost/config.hpp>
#include <boost/assert.hpp>

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#if HAS_CXXABI_H
#include <cxxabi.h>
#endif
#if HAS_EXECINFO_H
#include <execinfo.h>
#endif

#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string/replace.hpp>

// TODO: In all these functions, eliminate dynamic allocations & minimize potential runtime failures.

namespace ajg {
namespace synth {
namespace debug {

//
// Debugging macros
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_CERR_LEAD  (std::cerr << std::boolalpha << __FUNCTION__ << "| ")
#define AJG_CERR_TRAIL (std::endl)

#define AJG_DUMP(e)  (AJG_CERR_LEAD << #e << " = `" << (e) << "`" << AJG_CERR_TRAIL)
#define AJG_PRINT(e) (AJG_CERR_LEAD << (e)                        << AJG_CERR_TRAIL)

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

    boost::algorithm::replace_all(result, "boost::",     "b::");
    boost::algorithm::replace_all(result, "fusion::",    "f::");
    boost::algorithm::replace_all(result, "xpressive::", "x::");
    boost::algorithm::replace_all(result, "detail::",    "d::");
    boost::algorithm::replace_all(result, "ajg::",       "a::");
    boost::algorithm::replace_all(result, "synth::",     "s::");
    boost::algorithm::replace_all(result, "std::",       "");
    boost::algorithm::replace_all(result, "__1::",       "");

    return result;
}

inline std::string unmangle(std::string const& mangled) {

#if HAS_CXXABI_H

    // TODO[c++11]: unique_ptr.
    int status = 0;
    boost::shared_ptr<char> unmangled(abi::__cxa_demangle(mangled.c_str(), 0, 0, &status), std::free);
    return unmangled && status == 0 ? abbreviate(unmangled.get()) : mangled;

#else

    return mangled;

#endif // HAS_CXXABI_H

}


inline void fprint_backtrace(FILE* file, std::size_t frames_skipped = 0) {

#if HAS_EXECINFO_H

    void* frames[AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT]; // TODO: Make a "thread-local static global".
    std::size_t const n = backtrace(frames, AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT);

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
                std::string const signature = unmangle(mangled);
                std::fprintf(file, "%d\t%s\t%s\n", index, module.c_str(), signature.c_str());
            }
        }
    }

#else

    std::fprintf(file, "Backtrace unavailable\n");

#endif // HAS_EXECINFO_H

}

inline void signal_handler(int signum, siginfo_t* info, void* context) {
    char const* name = 0;

    switch (signum) {
    case SIGABRT: name = "SIGABRT"; break;
    case SIGSEGV: name = "SIGSEGV"; break;
    case SIGBUS:  name = "SIGBUS";  break;
    case SIGILL:  name = "SIGILL";  break;
    case SIGFPE:  name = "SIGFPE";  break;
    default:      name = "?";       break;
    }

    std::fprintf(stderr, "Caught signal %d (%s)\n", signum, name);
    fprint_backtrace(stderr);
    std::exit(signum);
}

inline void terminate_handler() {
    std::fprintf(stderr, "Terminated\n");
    fprint_backtrace(stderr, 1);
    std::exit(EXIT_FAILURE);
}

inline void unexpected_handler() {
    std::fprintf(stderr, "Unexpected Exception\n");
    fprint_backtrace(stderr, 1);
    std::exit(EXIT_FAILURE);
}

inline void set_handlers() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    sigemptyset( &sa.sa_mask );

    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS,  &sa, NULL);
    sigaction(SIGILL,  &sa, NULL);
    sigaction(SIGFPE,  &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL);

    std::set_terminate(terminate_handler);
    std::set_unexpected(unexpected_handler);
}

#ifndef AJG_SYNTH_DEBUG_NO_HANDLERS

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

#endif // AJG_SYNTH_DEBUG_NO_HANDLERS

}}} // namespace ajg::synth::debug

#ifndef AJG_SYNTH_DEBUG_NO_HANDLERS

namespace boost {

inline void assertion_failed( char const* const expression
                            , char const* const function
                            , char const* const file
                            , long        const line
                            ) {
    std::fprintf(stderr, "%s in %s() at %s:%ld\n", expression,
        ajg::synth::debug::abbreviate(function).c_str(), file, line);
    ajg::synth::debug::fprint_backtrace(stderr, 1);
    std::exit(EXIT_FAILURE);
}

inline void assertion_failed_msg( char const* const expression
                                , char const* const message
                                , char const* const function
                                , char const* const file
                                , long        const line
                                ) {
    std::fprintf(stderr, "%s [%s] in %s() at %s:%ld\n", expression, message,
        ajg::synth::debug::abbreviate(function).c_str(), file, line);
    ajg::synth::debug::fprint_backtrace(stderr, 1);
    std::exit(EXIT_FAILURE);
}

} // namespace boost

#endif // AJG_SYNTH_DEBUG_NO_HANDLERS

#endif // AJG_SYNTH_DEBUG_HPP_INCLUDED
