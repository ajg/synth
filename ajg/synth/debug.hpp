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
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include <cxxabi.h>
#include <execinfo.h>

#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace ajg {
namespace synth {
namespace debug {

//
// Debugging macros
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_DUMP(e) ((std::cerr << std::boolalpha) << "  " << #e << " = `" << (e) << "`" << std::endl)
#define AJG_PRINT(e)  ((std::cerr << std::boolalpha) << "  " << (e) << std::endl)

////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::string abbreviate(char const* s) {
    std::string result;
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
            result += "...";
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

// TODO: Eliminate dynamic allocations and minimize potential runtime failures.
inline void fprint_backtrace(FILE* file, std::size_t frames_skipped = 0) {
    void* frames[AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT]; // TODO: Make a "thread-local static global".
    std::size_t const n = backtrace(frames, AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT);

    boost::shared_ptr<char*> symbols(backtrace_symbols(frames, n), std::free);
    if (symbols) {
        for (std::size_t i = frames_skipped; i < n; ++i) {
            int index = 0, status = 0;
            std::string module, address, mangled;
            std::istringstream in(symbols.get()[i]);

            in >> index >> module >> address >> mangled;
            if (mangled == "start") break;

            std::string function = mangled;
            boost::shared_ptr<char> demangled(abi::__cxa_demangle(mangled.c_str(), 0, 0, &status), std::free);
            if (demangled && status == 0) {
                function = abbreviate(demangled.get());
            }
            std::fprintf(file, "%d\t%s\t%s\n", index, module.c_str(), function.c_str());
        }
    }
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
