//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_MEMORY_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_MEMORY_HPP_INCLUDED

#include <memory>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for std::auto_ptr
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, std::auto_ptr<T> >
    : public forwarding_adapter<Traits, T, std::auto_ptr<T> > {

    adapter(std::auto_ptr<T> const& adapted) : adapted_(adapted) {}
    std::auto_ptr<T> const& adapted_;


    template <class A> A forward() const { return A(boost::cref(*adapted_)); }
    // T&   get()   const { return *adapted_; }
    bool valid() const { return adapted_.get() != 0; }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_MEMORY_HPP_INCLUDED

