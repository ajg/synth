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

//
// specialization for boost::scoped_ptr
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T> // NOTE: Adapted as a const reference since scoped_ptr is noncopyable.
struct adapter<Behavior, boost::scoped_ptr<T> >  : forwarding_adapter<Behavior, T, boost::scoped_ptr<T> const*, adapter<Behavior, boost::scoped_ptr<T> > > {
    adapter(boost::scoped_ptr<T> const& adapted) : forwarding_adapter<Behavior, T, boost::scoped_ptr<T> const*, adapter<Behavior, boost::scoped_ptr<T> > >(&adapted) {}
    template <class A> A forward() const { return A(boost::cref(*this->adapted_->get())); }
    typename Behavior::boolean_type valid() const { return this->adapted_->get() != 0; }
};

/*
template <class Behavior, class T>
struct adapter<Behavior, boost::scoped_ptr<T> > : adapter<Behavior, T*> {
    adapter(boost::scoped_ptr<T> const& adapted) : adapter<Behavior, T*>(adapted.get()) {}
};
*/

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_SCOPED_PTR_HPP_INCLUDED

