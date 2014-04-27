//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_TESTING_HPP_INCLUDED
#define AJG_TESTING_HPP_INCLUDED

#define AJG_SYNTH_IS_TESTING 1

#include <ajg/synth/config.hpp>

#include <string>
#include <iostream>

#include <boost/static_assert.hpp>

// TODO: Wrap this into AJG_SYNTH_EXTERNAL_PUSH in config.hpp
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

// TODO: Wrap this into AJG_SYNTH_EXTERNAL_POP in config.hpp
#if AJG_SYNTH_IS_COMPILER_CLANG
#    pragma clang diagnostic pop
#elif AJG_SYNTH_IS_COMPILER_GCC && (AJG_SYNTH_COMPILER_VERSION >= 406)
#    pragma GCC diagnostic pop
#endif

namespace ajg {
// TODO: Move under synth namespace.
namespace detail {

std::size_t const max_tests_per_file = AJG_SYNTH_CONFIG_MAX_TEMPLATE_DEPTH - 4;

//
// check_test_number
////////////////////////////////////////////////////////////////////////////////////////////////////

template <int N>
struct check_test_number {
    BOOST_STATIC_ASSERT(N <= max_tests_per_file);
    BOOST_STATIC_CONSTANT(int, value = N);
};

// NOTE: Must be in an anonymous namespace to avoid linker errors (duplicate symbols.)
namespace { struct no_data {}; }

} // namespace detail

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
//     TODO: Rename AJG_SYNTH_UNIT_TEST.
////////////////////////////////////////////////////////////////////////////////////////////////////

#define unit_test(name) \
    namespace tut { template<> template<> \
    void group_type::object::test<ajg::detail::check_test_number<__LINE__>::value>() { \
        set_test_name(#name); \
        AJG_SYNTH_DEBUG_RESET_COUNT(0); \


 // TODO: Refactor into AJG_SYNTH_TEST_GROUP(name)
#define AJG_TESTING_BEGIN

//
// test_group
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = detail::no_data>
struct test_group : public tut::test_group<T, detail::max_tests_per_file> {
    typedef tut::test_group<T, detail::max_tests_per_file> base_type;
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
