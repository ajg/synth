//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_OPTIONAL_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_OPTIONAL_HPP_INCLUDED

#include <boost/optional/optional_fwd.hpp>

#include <ajg/synth/adapters/forwarding_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for boost::optional
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class T>
struct adapter<Value, boost::optional<T> >     : forwarding_adapter<Value, T, boost::optional<T> > {
    adapter(boost::optional<T> const& adapted) : forwarding_adapter<Value, T, boost::optional<T> >(adapted) {}

    template <class A> A forward() { return A(boost::ref(*this->adapted())); }
    template <class A> A forward() const { return A(boost::cref(*this->adapted())); }
    bool                 valid() const { return this->adapted(); }
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_OPTIONAL_HPP_INCLUDED

