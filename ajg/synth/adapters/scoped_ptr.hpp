//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_SCOPED_PTR_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_SCOPED_PTR_HPP_INCLUDED

// #include <ajg/synth/adapters/pointer.hpp>
#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/forwarding_adapter.hpp>

namespace boost {
template <class T> class scoped_ptr;
}

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for boost::scoped_ptr
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class T> // NOTE: Adapted as a const reference since scoped_ptr is noncopyable.
struct adapter<Value, boost::scoped_ptr<T> >  : forwarding_adapter<Value, T, boost::scoped_ptr<T> const*, adapter<Value, boost::scoped_ptr<T> > > {
    adapter(boost::scoped_ptr<T> const& adapted) : forwarding_adapter<Value, T, boost::scoped_ptr<T> const*, adapter<Value, boost::scoped_ptr<T> > >(&adapted) {}

    template <class A> A forward() { return A(boost::ref(*this->adapted()->get())); }
    template <class A> A forward() const { return A(boost::cref(*this->adapted()->get())); }
    bool                 valid() const { return this->adapted()->get() != 0; }
};

/*
template <class Value, class T>
struct adapter<Value, boost::scoped_ptr<T> > : adapter<Value, T*> {
    adapter(boost::scoped_ptr<T> const& adapted) : adapter<Value, T*>(adapted.get()) {}
};
*/

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_SCOPED_PTR_HPP_INCLUDED

