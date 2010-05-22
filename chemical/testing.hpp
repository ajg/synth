
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_TESTING_HPP_INCLUDED
#define CHEMICAL_TESTING_HPP_INCLUDED

#include <iostream>

#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>

// Disable long long for gcc's -pedantic.
#include <boost/config.hpp>
#undef BOOST_HAS_LONG_LONG

namespace chemical {
namespace detail {

#ifdef _WIN32
  #define CHEMICAL_WIN32_DIVERGE(a, b) a
#else
  #define CHEMICAL_WIN32_DIVERGE(a, b) b
#endif

#if defined(_MSC_VER) || (defined(__GNUC__)) // TODO: g++ 4.3+ only.
  static const int __counter_begin = __COUNTER__;
  #define TEST_NUMBER() (__COUNTER__ - chemical::detail::__counter_begin)
#else
  #define TEST_NUMBER() (__LINE__ >> 2)
#endif


#define ensure_throws(type, expr) \
    do { try { expr; fail("exception of type " #type " not thrown"); } \
         catch (type const&) { } \
       } while (0)


#define unit_test(name) \
    namespace tut { template<> template<> \
    void group_type::object::test<TEST_NUMBER()>() { \
        set_test_name(#name);

namespace {
struct empty {};
} // namespace anonymous

} // namespace detail

template <class T = detail::empty>
struct test_group : tut::test_group<T> {
    template <class U> test_group(U const& u) : tut::test_group<T>(u) {}
};

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

  private:

    std::string name_;
    tut::reporter reporter_;
    tut::test_runner_singleton runner_;
};



} // namespace chemical

#endif // CHEMICAL_TESTING_HPP_INCLUDED
