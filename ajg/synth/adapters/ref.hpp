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

/*
template <class Behavior, class T>
struct adapter<Behavior, boost::reference_wrapper<T> >  : forwarding_adapter<Behavior, T, boost::reference_wrapper<T> > {
    adapter(boost::reference_wrapper<T> const& adapted) : forwarding_adapter<Behavior, T, boost::reference_wrapper<T> >(adapted) {}
    template <class A> A forward() const { ? }
    boolean_type valid() const { return true; }
};
*/

template <class Behavior, class T>
struct adapter<Behavior, boost::reference_wrapper<T> > : base_adapter<Behavior> { // TODO: Use concrete_adapter.
  public:

    adapter(boost::reference_wrapper<T> const& adapted) : adapted_(adapted) {}

    template <class U>
    adapter(boost::reference_wrapper<T> const& adapted, const U& u) : adapted_(adapted, u) {}

    template <class U, class V>
    adapter(boost::reference_wrapper<T> const& adapted, const U& u, const V& v) : adapted_(adapted, u, v) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);

  protected:

    typedef adapter<Behavior, typename boost::remove_const<T>::type> wrapped_adapter_type;

    virtual boolean_type equal_adapted(adapter_type const& that) const {
        return adapted_.template equal_as<wrapped_adapter_type>(that);
    }

    virtual boolean_type less_adapted(adapter_type const& that) const {
        return adapted_.template less_as<wrapped_adapter_type>(that);
    }

  public:

    std::type_info const& type()   const { return typeid(T); }

    boolean_type  to_boolean()   const { return adapted_.to_boolean(); }
    floating_type to_floating()  const { return adapted_.to_floating(); }
    range_type    to_range()     const { return adapted_.to_range(); }
    // integer_type to_integer() const { return adapted_.to_integer(); }
    // string_type  to_string()  const { return adapted_.to_string(); }

    void input (istream_type& in)        { adapted_.input(in); }
    void output(ostream_type& out) const { adapted_.output(out); }

  private:

    wrapped_adapter_type adapted_;
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_REF_HPP_INCLUDED

