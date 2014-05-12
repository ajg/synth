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

#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include <ajg/synth/exceptions.hpp>

namespace ajg {
namespace synth {

using boost::optional;

template <class Behavior, class Adapted>
struct adapter;

//
// base_adapter
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior>
struct base_adapter {
  public:

    typedef Behavior                                                            behavior_type;
    typedef base_adapter                                                        base_type;
    typedef typename behavior_type::traits_type                                 traits_type;
    typedef typename behavior_type::value_type                                  value_type;
    typedef typename behavior_type::adapter_type                                adapter_type;

    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef typename value_type::iterator                                       iterator;
    typedef typename value_type::const_iterator                                 const_iterator;
    typedef typename value_type::range_type                                     range_type;

  public:

    inline base_adapter() {}
    virtual ~base_adapter() {}

  public:

    // TODO: Instead of throwing invalid_method here, defer default behavior to Traits.

    virtual std::type_info const& type() const = 0;

    virtual boolean_type  is_numeric()  const { return false; }

    virtual floating_type to_floating()   const { AJG_SYNTH_THROW(invalid_method("to_floating")); }
    virtual boolean_type  to_boolean()  const { AJG_SYNTH_THROW(invalid_method("to_boolean")); }
    virtual datetime_type to_datetime() const { AJG_SYNTH_THROW(invalid_method("to_datetime")); }
    virtual string_type   to_string()   const { return behavior_type::to_string(*this); }

    virtual void input (istream_type& in)        { AJG_SYNTH_THROW(invalid_method("input")); }
    virtual void output(ostream_type& out) const { AJG_SYNTH_THROW(invalid_method("output")); }

    // TODO: Refactor these into a single to_range function.
    virtual const_iterator begin() const { AJG_SYNTH_THROW(invalid_method("begin")); }
    virtual const_iterator end()   const { AJG_SYNTH_THROW(invalid_method("end")); }

    virtual const_iterator       find(value_type const& value) const { AJG_SYNTH_THROW(invalid_method("find")); }
    virtual optional<value_type> index(value_type const& what) const { AJG_SYNTH_THROW(invalid_method("index")); }

  private:

    template <class T, class A>                    friend struct synth::adapter;
    template <class B, class T, class A, class S>  friend struct forwarding_adapter;
    template <class T, template <class> class V>   friend struct value_facade;

    template <class Adapter> // TODO: Deal with forwarding_adapters.
    inline Adapter const* get() const { return dynamic_cast<Adapter const*>(this); }

    template <class T> // TODO: Deal with forwarding_adapters.
    inline T const& get_adapted() const {
        typedef synth::adapter<Behavior, T> concrete_adapter_type;
        concrete_adapter_type const* const concrete_adapter = this->template get<concrete_adapter_type>();
        BOOST_ASSERT(concrete_adapter);
        return concrete_adapter->adapted_;
    }

  protected:

    virtual boolean_type equal_adapted(adapter_type const& that) const = 0;
    virtual boolean_type less_adapted (adapter_type const& that) const = 0;

    template <class Adapter>
    inline boolean_type equal_as(adapter_type const& that) const {
        Adapter const* const this_ = this->template get<Adapter>();
        Adapter const* const that_ = that->template get<Adapter>();
        return this_ != 0 && that_ != 0 && std::equal_to<typename Adapter::adapted_type>()(this_->adapted_, that_->adapted_);
    }

    template <class Adapter>
    inline boolean_type less_as(adapter_type const& that) const {
        Adapter const* const this_ = this->template get<Adapter>();
        Adapter const* const that_ = that->template get<Adapter>();
        return this_ != 0 && that_ != 0 && std::less<typename Adapter::adapted_type>()(this_->adapted_, that_->adapted_);
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_BASE_ADAPTER_HPP_INCLUDED

