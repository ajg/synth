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
// specialization for void*
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct adapter<Value, void*>     : concrete_adapter<Value, void*> {
    adapter(void* const adapted) : concrete_adapter<Value, void*>(adapted) {}
};

template <class Value>
struct adapter<Value, void const*>     : concrete_adapter<Value, void const*> {
    adapter(void const* const adapted) : concrete_adapter<Value, void const*>(adapted) {}
};

//
// specialization for native pointers
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class T>
struct adapter<Value, T*>     : forwarding_adapter<Value, T, T*> {
    adapter(T* const adapted) : forwarding_adapter<Value, T, T*>(adapted) {}

    template <class A> A forward() { return A(boost::ref(*this->adapted())); }
    template <class A> A forward() const { return A(boost::cref(*this->adapted())); }
    bool                 valid() const { return this->adapted() != 0; }
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_POINTER_HPP_INCLUDED
