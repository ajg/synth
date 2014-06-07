//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_NUMERIC_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_NUMERIC_ADAPTER_HPP_INCLUDED

#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// numeric_adapter:
//     Base adapter implementation for primitive numeric types
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class Adapted, type_flags Flags>
struct numeric_adapter                      : concrete_adapter<Value, Adapted, type_flags(Flags | numeric | (boost::is_integral<Adapted>::value ? integral : 0) | (boost::is_floating_point<Adapted>::value ? floating : 0))> {
    numeric_adapter(Adapted const& adapted) : concrete_adapter<Value, Adapted, type_flags(Flags | numeric | (boost::is_integral<Adapted>::value ? integral : 0) | (boost::is_floating_point<Adapted>::value ? floating : 0))>(adapted) {}

    virtual optional<typename Value::number_type> get_number() const { return static_cast<typename Value::number_type>(this->adapted()); }
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_NUMERIC_ADAPTER_HPP_INCLUDED

