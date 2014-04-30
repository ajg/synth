//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TESTING_HPP_INCLUDED
#define AJG_SYNTH_TESTING_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <string>
#include <iostream>

#include <boost/static_assert.hpp>

// TODO: Wrap this into AJG_SYNTH_EXTERNAL_PUSH in support.hpp
#if AJG_SYNTH_IS_COMPILER_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wold-style-cast"
#    pragma clang diagnostic ignored "-Wunused-function"
#    pragma clang diagnostic ignored "-Wunused-variable"
#elif AJG_SYNTH_IS_COMPILER_GCC
#    if AJG_SYNTH_COMPILER_VERSION >= 406
#        pragma GCC diagnostic push
#    endif
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#    pragma GCC diagnostic ignored "-Wunused-function"
#    pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>

// TODO: Wrap this into AJG_SYNTH_EXTERNAL_POP in support.hpp
#if AJG_SYNTH_IS_COMPILER_CLANG
#    pragma clang diagnostic pop
#elif AJG_SYNTH_IS_COMPILER_GCC && (AJG_SYNTH_COMPILER_VERSION >= 406)
#    pragma GCC diagnostic pop
#endif

namespace ajg {
namespace synth {

std::size_t const max_tests_per_file = AJG_SYNTH_CONFIG_MAX_TEMPLATE_DEPTH - 4;

//
// check_test_number
////////////////////////////////////////////////////////////////////////////////////////////////////

template <int N>
struct check_test_number {
    BOOST_STATIC_ASSERT(N <= max_tests_per_file);
    BOOST_STATIC_CONSTANT(int, value = N);
};

//
// ensure_throws
//     TODO[c++11]: Replace with function taking a lambda.
////////////////////////////////////////////////////////////////////////////////////////////////////

#define ensure_throws(type, expr) \
    do { try { expr; fail("exception of type " #type " not thrown"); } \
         catch (type const&) { } \
       } while (0)

//
// wensure_equals
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T

inline void wensure_equals(std::wstring const& expect, std::wstring const& actual) {
    tut::ensure_equals(actual.length(), expect.length());
    tut::ensure(actual == expect); // XXX: tut's ensure_equals doesn't play well with wchar_t.
}

#endif

//
// no_data
////////////////////////////////////////////////////////////////////////////////////////////////////

// NOTE: Must be in an anonymous namespace to avoid linker errors (duplicate symbols.)
namespace { struct no_data {}; }

//
// unit_test
//     TODO: Rename AJG_SYNTH_UNIT_TEST.
//     TODO[c++11]: Replace with function taking a lambda.
////////////////////////////////////////////////////////////////////////////////////////////////////

#define unit_test(name) \
    namespace tut { template<> template<> \
    void test_group_type::object::test</**/::ajg::synth::check_test_number<__LINE__>::value>() { \
        set_test_name(#name); \
        AJG_SYNTH_ONLY_DEBUG(::ajg::synth::debug::reset()); \
        AJG_SYNTH_ONLY_DEBUG(::ajg::synth::debug::quiet(true)); \

//
// AJG_SYNTH_TEST_GROUP_WITH_DATA
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_TEST_GROUP_WITH_DATA(name, data) \
    struct test_group_type : ::ajg::synth::test_group<data> { \
        test_group_type() : ::ajg::synth::test_group<data>(name) {} } \
    static const group

//
// AJG_SYNTH_TEST_GROUP
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_TEST_GROUP(name) AJG_SYNTH_TEST_GROUP_WITH_DATA(name, AJG_SYNTH_EMPTY)

//
// test_group
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = no_data>
struct test_group : public tut::test_group<T, max_tests_per_file> {
    typedef tut::test_group<T, max_tests_per_file> base_type;
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

}} // namespace ajg::synth

#endif // AJG_SYNTH_TESTING_HPP_INCLUDED
