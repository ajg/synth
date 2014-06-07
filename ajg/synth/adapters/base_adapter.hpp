//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_BASE_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_BASE_ADAPTER_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <string>
#include <istream>
#include <ostream>
#include <typeinfo>
#include <stdexcept>
#include <functional>

#include <boost/optional.hpp>

#include <ajg/synth/exceptions.hpp>

namespace ajg {
namespace synth {
namespace adapters {

// TODO[c++11]: ensure the underlying type is wide enough.
enum type_flags
    { unspecified
    , unit          = 1 << 0
    , boolean       = 1 << 1
    , textual       = 1 << 2
    ,   character   = 1 << 3
    , numeric       = 1 << 4
    ,   floating    = 1 << 5
    ,   integral    = 1 << 6
 // ,   signed      = 1 << 7
 // ,   unsigned    = 1 << 8
    , chronologic   = 1 << 9
    , sequential    = 1 << 10
    , associative   = 1 << 11
    , container     = 1 << 12
    };

template <class Value, class Adapted>
struct adapter;

template <class Value, class T, class Adapted, class Specialized>
struct forwarding_adapter;

using boost::optional;

//
// base_adapter
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct base_adapter {
  public:

    typedef Value                                                               value_type;
    typedef base_adapter                                                        base_type;

    typedef typename value_type::iterator                                       iterator;
    typedef typename value_type::const_iterator                                 const_iterator;
    typedef typename value_type::range_type                                     range_type;
    typedef typename value_type::attribute_type                                 attribute_type;
    typedef typename value_type::attributes_type                                attributes_type;
    typedef typename value_type::adapter_type                                   adapter_type;
    typedef typename value_type::traits_type                                    traits_type;

    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::integer_type                                  integer_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;


  public:

    inline base_adapter() {}
    virtual ~base_adapter() {}

  public:

    virtual std::type_info const& type()  const = 0;
    virtual type_flags            flags() const = 0;

    virtual optional<boolean_type>  get_boolean()  const = 0;
    virtual optional<number_type>   get_number()   const = 0;
    virtual optional<datetime_type> get_datetime() const = 0;
    virtual optional<string_type>   get_string()   const = 0;
    virtual optional<range_type>    get_range()    const = 0;

    virtual boolean_type input (istream_type& istream) const = 0;
    virtual boolean_type output(ostream_type& ostream) const = 0;

    virtual attribute_type  attribute(value_type const& key)                                  const = 0;
    virtual void            attribute(value_type const& key, attribute_type const& attribute) const = 0;
    virtual attributes_type attributes()                                                      const = 0;

    virtual const_iterator find(value_type const& value) const = 0;

    virtual boolean_type equal_adapted(adapter_type const& that) const = 0;
    virtual boolean_type less_adapted (adapter_type const& that) const = 0;

    // TODO: Move this to concrete_adapter.

    template <class T> // TODO: Deal with forwarding_adapters.
    inline T& get_adapted() const {
        typedef adapters::adapter<Value, T> specialized_type;
        specialized_type const* const specialization = this->template get<specialized_type>();
        BOOST_ASSERT(specialization);
        return specialization->adapted();
    }

  protected:

    template <class Adapter> // TODO: Deal with forwarding_adapters.
    inline Adapter const* get() const { return dynamic_cast<Adapter const*>(this); }

    template <class Adapter>
    inline boolean_type equal_as(adapter_type const& that) const {
        Adapter const* const this_ = this->template get<Adapter>();
        Adapter const* const that_ = that->template get<Adapter>();
        return this_ != 0 && that_ != 0 && std::equal_to<typename Adapter::bare_adapted_type>()(this_->adapted_, that_->adapted_);
    }

    template <class Adapter>
    inline boolean_type less_as(adapter_type const& that) const {
        Adapter const* const this_ = this->template get<Adapter>();
        Adapter const* const that_ = that->template get<Adapter>();
        return this_ != 0 && that_ != 0 && std::less<typename Adapter::bare_adapted_type>()(this_->adapted_, that_->adapted_);
    }

  private:

    template <class T, class A>                    friend struct adapters::adapter;
    template <class B, class T, class A, class S>  friend struct adapters::forwarding_adapter;
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_BASE_ADAPTER_HPP_INCLUDED

