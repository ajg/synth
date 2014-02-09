
//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ADAPTERS_SHARED_PTR_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ADAPTERS_SHARED_PTR_HPP_INCLUDED

#include <chemical/synthesis/adapters/adapter.hpp>
#include <chemical/synthesis/adapters/detail/smart_ptr_fwd.hpp>

namespace chemical {
namespace synthesis {

//
// specialization for boost::shared_ptr
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, shared_ptr<T> >
    : public forwarding_adapter<Traits, T, shared_ptr<T> > {

    adapter(shared_ptr<T> const& adapted) : adapted_(adapted) {}
    shared_ptr<T> const adapted_;

    template <class A> A forward() const { return A(boost::cref(*adapted_)); }
    // T&   get()   const { return *adapted_; }
    bool valid() const { return adapted_; }
};


}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_ADAPTERS_SHARED_PTR_HPP_INCLUDED

