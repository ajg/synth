//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_TESTING_HPP_INCLUDED
#define AJG_TESTING_HPP_INCLUDED

#include <ajg/synth/config.hpp>

#include <string>
#include <iostream>

#include <boost/static_assert.hpp>

#if AJG_SYNTH_COMPILER_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wold-style-cast"
#    pragma clang diagnostic ignored "-Wunused-function"
#    pragma clang diagnostic ignored "-Wunused-variable"
#elif AJG_SYNTH_COMPILER_GCC
#    if AJG_SYNTH_COMPILER_GCC_VERSION >= 406
#        pragma GCC diagnostic push
#    endif
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#    pragma GCC diagnostic ignored "-Wunused-function"
#    pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>

#if AJG_SYNTH_COMPILER_CLANG
#    pragma clang diagnostic pop
#elif AJG_SYNTH_COMPILER_GCC && (AJG_SYNTH_COMPILER_GCC_VERSION >= 406)
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

#define AJG_TESTING_BEGIN
#define TEST_NUMBER()

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
//     TODO: Uppercase.
////////////////////////////////////////////////////////////////////////////////////////////////////

#define ensure_throws(type, expr) \
    do { try { expr; fail("exception of type " #type " not thrown"); } \
         catch (type const&) { } \
       } while (0)

//
// unit_test
//     TODO: Uppercase.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
#    define AJG_DEBUG_RESET_COUNT() (ajg::synth::debug::count = 0)
#else
#    define AJG_DEBUG_RESET_COUNT() ((void) 0)
#endif

#define unit_test(name) \
    namespace tut { template<> template<> \
    void group_type::object::test<ajg::detail::check_test_number<__LINE__>::value>() { \
        set_test_name(#name); \
        AJG_DEBUG_RESET_COUNT(); \

// NOTE: Must be in an anonymous namespace to avoid linker errors (duplicate symbols.)
namespace { struct no_data {}; }

} // namespace detail

//
// test_group
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = detail::no_data>
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
