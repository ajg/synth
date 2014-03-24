//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_OPTIONAL_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_OPTIONAL_HPP_INCLUDED

#include <boost/optional/optional_fwd.hpp>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for boost::optional
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, optional<T> >
    : public forwarding_adapter<Traits, T, optional<T> > {

    adapter(optional<T> const& adapted) : adapted_(adapted) {}
    optional<T> adapted_;

    bool   valid() const { return adapted_; }
    //T const& get() const { return *adapted_; }
    template <class A> A forward() const { return A(ref(*adapted_)); }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_OPTIONAL_HPP_INCLUDED

