//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED

#include <boost/type_traits/has_left_shift.hpp>
#include <boost/type_traits/has_right_shift.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/base_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {


template <class Value, class Adapted, type_flags Flags = unspecified/*, class Holder = Adapted*/, class Specialized = adapter<Value, Adapted> >
struct concrete_adapter_without_io : base_adapter<Value> {
    concrete_adapter_without_io(typename boost::remove_reference<Adapted>::type const& adapted) : adapted_(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Value);

    typedef typename boost::remove_reference<Adapted>::type bare_adapted_type;

  public:

    virtual std::type_info const& type()  const { return typeid(Adapted); }
    virtual type_flags            flags() const { return Flags; }

    virtual optional<boolean_type>  get_boolean()  const { return boost::none; }
    virtual optional<number_type>   get_number()   const { return boost::none; }
    virtual optional<datetime_type> get_datetime() const { return boost::none; }
    virtual optional<string_type>   get_string()   const { return boost::none; }
    virtual optional<range_type>    get_range()    const { return boost::none; }

    virtual attribute_type  attribute(value_type const& key)                                  const { return boost::none; }
    virtual void            attribute(value_type const& key, attribute_type const& attribute) const {} // TODO: return false;
    virtual attributes_type attributes()                                                      const { return attributes_type(); }

    virtual const_iterator find(value_type const& value) const { return const_iterator(); }

  protected:

    virtual boolean_type equal_adapted(adapter_type const& that) const { return this->template equal_as<Specialized>(that); }
    virtual boolean_type less_adapted(adapter_type const& that) const { return this->template less_as<Specialized>(that); }

  protected:

    // inline bare_adapted_type&       adapted()       { return this->adapted_; }
    // inline bare_adapted_type const& adapted() const { return this->adapted_; }

    inline bare_adapted_type& adapted() const { return const_cast<bare_adapted_type&>(this->adapted_); }

  private:

    friend struct base_adapter<value_type>;

  private:

    /*mutable*/ Adapted adapted_;
};

template <class Value, class Adapted, type_flags Flags = unspecified/*, class Holder = Adapted*/, class Specialized = adapter<Value, Adapted> >
struct concrete_adapter                                                              : concrete_adapter_without_io<Value, Adapted, Flags, Specialized> {
    concrete_adapter(typename boost::remove_reference<Adapted>::type const& adapted) : concrete_adapter_without_io<Value, Adapted, Flags, Specialized>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Value);

  public:

    virtual boolean_type input (istream_type& istream) const { return input_adapted(istream, this->adapted()); }
    virtual boolean_type output(ostream_type& ostream) const { return output_adapted(ostream, this->adapted()); }

  private:

    // FIXME: Types that are implicitly convertible to e.g. bool fail here.

    template <class A>
    struct has_useful_input_operator {
        BOOST_STATIC_CONSTANT(bool, value =
          (boost::has_right_shift<istream_type, A>::value) &&
          !boost::is_pointer<A>::value &&
          !boost::is_array<A>::value);
    };

    template <class A>
    struct has_useful_output_operator {
        BOOST_STATIC_CONSTANT(bool, value =
          (boost::has_left_shift<ostream_type, A>::value) &&
          !boost::is_pointer<A>::value &&
          !boost::is_array<A>::value);
    };


    template <class A> boolean_type input_adapted(istream_type& istream, A& a, typename boost::disable_if<has_useful_input_operator<A> >::type* = 0) const { return false; }
    template <class A> boolean_type input_adapted(istream_type& istream, A& a, typename boost::enable_if<has_useful_input_operator<A> >::type* = 0) const { (void)(istream >> a); return true; }

    template <class A> boolean_type output_adapted(ostream_type& ostream, A const& a, typename boost::disable_if<has_useful_output_operator<A> >::type* = 0) const { return false; }
    template <class A> boolean_type output_adapted(ostream_type& ostream, A const& a, typename boost::enable_if<has_useful_output_operator<A> >::type* = 0) const { (void)(ostream << a); return true; }
};


}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_CONCRETE_ADAPTER_HPP_INCLUDED

