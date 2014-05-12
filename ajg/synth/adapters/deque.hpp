//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_DEQUE_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_DEQUE_HPP_INCLUDED

#include <deque>

#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for std::deque
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, std::deque<T> >  : container_adapter<Behavior, std::deque<T> > {
    adapter(std::deque<T> const& adapted) : container_adapter<Behavior, std::deque<T> >(adapted) {}
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_DEQUE_HPP_INCLUDED

