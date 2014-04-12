//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_TESTING_HPP_INCLUDED
#define AJG_TESTING_HPP_INCLUDED

#include <string>
#include <iostream>

#include <boost/static_assert.hpp>
#include <boost/preprocessor/stringize.hpp>

#ifndef _MSC_VER
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#    pragma GCC diagnostic ignored "-Wunused-function"
#    pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>

#ifndef _MSC_VER
#    pragma GCC diagnostic pop
#endif

#define AJG_TESTING 1

#ifndef TEMPLATE_DEPTH
#define TEMPLATE_DEPTH 1024
#endif

#ifndef AJG_TESTING_MAX_TESTS_PER_FILE
#define AJG_TESTING_MAX_TESTS_PER_FILE (TEMPLATE_DEPTH - 5)
#endif

namespace ajg {
namespace detail {

//
// AJG_COUNTER, AJG_TESTING_BEGIN, TEST_NUMBER
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __COUNTER__
#define AJG_COUNTER __COUNTER__
#else
// Use line numbers in this case, which require a much higher
// maximum template recursion depth from the compiler.
#define AJG_COUNTER __LINE__
#endif

#define AJG_TESTING_BEGIN \
  namespace ajg { \
  namespace detail { \
      static unsigned const counter_start = AJG_COUNTER; \
  }} // namespace ajg::detail
#define TEST_NUMBER() (AJG_COUNTER - ajg::detail::counter_start)

//
// check_test_number
////////////////////////////////////////////////////////////////////////////////////////////////////

template <int N>
struct check_test_number {
    BOOST_STATIC_ASSERT(N <= AJG_TESTING_MAX_TESTS_PER_FILE);
    BOOST_STATIC_CONSTANT(int, value = N);
};

//
// ensure_throws
////////////////////////////////////////////////////////////////////////////////////////////////////

#define ensure_throws(type, expr) \
    do { try { expr; fail("exception of type " #type " not thrown"); } \
         catch (type const&) { } \
       } while (0)

//
// unit_test
////////////////////////////////////////////////////////////////////////////////////////////////////

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
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = detail::empty>
struct test_group : public tut::test_group<T, AJG_TESTING_MAX_TESTS_PER_FILE> {
    typedef tut::test_group<T, AJG_TESTING_MAX_TESTS_PER_FILE> base_type;
    test_group(char const* const name) : base_type(name) {}
};

//
// test_suite
////////////////////////////////////////////////////////////////////////////////////////////////////

struct test_suite {
  public:

    test_suite(std::string const& name) : name_(name) {
        runner_.get().set_callback(&reporter_);
    }

    bool run() {
        std::cerr << "Running the " << name_ << " test suite:" << std::endl;
        runner_.get().run_tests();
        return reporter_.all_ok();
    }

    bool run(std::string const group)
    try {
        std::cerr << "Running group" << group << " of the " << name_ << " test suite:" << std::endl;
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
