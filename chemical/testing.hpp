
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_TESTING_HPP_INCLUDED
#define CHEMICAL_TESTING_HPP_INCLUDED

#include <string>
#include <iostream>

#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>

// Disable long long for gcc's -pedantic.
#include <boost/config.hpp>
#undef BOOST_HAS_LONG_LONG

namespace chemical {
namespace detail {

//
// DEBUG_ONLY:
//     Conditional compilation using a macro prefix.
////////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
  #define DEBUG_ONLY
#else
  #define CAT_SLASH(c) /##c
  #define DEBUG_ONLY   CAT_SLASH(/)
#endif

//
// CHEMICAL_WIN32_DIVERGE:
//     Picks the first version for WIN32, otherwise the second.
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
  #define CHEMICAL_WIN32_DIVERGE(a, b) a
#else
  #define CHEMICAL_WIN32_DIVERGE(a, b) b
#endif

//
// CHEMICAL_TESTING_BEGIN, TEST_NUMBER
////////////////////////////////////////////////////////////////////////////////

#ifdef __COUNTER__ // E.g. MSVC, GCC 4.3+, Intel...

  #define CHEMICAL_TESTING_HAS_COUNTER
  #define CHEMICAL_TESTING_BEGIN \
    namespace chemical { namespace detail { \
        static unsigned const counter_start = __COUNTER__; \
    }}
  #define TEST_NUMBER() (__COUNTER__ - chemical::detail::counter_start)

#else // no __COUNTER__, so use __LINE__ / 4 instead:

  #define CHEMICAL_TESTING_BEGIN // Nothing.
  #define TEST_NUMBER() (__LINE__ >> 2)

#endif

//
// ensure_throws
////////////////////////////////////////////////////////////////////////////////

#define ensure_throws(type, expr) \
    do { try { expr; fail("exception of type " #type " not thrown"); } \
         catch (type const&) { } \
       } while (0)

//
// unit_test
////////////////////////////////////////////////////////////////////////////////

#define unit_test(name) \
    namespace tut { template<> template<> \
    void group_type::object::test<TEST_NUMBER()>() { \
        set_test_name(#name);

namespace {
    struct empty {};
} // namespace anonymous

} // namespace detail

//
// test_group
////////////////////////////////////////////////////////////////////////////////

template <class T = detail::empty>
struct test_group : tut::test_group<T> {
    template <class U> test_group(U const& u) : tut::test_group<T>(u) {}
};

//
// test_suite
////////////////////////////////////////////////////////////////////////////////

struct test_suite {
  public:

    test_suite(std::string const& name)
            : name_(name) {
        runner_.get().set_callback(&reporter_);
    }

    bool run() {
        std::cout << "Running the " << name_
                  << " test suite:" << std::endl;
        runner_.get().run_tests();
        return !reporter_.all_ok();
    }

    bool run(std::string const group) 
    try {
        std::cout << "Running group" << group
                  << " of the " << name_
                  << " test suite:" << std::endl;
        runner_.get().run_tests(group);
        return !reporter_.all_ok();
    }
    catch (tut::no_such_group const&) {
        std::cerr << "No such group: " << group << std::endl;
        return false;
    }

  private:

    std::string name_;
    tut::reporter reporter_;
    tut::test_runner_singleton runner_;
};

} // namespace chemical

#endif // CHEMICAL_TESTING_HPP_INCLUDED
