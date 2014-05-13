//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_MEMORY_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_MEMORY_HPP_INCLUDED

#include <memory>

#include <ajg/synth/adapters/forwarding_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for std::auto_ptr
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T> // NOTE: Adapted as a const reference since scoped_ptr is noncopyable.
struct adapter<Behavior, std::auto_ptr<T> >  : forwarding_adapter<Behavior, T, std::auto_ptr<T> const*, adapter<Behavior, std::auto_ptr<T> > > {
    adapter(std::auto_ptr<T> const& adapted) : forwarding_adapter<Behavior, T, std::auto_ptr<T> const*, adapter<Behavior, std::auto_ptr<T> > >(&adapted) {}

    template <class A> A forward() const { return A(boost::cref(*this->adapted()->get())); }
    typename Behavior::boolean_type valid() const { return this->adapted()->get() != 0; }
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_MEMORY_HPP_INCLUDED

