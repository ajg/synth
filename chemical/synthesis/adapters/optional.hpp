
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ADAPTERS_OPTIONAL_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ADAPTERS_OPTIONAL_HPP_INCLUDED

#include <boost/optional/optional_fwd.hpp>

#include <chemical/synthesis/adapters/adapter.hpp>

namespace chemical {
namespace synthesis {

//
// specialization for boost::optional
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, optional<T> >
    : public forwarding_adapter<Traits, T, optional<T> > {

    adapter(optional<T> const& adapted) : adapted_(adapted) {}
    optional<T> const adapted_;

    bool   valid() const { return adapted_; }
    //T const& get() const { return *adapted_; }
    template <class A> A forward() const { return A(ref(*adapted_)); }
};

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_ADAPTERS_OPTIONAL_HPP_INCLUDED

