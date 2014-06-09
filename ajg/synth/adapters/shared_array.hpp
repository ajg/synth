//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_SHARED_ARRAY_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_SHARED_ARRAY_HPP_INCLUDED

#include <ajg/synth/adapters/array.hpp>
#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/forwarding_adapter.hpp>

namespace boost {
template <class T> class shared_array;
}

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for boost::shared_array
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class T>
struct adapter<Value, boost::shared_array<T> >                               : adapter<Value, T[]> {
    adapter(boost::shared_array<T> const& adapted, std::size_t const length) : adapter<Value, T[]>(adapted.get(), length) {}
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_SHARED_ARRAY_HPP_INCLUDED

