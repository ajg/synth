//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_NONE_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_NONE_HPP_INCLUDED

#include <boost/none_t.hpp>

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for boost::none_t
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct adapter<Value, boost::none_t>      : concrete_adapter_without_operators<Value, boost::none_t, unit> {
    adapter(boost::none_t const& adapted) : concrete_adapter_without_operators<Value, boost::none_t, unit>(adapted) {}
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_NONE_HPP_INCLUDED

