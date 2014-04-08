//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_NULL_VALUE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_NULL_VALUE_HPP_INCLUDED

#include <functional>

#include <ajg/synth/value_facade.hpp>

namespace ajg {
namespace synth {

template <class Char>
struct null_value : value_facade<Char, null_value<Char> > {
  public:

    typedef value_facade<Char, null_value> base_type;

  public:

    AJG_SYNTH_VALUE_CONSTRUCTORS(null_value, base_type, {})
};

}} // namespace ajg::synth

namespace std {

template<class Char>
struct equal_to<ajg::synth::null_value<Char> > {
    bool operator()( ajg::synth::null_value<Char> const& a
                   , ajg::synth::null_value<Char> const& b
                   ) const {
        return true;
    }
};

template<class Char>
struct less<ajg::synth::null_value<Char> > {
    bool operator()( ajg::synth::null_value<Char> const& a
                   , ajg::synth::null_value<Char> const& b
                   ) const {
        return false;
    }
};

} // namespace std

#endif // AJG_SYNTH_ENGINES_NULL_VALUE_HPP_INCLUDED

