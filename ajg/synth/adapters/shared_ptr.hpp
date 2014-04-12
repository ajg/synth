//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_SHARED_PTR_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_SHARED_PTR_HPP_INCLUDED

#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/forwarding_adapter.hpp>
#include <ajg/synth/adapters/detail/smart_ptr_fwd.hpp>

namespace ajg {
namespace synth {

//
// specialization for boost::shared_ptr
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, shared_ptr<T> >
    : public forwarding_adapter<Behavior, T, shared_ptr<T> > {

    adapter(shared_ptr<T> const& adapted) : adapted_(adapted) {}
    shared_ptr<T> adapted_;

    template <class A> A forward() const { return A(boost::cref(*adapted_)); }
    // T&   get()   const { return *adapted_; }
    bool valid() const { return bool(adapted_); }
};


}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_SHARED_PTR_HPP_INCLUDED

