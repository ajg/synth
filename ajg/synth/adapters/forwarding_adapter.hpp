//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_FORWARDING_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_FORWARDING_ADAPTER_HPP_INCLUDED

#include <boost/type_traits/remove_reference.hpp>

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {

template <class Behavior, class T, class Adapted, class Sub = adapter<Behavior, Adapted> >
struct forwarding_adapter : concrete_adapter<Behavior, Adapted/*, Sub?*/> {
    forwarding_adapter(Adapted const& adapted) : concrete_adapter<Behavior, Adapted>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);

    const_iterator begin() const { return valid() ? forward().begin() : const_iterator(); }
    const_iterator end()   const { return valid() ? forward().end()   : const_iterator(); }

    floating_type to_floating()  const { return valid() ? forward().to_floating() : floating_type(0); }
    boolean_type  to_boolean() const { return valid() ? forward().to_boolean() : boolean_type(false); }

    boolean_type equal(adapter_type const& that) const {
        return valid() ? forward().equal(that) : boolean_type(false);
    }

    void input (istream_type& in)        { if (valid()) forward().input(in); }
    void output(ostream_type& out) const { if (valid()) forward().output(out); }

    std::type_info const& type() const { return forward().type(); }

  protected:

    virtual boolean_type equal_adapted(adapter_type const& that) const {
        // TODO: Check all cases:
        //     a. type(*that) == type(*this)
        //     b. type(that->adapted) == type(this->adapted)
        return forward().template equal_as<adapter<Behavior, T> >(that);
    }

    virtual boolean_type less_adapted(adapter_type const& that) const {
        return forward().template less_as<adapter<Behavior, T> >(that);
    }

  private:

    typedef typename boost::remove_reference<T>::type                           bare_type;
    typedef adapter<Behavior, boost::reference_wrapper<T> >                     ref_type;
    typedef adapter<Behavior, boost::reference_wrapper<bare_type const> >       cref_type;

  private:

    inline cref_type forward() const {
        return static_cast<Sub const*>(this)->template forward<cref_type>();
    }

    inline boolean_type valid() const { return static_cast<Sub const*>(this)->valid(); }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_FORWARDING_ADAPTER_HPP_INCLUDED

