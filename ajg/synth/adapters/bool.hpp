//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_BOOL_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_BOOL_HPP_INCLUDED

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for bool
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct adapter<Value, bool>      : concrete_adapter<Value, bool, boolean> {
    adapter(bool const& adapted) : concrete_adapter<Value, bool, boolean>(adapted) {}

    virtual optional<typename Value::boolean_type> get_boolean() const { return this->adapted(); }
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_BOOL_HPP_INCLUDED

