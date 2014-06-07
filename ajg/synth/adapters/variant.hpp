//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_VARIANT_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_VARIANT_HPP_INCLUDED

#include <boost/variant/variant_fwd.hpp>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for boost::variant
////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: Use forwarding_adapter or forward the actual value elsehow.
template <class Value, BOOST_VARIANT_ENUM_PARAMS(class T)>
struct adapter<Value, boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> > : concrete_adapter<Value, boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>, container> {
    adapter(boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> const& adapted) : concrete_adapter<Value, boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>, container>(adapted) {}
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_OPTIONAL_HPP_INCLUDED

