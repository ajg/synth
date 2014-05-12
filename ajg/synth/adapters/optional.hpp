//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_OPTIONAL_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_OPTIONAL_HPP_INCLUDED

#include <boost/optional/optional_fwd.hpp>

#include <ajg/synth/adapters/forwarding_adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for boost::optional
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, boost::optional<T> > : forwarding_adapter<Behavior, T, boost::optional<T> > {
    adapter(boost::optional<T> const& adapted) : forwarding_adapter<Behavior, T, boost::optional<T> >(adapted) {}

    typename Behavior::boolean_type valid() const { return this->adapted_; }
    template <class A> A forward() const { return A(boost::ref(*this->adapted_)); }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_OPTIONAL_HPP_INCLUDED

