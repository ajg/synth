//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_VECTOR_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_VECTOR_HPP_INCLUDED

#include <vector>

#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for std::vector
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, std::vector<T> >  : container_adapter<Behavior, std::vector<T> > {
    adapter(std::vector<T> const& adapted) : container_adapter<Behavior, std::vector<T> >(adapted) {}
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_VECTOR_HPP_INCLUDED

