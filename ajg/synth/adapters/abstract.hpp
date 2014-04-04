//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_ABSTRACT_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_ABSTRACT_HPP_INCLUDED

#include <string>
#include <istream>
#include <ostream>
#include <typeinfo>
#include <stdexcept>
#include <functional>

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/throw_exception.hpp>

#include <ajg/synth/detail.hpp>

namespace ajg {
namespace synth {

using boost::optional;
using boost::throw_exception;

template <class Traits, class Adapted>
struct adapter;

template <class Traits>
struct abstract_numeric_adapter;

//
// bad_method exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct bad_method : public std::invalid_argument {
    std::string const name;

    bad_method(std::string const& name)
        : std::invalid_argument("bad method `" + name + "'")
        , name(name) {}

    ~bad_method() throw () {}
};

//
// abstract_adapter
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits>
struct abstract_adapter {
  public:

    typedef abstract_adapter                        abstract_type;
    typedef Traits                                  traits_type;

    typedef typename traits_type::char_type         char_type;
    typedef typename traits_type::size_type         size_type;
    typedef typename traits_type::value_type        value_type;
    typedef typename traits_type::boolean_type      boolean_type;
    typedef typename traits_type::number_type       number_type;
    typedef typename traits_type::string_type       string_type;
    typedef typename traits_type::datetime_type     datetime_type;
    typedef typename traits_type::duration_type     duration_type;
    typedef typename traits_type::istream_type      istream_type;
    typedef typename traits_type::ostream_type      ostream_type;

    typedef typename traits_type::iterator          iterator;
    typedef typename traits_type::const_iterator    const_iterator;

 // typedef value_iterator<value_type const>        iterator;
 // typedef value_iterator<value_type const>        const_iterator;

  public:

    // TODO: Instead of throwing bad_method here, defer default behavior to Traits.

    virtual std::type_info const& type() const = 0;

    // TODO: Refactor these into a single to_range function.
    virtual const_iterator begin() const { AJG_SYNTH_THROW(bad_method("begin")); }
    virtual const_iterator end()   const { AJG_SYNTH_THROW(bad_method("end")); }

    virtual number_type   count()       const { AJG_SYNTH_THROW(bad_method("count")); } // TODO: Rename to_number
    virtual boolean_type  test()        const { AJG_SYNTH_THROW(bad_method("test")); }  // TODO: Rename to_boolean
    virtual datetime_type to_datetime() const { AJG_SYNTH_THROW(bad_method("to_datetime")); }
    virtual string_type   to_string()   const { return traits_type::adapter_traits::to_string(*this); }

    virtual void input (istream_type& in)        { AJG_SYNTH_THROW(bad_method("input")); }
    virtual void output(ostream_type& out) const { AJG_SYNTH_THROW(bad_method("output")); }

    virtual const_iterator       find(value_type const& value) const { AJG_SYNTH_THROW(bad_method("find")); }
    virtual optional<value_type> index(value_type const& what) const { AJG_SYNTH_THROW(bad_method("index")); }

    virtual ~abstract_adapter() {}

  private:

    template <class T, class A>                   friend struct synth::adapter;
    template <class T, class _, class A, class D> friend struct forwarding_adapter;
    template <class T>                            friend struct abstract_numeric_adapter;
    template <class C, class V, class T>          friend struct value_facade;

    template <class Adapter> // TODO: Deal with forwarding_adapters.
    inline Adapter const* get() const { return dynamic_cast<Adapter const*>(this); }

    template <class T> // TODO: Deal with forwarding_adapters.
    inline T const& get_adapted() const {
        typedef synth::adapter<Traits, T> concrete_adapter_type;
        concrete_adapter_type const* const concrete_adapter = this->template get<concrete_adapter_type>();
        BOOST_ASSERT(concrete_adapter);
        return concrete_adapter->adapted_;
    }

  protected:

    virtual boolean_type equal_adapted(abstract_type const& that) const = 0;
    virtual boolean_type less_adapted (abstract_type const& that) const = 0;

    template <class Adapter>
    inline boolean_type equal_as(abstract_type const& that) const {
        Adapter const* const this_ = this->template get<Adapter>();
        Adapter const* const that_ = that.template get<Adapter>();
        return this_ != 0 && that_ != 0 && std::equal_to<typename Adapter::adapted_type>()(this_->adapted_, that_->adapted_);
    }

    template <class Adapter>
    inline boolean_type less_as(abstract_type const& that) const {
        Adapter const* const this_ = this->template get<Adapter>();
        Adapter const* const that_ = that.template get<Adapter>();
        return this_ != 0 && that_ != 0 && std::less<typename Adapter::adapted_type>()(this_->adapted_, that_->adapted_);
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_ABSTRACT_HPP_INCLUDED

