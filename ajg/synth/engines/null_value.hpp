//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_NULL_VALUE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_NULL_VALUE_HPP_INCLUDED

#include <functional>

#include <ajg/synth/value_facade.hpp>

namespace ajg {
namespace synth {

template <class Traits>
struct null_value : value_facade<Traits, null_value> {
  public:

    typedef null_value                          value_type;
    typedef typename value_type::facade_type    facade_type;

  public:

                                         null_value() : facade_type() {}
    template <class T>                   null_value(T const& t) : facade_type(t) {}
    template <class T, class U>          null_value(T const& t, U const& u) : facade_type(t, u) {}
    template <class T, class U, class V> null_value(T const& t, U const& u, V const& v) : facade_type(t, u, v) {}
};

}} // namespace ajg::synth

namespace std {

template <class Traits>
struct equal_to<ajg::synth::null_value<Traits> > {
    bool operator()( ajg::synth::null_value<Traits> const& a
                   , ajg::synth::null_value<Traits> const& b
                   ) const {
        return true;
    }
};

template <class Traits>
struct less<ajg::synth::null_value<Traits> > {
    bool operator()( ajg::synth::null_value<Traits> const& a
                   , ajg::synth::null_value<Traits> const& b
                   ) const {
        return false;
    }
};

} // namespace std

#endif // AJG_SYNTH_ENGINES_NULL_VALUE_HPP_INCLUDED

