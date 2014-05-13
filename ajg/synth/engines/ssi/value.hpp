//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_SSI_VALUE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_SSI_VALUE_HPP_INCLUDED

#include <ajg/synth/engines/base_value.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace ssi {

template <class Traits>
struct value : base_value<Traits, value> {
  public:

    typedef value                                                               value_type;

    typedef typename value_type::base_type                                      base_type;
    typedef typename value_type::behavior_type                                  behavior_type;

    typedef typename behavior_type::sequence_type                               sequence_type;
    typedef typename behavior_type::mapping_type                                mapping_type;

  public:

    value() : base_type() {}
    template <class T> value(T const& t) : base_type(t) {}
};

}}}} // namespace ajg::synth::engines::ssi

#endif // AJG_SYNTH_ENGINES_SSI_VALUE_HPP_INCLUDED
