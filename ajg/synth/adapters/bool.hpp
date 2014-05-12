//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_BOOL_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_BOOL_HPP_INCLUDED

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for bool
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior>
struct adapter<Behavior, bool>   : concrete_adapter<Behavior, bool> {
    adapter(bool const& adapted) : concrete_adapter<Behavior, bool>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(bool);

    floating_type to_floating()  const { return this->adapted_ ? 1 : 0; }
    boolean_type to_boolean() const { return this->adapted_; }

    void input (istream_type& in)        { in >> this->adapted_; }
    void output(ostream_type& out) const { out << (this->adapted_ ? "True" : "False"); } // TODO: Configure via Traits.
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_BOOL_HPP_INCLUDED

