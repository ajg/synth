//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_POINTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_POINTER_HPP_INCLUDED

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for native pointers
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, T*>
    : public forwarding_adapter<Traits, T, T*> {

    adapter(T* adapted) : adapted_(adapted) {}
    T* adapted_;

 // template <class Adapter> optional<Adapter> forward() const {
 //     return adapted_ ? Adapter(ref(*adapted_)) : none;
 // }


    template <class A> A forward() const { return A(boost::ref(*adapted_)); }
    // T const& get() const { return *adapted_; }
    bool valid() const { return adapted_ != 0; }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_POINTER_HPP_INCLUDED
