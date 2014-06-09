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
namespace adapters {

//
// specialization for boost::reference_wrapper
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
TODO:
template <class Value, class T>
struct adapter<Value, boost::reference_wrapper<T> >     : forwarding_adapter<Value, T, boost::reference_wrapper<T> > {
    adapter(boost::reference_wrapper<T> const& adapted) : forwarding_adapter<Value, T, boost::reference_wrapper<T> >(adapted) {}
    template <class A> A forward() const { ? }
    boolean_type valid() const { return true; }
};
*/

template <class Value, class T>
struct adapter<Value, boost::reference_wrapper<T> > : base_adapter<Value> { // TODO: Use concrete_adapter or forwarding_adapter.
  public:

    adapter(boost::reference_wrapper<T> const& adapted) : adapted_(adapted) {}

    template <class U>
    adapter(boost::reference_wrapper<T> const& adapted, const U& u) : adapted_(adapted, u) {}

    template <class U, class V>
    adapter(boost::reference_wrapper<T> const& adapted, const U& u, const V& v) : adapted_(adapted, u, v) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Value);

  protected:

    typedef adapter<Value, typename boost::remove_const<T>::type> wrapped_adapter_type;

    virtual boolean_type equal_adapted(adapter_type const& that) const {
        return this->adapted_.template equal_as<wrapped_adapter_type>(that);
    }

    virtual boolean_type less_adapted(adapter_type const& that) const {
        return this->adapted_.template less_as<wrapped_adapter_type>(that);
    }

  public:

    virtual std::type_info const& type()  const { return typeid(T); } // XXX: this->adapted_.type(); ?
    virtual type_flags            flags() const { return this->adapted_.flags(); }

    virtual optional<boolean_type>  get_boolean()  const { return this->adapted_.get_boolean(); }
    virtual optional<number_type>   get_number()   const { return this->adapted_.get_number(); }
    virtual optional<datetime_type> get_datetime() const { return this->adapted_.get_datetime(); }
    virtual optional<string_type>   get_string()   const { return this->adapted_.get_string(); }
    virtual optional<range_type>    get_range()    const { return this->adapted_.get_range(); }

    virtual boolean_type input (istream_type& istream) const { return this->adapted_.input(istream); }
    virtual boolean_type output(ostream_type& ostream) const { return this->adapted_.output(ostream); }

    virtual attribute_type  attribute(value_type const& key) const { return this->adapted_.attribute(key); }
    virtual void            attribute(value_type const& key, attribute_type const& attribute) const { this->adapted_.attribute(key, attribute); }
    virtual attributes_type attributes() const { return this->adapted_.attributes(); }

    virtual const_iterator find(value_type const& value) const { return this->adapted_.find(value); }

  private:

    wrapped_adapter_type adapted_;
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_REF_HPP_INCLUDED

