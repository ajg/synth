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
struct adapter<Value, boost::none_t>      : concrete_adapter<Value, boost::none_t, unit> {
    adapter(boost::none_t const& adapted) : concrete_adapter<Value, boost::none_t, unit>(adapted) {}
};

}}} // namespace ajg::synth::adapters

namespace std {

template <>
struct less<boost::none_t> {
    bool operator()(boost::none_t const&, boost::none_t const&) const {
        return false;
    }
};

} // namespace std

/*
namespace boost {

inline bool operator <(none_t const&, none_t const&) { return false; }

} // namespace boost
*/

#endif // AJG_SYNTH_ADAPTERS_NONE_HPP_INCLUDED

