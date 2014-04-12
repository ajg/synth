//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_TMPL_VALUE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_TMPL_VALUE_HPP_INCLUDED

#include <ajg/synth/value_facade.hpp>

namespace ajg {
namespace synth {
namespace tmpl {

template <class Traits>
struct value : value_facade<Traits, value> {
  public:

    typedef value                               value_type;
    typedef typename value_type::facade_type    facade_type;

  public:

    value() : facade_type() {}
    template <class T> value(T const& t) : facade_type(t) {}
};

}}} // namespace ajg::synth::tmpl

#endif // AJG_SYNTH_ENGINES_TMPL_VALUE_HPP_INCLUDED
