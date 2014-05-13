//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_NULL_VALUE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_NULL_VALUE_HPP_INCLUDED

#include <functional>

#include <ajg/synth/engines/base_value.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace null {

template <class Traits>
struct value : base_value<Traits, value> {
  public:

    typedef value                                                               value_type;

    typedef typename value_type::facade_type                                    facade_type;
    typedef typename value_type::behavior_type                                  behavior_type;

    typedef typename behavior_type::sequence_type                               sequence_type;
    typedef typename behavior_type::mapping_type                                mapping_type;

  public:

                                         value() : facade_type() {}
    template <class T>                   value(T const& t) : facade_type(t) {}
    template <class T, class U>          value(T const& t, U const& u) : facade_type(t, u) {}
    template <class T, class U, class V> value(T const& t, U const& u, V const& v) : facade_type(t, u, v) {}
};

}}}} // namespace ajg::synth::engines::null

namespace std {

template <class Traits>
struct equal_to<ajg::synth::engines::null::value<Traits> > {
    bool operator()( ajg::synth::engines::null::value<Traits> const& a
                   , ajg::synth::engines::null::value<Traits> const& b
                   ) const {
        return true;
    }
};

template <class Traits>
struct less<ajg::synth::engines::null::value<Traits> > {
    bool operator()( ajg::synth::engines::null::value<Traits> const& a
                   , ajg::synth::engines::null::value<Traits> const& b
                   ) const {
        return false;
    }
};

} // namespace std

#endif // AJG_SYNTH_ENGINES_NULL_VALUE_HPP_INCLUDED

