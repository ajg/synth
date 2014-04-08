//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_FORWARDING_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_FORWARDING_ADAPTER_HPP_INCLUDED

#include <ajg/synth/adapters/base_adapter.hpp>

namespace ajg {
namespace synth {

// TODO: Move to separate file.
template <class Traits, class T, class Adapted, class Derived = adapter<Traits, Adapted> >
struct forwarding_adapter : public base_adapter<Traits> {

    AJG_SYNTH_ADAPTER_TYPEDEFS(Adapted, forwarding_adapter);

  protected:

    forwarding_adapter() {}
    ~forwarding_adapter() {}

  protected:

    virtual boolean_type equal_adapted(base_type const& that) const {
        return forward().template equal_as<adapter<Traits, T> >(that);
    }

    virtual boolean_type less_adapted(base_type const& that) const {
        return forward().template less_as<adapter<Traits, T> >(that);
    }

  public:

    const_iterator begin() const { return valid() ? forward().begin() : const_iterator(); }
    const_iterator end()   const { return valid() ? forward().end()   : const_iterator(); }

    number_type  to_number()  const { return valid() ? forward().to_number()  : number_type(); }
    boolean_type to_boolean() const { return valid() ? forward().to_boolean() : boolean_type(); }

    boolean_type equal(base_type const& that) const {
        return valid() ? forward().equal(that) : boolean_type();
    }

    void input (istream_type& in)        { if (valid()) forward().input(in); }
    void output(ostream_type& out) const { if (valid()) forward().output(out); }

    std::type_info const& type() const { return forward().type(); }

  private:

    typedef typename boost::remove_reference<T>::type               T_noref;
    typedef adapter<Traits, reference_wrapper<T> >                  ref_type;
    typedef adapter<Traits, reference_wrapper<T_noref const> >      cref_type;

  private:

    inline cref_type forward() const {
        return static_cast<Derived const*>(this)->template forward<cref_type>();
    }

    // inline T&       get()         { return static_cast<Derived const*>(this)->get(); }
    // inline T const& get()   const { return static_cast<Derived const*>(this)->get(); }
    inline bool     valid() const { return static_cast<Derived const*>(this)->valid(); }

};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_FORWARDING_ADAPTER_HPP_INCLUDED

