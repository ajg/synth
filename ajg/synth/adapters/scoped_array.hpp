//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_SCOPED_ARRAY_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_SCOPED_ARRAY_HPP_INCLUDED

#include <ajg/synth/adapters/array.hpp>
#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/forwarding_adapter.hpp>

namespace boost {
template <class T> class scoped_array;
}

namespace ajg {
namespace synth {

//
// specialization for boost::scoped_array
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, boost::scoped_array<T> > : adapter<Behavior, T[]> {
    adapter(boost::scoped_array<T> const& adapted, std::size_t const length) : adapter<Behavior, T[]>(adapted.get(), length) {}
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_SCOPED_ARRAY_HPP_INCLUDED

