//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_FORWARDING_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_FORWARDING_ADAPTER_HPP_INCLUDED

#include <boost/type_traits/remove_reference.hpp>

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

template <class Value, class T, class Adapted, class Specialized = adapter<Value, Adapted> >
struct forwarding_adapter                      : concrete_adapter<Value, Adapted, unspecified, Specialized> {
    forwarding_adapter(Adapted const& adapted) : concrete_adapter<Value, Adapted, unspecified, Specialized>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Value);

    virtual std::type_info const& type()  const { return this->valid() ? this->forward().type()  : typeid(void); }
    virtual type_flags            flags() const { return this->valid() ? this->forward().flags() : unspecified; }

    virtual optional<boolean_type>  get_boolean()  const { return this->valid() ? this->forward().get_boolean()  : boost::none; }
    virtual optional<number_type>   get_number()   const { return this->valid() ? this->forward().get_number()   : boost::none; }
    virtual optional<datetime_type> get_datetime() const { return this->valid() ? this->forward().get_datetime() : boost::none; }
    virtual optional<string_type>   get_string()   const { return this->valid() ? this->forward().get_string()   : boost::none; }
    virtual optional<range_type>    get_range()    const { return this->valid() ? this->forward().get_range()    : boost::none; }

    virtual attribute_type  attribute(value_type const& key) const { return this->valid() ? this->forward().attribute(key) : attribute_type(); }
    virtual void            attribute(value_type const& key, attribute_type const& attribute) const { if (this->valid()) this->forward().attribute(key, attribute); }
    virtual attributes_type attributes() const { return this->valid() ? this->forward().attributes() : attributes_type(); }

    virtual const_iterator find(value_type const& value) const { return this->valid() ? this->forward().find(value) : const_iterator(); }

    virtual boolean_type input (istream_type& istream) const { return this->valid() ? this->forward().input(istream) : false; }
    virtual boolean_type output(ostream_type& ostream) const { return this->valid() ? this->forward().output(ostream) : false; }

  protected:

    virtual boolean_type equal_adapted(adapter_type const& that) const {
        // TODO: Check all cases:
        //     a. type(*that) == type(*this)
        //     b. type(that->adapted) == type(this->adapted)
        return this->forward().template equal_as<adapter<Value, T> >(that);
    }

    virtual boolean_type less_adapted(adapter_type const& that) const {
        return this->forward().template less_as<adapter<Value, T> >(that);
    }

  private:

    typedef typename boost::remove_reference<T>::type                        bare_type;
    typedef adapter<Value, boost::reference_wrapper<T> >                     ref_type;
    typedef adapter<Value, boost::reference_wrapper<bare_type const> >       cref_type;

  private:

    inline ref_type forward() { return static_cast<Specialized*>(this)->template forward<ref_type>(); }
    inline cref_type forward() const { return static_cast<Specialized const*>(this)->template forward<cref_type>(); }
    inline boolean_type valid() const { return static_cast<Specialized const*>(this)->valid(); }
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_FORWARDING_ADAPTER_HPP_INCLUDED

