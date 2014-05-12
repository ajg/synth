//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_REF_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_REF_HPP_INCLUDED

#include <boost/ref.hpp>
#include <boost/type_traits/remove_const.hpp>

#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/forwarding_adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for boost::reference_wrapper
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, boost::reference_wrapper<T> > : public base_adapter<Behavior> {
  public:

    // AJG_SYNTH_ADAPTER(boost::reference_wrapper<T>)

    AJG_SYNTH_ADAPTER_TYPEDEFS(boost::reference_wrapper<T>);

  public:

    // /*const*/ adapted_type adapted_;

    adapter(adapted_type const& adapted) : adapted_(adapted) {}

    template <class U>
    adapter(adapted_type const& adapted, const U& u) : adapted_(adapted, u) {}

    template <class U, class V>
    adapter(adapted_type const& adapted, const U& u, const V& v) : adapted_(adapted, u, v) {}

  protected:

    typedef adapter<Behavior, typename boost::remove_const<T>::type> wrapped_adapter_type;

    virtual boolean_type equal_adapted(adapter_type const& that) const {
        return adapted_.template equal_as<wrapped_adapter_type>(that);
    }

    virtual boolean_type less_adapted(adapter_type const& that) const {
        return adapted_.template less_as<wrapped_adapter_type>(that);
    }

  public:

    boolean_type equal(adapter_type const& that) const { return adapted_.equal(that); }
    boolean_type to_boolean() const { return adapted_.to_boolean(); }
    floating_type to_floating()  const { return adapted_.to_floating(); }
    // integer_type to_integer() const { return adapted_.to_integer(); }
    // string_type  to_string()  const { return adapted_.to_string(); }
    void input (istream_type& in)        { adapted_.input(in); }
    void output(ostream_type& out) const { adapted_.output(out); }
    std::type_info const& type() const { return typeid(T); }

  private:

    wrapped_adapter_type adapted_;
};



/*
template <class Behavior, class T>
struct adapter<Behavior, reference_wrapper<T> >
    : public forwarding_adapter<Behavior, T, reference_wrapper<T> > {

    adapter(reference_wrapper<T const> const& adapted) : adapted_(adapted) {}
    adapter<Behavior, T> adapted_;
    // adapter<Behavior, typename remove_const<T>::type> adapted_;
    // T const& adapted_;

    bool   valid() const { return true; }
    // T const& get() const { return adapted_; }
};*/



}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_REF_HPP_INCLUDED

