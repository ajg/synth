//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DEBUG_HPP_INCLUDED
#define AJG_SYNTH_DEBUG_HPP_INCLUDED

#ifdef NDEBUG
#    error Debugging instrumentation with NDEBUG
#endif

#ifndef AJG_SYNTH_DEBUG_NO_HANDLERS
#define BOOST_ENABLE_ASSERT_HANDLER
#endif

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

#if HAS_CXXABI_H
#include <cxxabi.h>
#endif
#if HAS_EXECINFO_H
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

//
// Debugging macros
////////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME: For some god-forsaken reason the compilers used by Travis CI can't find fprintf in
//        namespace `std`. So this macro is a workaround until we find a solution.

#define AJG_FPRINTF /*std::*/fprintf

#define AJG_CERR_LEAD  (std::cerr << std::boolalpha << "    " << __FUNCTION__ << "() - \t")
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
    boost::algorithm::replace_all(result, "xpressive::", "x::");
    boost::algorithm::replace_all(result, "detail::",    "d::");
    boost::algorithm::replace_all(result, "ajg::",       "a::");
    boost::algorithm::replace_all(result, "synth::",     "s::");
    boost::algorithm::replace_all(result, "std::",       "");
    boost::algorithm::replace_all(result, "__1::",       "");

    return result;
}

// TODO: Make this usable outside of debugging.
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

// TODO: Reimplement using static regexes.
static boost::xpressive::sregex const signature = boost::xpressive::sregex::compile(
    "((?<=[\\s:~])(\\w+)\\s*\\(([\\w\\s,<>\\[\\].=&':/*]*?)\\)\\s*(const)?\\s*(?={))");

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
                std::string entry = unmangle(mangled);
                boost::xpressive::smatch match;

                if (boost::xpressive::regex_match(entry, match, signature)) {
                    entry = match[2] + "(" + match[1] + ")";
                }

                AJG_FPRINTF(file, "%3d\t%s\t%s\n", index, module.c_str(), entry.c_str());
            }
        }
    }

#else

    AJG_FPRINTF(file, "Backtrace unavailable\n");

#endif // HAS_EXECINFO_H

}

inline void signal_handler( int        signum
#if HAS_SIGACTION
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

    AJG_FPRINTF(stderr, "Caught signal %d (%s)\n", signum, name);
    fprint_backtrace(stderr);
    std::exit(signum);
}

#ifdef AJG_SYNTH_THROW_EXCEPTION
#undef AJG_SYNTH_THROW_EXCEPTION
#endif

#define AJG_SYNTH_THROW_EXCEPTION ajg::synth::debug::throw_exception

template <class Exception>
BOOST_ATTRIBUTE_NORETURN
inline void throw_exception(Exception const& e) {
    std::string const name = unmangle(typeid(Exception).name());
    AJG_FPRINTF(stderr, "Exception of type `%s` about to be thrown\n", name.c_str());
    fprint_backtrace(stderr, 1);
    boost::throw_exception(e);
}

inline void terminate_handler() {
    AJG_FPRINTF(stderr, "Terminated\n");
    fprint_backtrace(stderr, 1);
    std::exit(EXIT_FAILURE);
}

inline void unexpected_handler() {
    AJG_FPRINTF(stderr, "Unexpected exception\n");
    fprint_backtrace(stderr, 1);
    std::exit(EXIT_FAILURE);
}

inline void set_handlers() {
#if HAS_SIGACTION

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

#endif // HAS_SIGACTION

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
    AJG_FPRINTF(stderr, "%s in %s() at %s:%ld\n", expression,
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
    AJG_FPRINTF(stderr, "%s [%s] in %s() at %s:%ld\n", expression, message,
        ajg::synth::debug::abbreviate(function).c_str(), file, line);
    ajg::synth::debug::fprint_backtrace(stderr, 1);
    std::exit(EXIT_FAILURE);
}

} // namespace boost

#endif // AJG_SYNTH_DEBUG_NO_HANDLERS

#endif // AJG_SYNTH_DEBUG_HPP_INCLUDED
