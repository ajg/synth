//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_SET_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_SET_HPP_INCLUDED

#include <set>

#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for std::set
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, std::set<T> >  : container_adapter<Behavior, std::set<T> > {
    adapter(std::set<T> const& adapted) : container_adapter<Behavior, std::set<T> >(adapted) {}
};

//
// specialization for std::multiset
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, std::multiset<T> >  : container_adapter<Behavior, std::multiset<T> > {
    adapter(std::multiset<T> const& adapted) : container_adapter<Behavior, std::multiset<T> >(adapted) {}
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_SET_HPP_INCLUDED
