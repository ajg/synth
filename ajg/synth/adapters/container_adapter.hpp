//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_CONTAINER_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_CONTAINER_ADAPTER_HPP_INCLUDED

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

// TODO: Move to own file.
template <class Value, class Adapted, class Specialized, class Iterator, type_flags Flags>
struct range_adapter                      : concrete_adapter<Value, Adapted, Flags, Specialized> {
    range_adapter(Adapted const& adapted) : concrete_adapter<Value, Adapted, Flags, Specialized>(adapted) {}

    virtual optional<typename Value::range_type> get_range() const override { return typename Value::range_type(this->begin(), this->end()); } // TODO[c++11]: Use std::begin & std::end.

  protected:

    // TODO: Use CRTP to eliminate virtual call.
    // TODO[c++11]: Use std::begin & std::end instead.
    virtual Iterator begin() const = 0;
    virtual Iterator end()   const = 0;
};

// TODO: Use range_adapter.
template <class Value, class Adapted, type_flags Flags>
struct container_adapter                      : concrete_adapter<Value, Adapted, type_flags(Flags | container)> {
    container_adapter(Adapted const& adapted) : concrete_adapter<Value, Adapted, type_flags(Flags | container)>(adapted) {}

    virtual optional<typename Value::range_type> get_range() const override { return typename Value::range_type(this->adapted().begin(), this->adapted().end()); } // TODO[c++11]: Use std::begin & std::end.
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_CONTAINER_ADAPTER_HPP_INCLUDED

