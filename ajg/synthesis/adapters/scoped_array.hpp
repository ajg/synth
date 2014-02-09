//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_ADAPTERS_SCOPED_ARRAY_HPP_INCLUDED
#define AJG_SYNTHESIS_ADAPTERS_SCOPED_ARRAY_HPP_INCLUDED

#include <ajg/synthesis/adapters/array.hpp>
#include <ajg/synthesis/adapters/adapter.hpp>
#include <ajg/synthesis/adapters/detail/smart_ptr_fwd.hpp>

namespace ajg {
namespace synthesis {

//
// specialization for boost::scoped_array
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, scoped_array<T> >
    : public adapter<Traits, T[]> {

    adapter(scoped_array<T> const& adapted, size_t const length)
        : adapter<Traits, T[]>(adapted.get(), length) {}
};

/*
template <class Traits, class T>
struct adapter<Traits, scoped_array<T> >
    : public forwarding_adapter<Traits, T[], scoped_array<T> > {

    adapter(scoped_array<T> const& adapted, size_t const length)
        : adapted_(adapted), length_(length) {}
    scoped_array<T> const& adapted_;
    size_t const length_;

    template <class A> A forward() const { return A(boost::cref(*reinterpret_cast<T(*)[]>(adapted_.get())), length_); }
    // T (&get() const)[] { return *reinterpret_cast<T(*)[]>(adapted_.get()); }
    bool valid() const { return adapted_; }
};*/

}} // namespace ajg::synthesis

#endif // AJG_SYNTHESIS_ADAPTERS_SCOPED_ARRAY_HPP_INCLUDED

