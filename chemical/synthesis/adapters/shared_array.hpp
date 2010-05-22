
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ADAPTERS_SHARED_ARRAY_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ADAPTERS_SHARED_ARRAY_HPP_INCLUDED

#include <chemical/synthesis/adapters/array.hpp>
#include <chemical/synthesis/adapters/adapter.hpp>
#include <chemical/synthesis/adapters/detail/smart_ptr_fwd.hpp>

namespace chemical {
namespace synthesis {

//
// specialization for boost::shared_array
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, shared_array<T> >
    : public adapter<Traits, T[]> {

    adapter(shared_array<T> const& adapted, size_t const length)
        : adapter<Traits, T[]>(adapted.get(), length) {}
};

/*
template <class Traits, class T>
struct adapter<Traits, shared_array<T> >
    : public forwarding_adapter<Traits, T[], shared_array<T> > {

    adapter(shared_array<T> const& adapted, size_t const length)
        : adapted_(adapted), length_(length) {}
    shared_array<T> adapted_;
    size_t const length_;


    template <class A> A forward() const { return A(cref(*reinterpret_cast<T(*)[]>(adapted_.get())), length_); }
    // T (&get() const)[] { return *reinterpret_cast<T(*)[]>(adapted_.get()); }
    bool valid() const { return adapted_; }
};
*/

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_ADAPTERS_SHARED_ARRAY_HPP_INCLUDED

