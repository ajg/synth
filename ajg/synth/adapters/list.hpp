//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_LIST_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_LIST_HPP_INCLUDED

#include <list>

#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for std::list
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, std::list<T> >  : container_adapter<Behavior, std::list<T> > {
    adapter(std::list<T> const& adapted) : container_adapter<Behavior, std::list<T> >(adapted) {}
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_LIST_HPP_INCLUDED

