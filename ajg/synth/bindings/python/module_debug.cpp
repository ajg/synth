//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#define BOOST_ENABLE_ASSERT_HANDLER
#include <boost/python.hpp>

#include <ajg/synth/engines/django.hpp>
// #include <ajg/synth/engines/ssi.hpp>
// #include <ajg/synth/engines/tmpl.hpp>
#include <ajg/synth/bindings/python/binding_debug.hpp>


struct test_tags : boost::fusion::vector8
    < ajg::synth::django::comment_tag
    , ajg::synth::django::for_tag
    , ajg::synth::django::if_tag
    , ajg::synth::django::variable_tag
    , ajg::synth::django::block_tag
    , ajg::synth::django::extends_tag
    , ajg::synth::django::include_tag
    , ajg::synth::django::now_tag
    > {};

struct test_filters : boost::fusion::vector2
    < ajg::synth::django::yesno_filter
    , ajg::synth::django::safe_filter
    > {};

struct test_library
    : boost::mpl::pair
        < test_tags
        , test_filters
        >
{};

void set_handlers();

BOOST_PYTHON_MODULE(synth)
{
    set_handlers();
    using namespace boost::python;
    typedef ajg::synth::python::binding<char
      , ajg::synth::django::engine<test_library>
      // , ajg::synth::ssi::engine<>
      // , ajg::synth::tmpl::engine<>
      >                                 Template;
    typedef Template::string_type       String;

    def("version", ajg::synth::python::version);

    class_<Template>("Template", init<String, String>())
        .def("render_to_string", &Template::render_to_string)
    ;
}

#include <iostream>
#include <sstream>
#include <signal.h>
#include <cxxabi.h>
#include <execinfo.h>

void print_stack_trace(size_t numskip = 0) {
    std::vector<void*> bt(100);
    bt.resize(backtrace(&(*bt.begin()), bt.size()));
    char **btsyms = backtrace_symbols(&(*bt.begin()), bt.size());
    if (btsyms) {
        for (size_t i = numskip; i < bt.size(); i++) {
            std::istringstream in(btsyms[i]);
            int idx = 0; std::string nt, addr, mangled;
            in >> idx >> nt >> addr >> mangled;
            if (mangled == "start") break;
            int status = 0;
            char *demangled = abi::__cxa_demangle(mangled.c_str(), 0, 0, &status);

            std::string frame = (status==0) ?
              std::string(demangled, demangled+strlen(demangled)) :
              std::string(mangled.begin(), mangled.end());
            std::cerr << idx << " " << nt << " " << frame << std::endl;
            free(demangled);
        }
        free(btsyms);
    }
}

void abort_handler(int signum, siginfo_t *info, void *context)
{
   // associate each signal with a signal name string.
   const char* name = NULL;
   switch( signum )
   {
   case SIGABRT: name = "SIGABRT";  break;
   case SIGSEGV: name = "SIGSEGV";  break;
   case SIGBUS:  name = "SIGBUS";   break;
   case SIGILL:  name = "SIGILL";   break;
   case SIGFPE:  name = "SIGFPE";   break;
   }

   // Notify the user which signal was caught. We use printf, because this is the
   // most basic output function. Once you get a crash, it is possible that more
   // complex output systems like streams and the like may be corrupted. So we
   // make the most basic call possible to the lowest level, most
   // standard print function.
   if ( name )
      fprintf( stderr, "Caught signal %d (%s)\n", signum, name );
   else
      fprintf( stderr, "Caught signal %d\n", signum );

   // Dump a stack trace.
   // This is the function we will be implementing next.
   print_stack_trace();

   // If you caught one of the above signals, it is likely you just
   // want to quit your program right now.
   exit( signum );
}

 void
     termination_handler (int signum)
     {
        fprintf( stderr, "Caught signal %d\n", signum );
        print_stack_trace();

     }

void set_handlers() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = abort_handler;
    sigemptyset( &sa.sa_mask );

    sigaction( SIGABRT, &sa, NULL );
    sigaction( SIGSEGV, &sa, NULL );
    sigaction( SIGBUS,  &sa, NULL );
    sigaction( SIGILL,  &sa, NULL );
    sigaction( SIGFPE,  &sa, NULL );
    sigaction( SIGPIPE, &sa, NULL );

    signal(SIGSEGV, termination_handler);

    fprintf( stderr, "==Handlers Set==\n" );
}

namespace boost {
    void assertion_failed(char const * expr, char const * function, char const * file, long line) {
        std::cerr << expr << " in " << function << " " << file << ":" << line << std::endl;
        print_stack_trace();
    }
}
