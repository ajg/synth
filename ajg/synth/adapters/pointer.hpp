//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_POINTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_POINTER_HPP_INCLUDED

#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/forwarding_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for native pointers
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, T*>  : forwarding_adapter<Behavior, T, T*> {
    adapter(T* const adapted) : forwarding_adapter<Behavior, T, T*>(adapted) {}

    template <class A> A forward() const { return A(boost::ref(*this->adapted())); }
    typename Behavior::boolean_type valid() const { return this->adapted() != 0; }

 // template <class Adapter> optional<Adapter> forward() const {
 //     return this->adapted() ? Adapter(boost::ref(*adapted_)) : boost::none;
 // }
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_POINTER_HPP_INCLUDED
