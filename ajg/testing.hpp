//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_TESTING_HPP_INCLUDED
#define AJG_TESTING_HPP_INCLUDED

#include <string>
#include <iostream>

#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>

#include <boost/config.hpp>
// #undef BOOST_HAS_LONG_LONG // Disable long long for gcc's -pedantic.
#include <boost/static_assert.hpp>
#include <boost/preprocessor/stringize.hpp>

#define AJG_TESTING 1

#ifndef AJG_TESTING_MAX_TESTS_PER_FILE
#define AJG_TESTING_MAX_TESTS_PER_FILE 200
#endif

namespace ajg {
namespace detail {

//
// AJG_WIN32_DIVERGE:
//     Picks the first version for WIN32, otherwise the second.
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
  #define AJG_WIN32_DIVERGE(a, b) a
#else
  #define AJG_WIN32_DIVERGE(a, b) b
#endif

//
// AJG_TESTING_BEGIN, TEST_NUMBER
////////////////////////////////////////////////////////////////////////////////

#ifdef __COUNTER__ // E.g. MSVC, GCC 4.3+, Intel...

  #define AJG_TESTING_HAS_COUNTER
  #define AJG_TESTING_BEGIN \
    namespace ajg { namespace detail { \
        static unsigned const counter_start = __COUNTER__; \
    }}
  #define TEST_NUMBER() (__COUNTER__ - ajg::detail::counter_start)

#else // !__COUNTER__

  // Use line numbers in this case, which require a much higher
  // maximum template recursion depth from the compiler.
  #define AJG_TESTING_BEGIN // Nothing.
  #define TEST_NUMBER() (__LINE__)

#endif

//
// check_test_number
////////////////////////////////////////////////////////////////////////////////

template <int N>
struct check_test_number {
    BOOST_STATIC_ASSERT(N <= AJG_TESTING_MAX_TESTS_PER_FILE);
    BOOST_STATIC_CONSTANT(int, value = N);
};

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
    void group_type::object::test<ajg::detail::check_test_number<TEST_NUMBER()>::value>() { \
        set_test_name(#name ":" BOOST_PP_STRINGIZE(__LINE__));

namespace {
    struct empty {};
} // namespace anonymous

} // namespace detail

//
// test_group
////////////////////////////////////////////////////////////////////////////////

template <class T = detail::empty>
struct test_group : public tut::test_group<T, AJG_TESTING_MAX_TESTS_PER_FILE> {
    typedef tut::test_group<T, AJG_TESTING_MAX_TESTS_PER_FILE> base_type;
    template <class U> test_group(U const& u) : base_type(u) {}
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
        return reporter_.all_ok();
    }

    bool run(std::string const group)
    try {
        std::cout << "Running group" << group
                  << " of the " << name_
                  << " test suite:" << std::endl;
        runner_.get().run_tests(group);
        return reporter_.all_ok();
    }
    catch (tut::no_such_group const&) {
        std::cerr << "No such group: " << group << std::endl;
        return false;
    }

  private:

    std::string                name_;
    tut::reporter              reporter_;
    tut::test_runner_singleton runner_;
};

} // namespace ajg

#endif // AJG_TESTING_HPP_INCLUDED
