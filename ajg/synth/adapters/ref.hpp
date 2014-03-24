//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_REF_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_REF_HPP_INCLUDED

#include <boost/ref.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for boost::reference_wrapper
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, reference_wrapper<T> >
    : public abstract_adapter<Traits> {

    // AJG_SYNTH_ADAPTER(reference_wrapper<T>)

    AJG_SYNTH_ADAPTER_TYPEDEFS(reference_wrapper<T>, adapter);
    // /*const*/ adapted_type adapted_;

    adapter(adapted_type const& adapted) : adapted_(adapted) {}

    template <class U>
    adapter(adapted_type const& adapted, const U& u) : adapted_(adapted, u) {}

    template <class U, class V>
    adapter(adapted_type const& adapted, const U& u, const V& v) : adapted_(adapted, u, v) {}

  public:

    boolean_type equal(abstract_type const& that) const { return adapted_.equal(that); }
    number_type  count() const { return adapted_.count(); }
    boolean_type test()  const { return adapted_.test(); }
    void input (istream_type& in)        { adapted_.input(in); }
    void output(ostream_type& out) const { adapted_.output(out); }
    std::type_info const& type() const { return typeid(T); }


    /*integer_type to_integer() const { return adapted_.to_integer(); }
    boolean_type operator !() const { return adapted_.operator !(); }
    string_type  to_string()  const { return adapted_.to_string(); }*/

  private:

    // adapted_type const& adapted_;
    adapter<Traits, typename remove_const<T>::type> adapted_;
};



/*
template <class Traits, class T>
struct adapter<Traits, reference_wrapper<T> >
    : public forwarding_adapter<Traits, T, reference_wrapper<T> > {

    adapter(reference_wrapper<T const> const& adapted) : adapted_(adapted) {}
    adapter<Traits, T> adapted_;
    // adapter<Traits, typename remove_const<T>::type> adapted_;
    // T const& adapted_;

    bool   valid() const { return true; }
    T const& get() const { return adapted_; }
};*/



}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_REF_HPP_INCLUDED

