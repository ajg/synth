//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_CONTAINER_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_CONTAINER_ADAPTER_HPP_INCLUDED

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {

template <class Behavior, class Adapted>
struct container_adapter : concrete_adapter<Behavior, Adapted> {
    container_adapter(Adapted const& adapted) : concrete_adapter<Behavior, Adapted>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);

    boolean_type to_boolean() const { return !this->adapted().empty(); }
    range_type   to_range()   const { return range_type(this->adapted().begin(), this->adapted().end()); } // TODO[c++11]: Use std::begin & std::end.

    void output(ostream_type& out) const { behavior_type::enumerate(*this, out); }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_CONTAINER_ADAPTER_HPP_INCLUDED

