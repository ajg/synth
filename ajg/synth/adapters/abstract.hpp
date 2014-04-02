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
struct abstract_adapter { // TODO: Rename to virtual_adapter
  public:

    typedef abstract_adapter this_type;
    typedef abstract_adapter abstract_type;
    typedef Traits           traits_type;

    typedef typename traits_type::char_type     char_type;
    typedef typename traits_type::size_type     size_type;
    typedef typename traits_type::value_type    value_type;
    typedef typename traits_type::boolean_type  boolean_type;
    typedef typename traits_type::number_type   number_type;
    typedef typename traits_type::string_type   string_type;
    typedef typename traits_type::datetime_type datetime_type;
    typedef typename traits_type::duration_type duration_type;
    typedef typename traits_type::istream_type  istream_type;
    typedef typename traits_type::ostream_type  ostream_type;

    typedef typename traits_type::iterator       iterator;
    typedef typename traits_type::const_iterator const_iterator;

    //typedef value_iterator<value_type const> iterator;
    //typedef value_iterator<value_type const> const_iterator;

  public:

    virtual const_iterator begin() const { AJG_SYNTH_THROW(bad_method("begin")); }
    virtual const_iterator end()   const { AJG_SYNTH_THROW(bad_method("end")); }

    virtual number_type   count() const { AJG_SYNTH_THROW(bad_method("count")); } // TODO: Rename to_number
    virtual boolean_type  test()  const { AJG_SYNTH_THROW(bad_method("test")); }  // TODO: Rename to_boolean
    virtual datetime_type to_datetime()  const { AJG_SYNTH_THROW(bad_method("to_datetime")); }

    // TODO: Make virtual?
    inline string_type to_string() const {
        std::basic_ostringstream<char_type> stream;
        this->output(stream);
        return stream.str();
    }

    // TODO: concrete | abstract
    //       bool     | boolean
    //       char     | character
    //       number   | numeric
    //       string   | ?
    //       datetime | chronological
    //       ?        | sequential
    //       ?        | mapping/indexed/associative
    //

    virtual boolean_type equal(abstract_type const& that) const {
        if (this->shares_type_with(that))                  return this->equal_adapted(that);
        else if (this->is_boolean() && that.is_boolean())  return this->test()      == that.test();
        else if (this->is_numeric() && that.is_numeric())  return this->count()     == that.count();
        else if (this->is_string()  && that.is_string())   return this->to_string() == that.to_string();
        // TODO: Compare sequences, etc.
        else return false;
    }

    virtual boolean_type less(abstract_type const& that) const {
        if (this->shares_type_with(that))                  return this->less_adapted(that);
        else if (this->is_boolean() && that.is_boolean())  return this->test()      == that.test();
        else if (this->is_numeric() && that.is_numeric())  return this->count()     == that.count();
        else if (this->is_string()  && that.is_string())   return this->to_string() == that.to_string();
        // TODO: Compare sequences, etc.
        else return false;
    }

    virtual void input (istream_type& in)        { AJG_SYNTH_THROW(bad_method("input")); }
    virtual void output(ostream_type& out) const { AJG_SYNTH_THROW(bad_method("output")); }

    virtual std::type_info const& type() const { AJG_SYNTH_THROW(bad_method("type")); }

    virtual const_iterator find(value_type const& value) const {
        AJG_SYNTH_THROW(bad_method("find"));
    }

    virtual optional<value_type> index(value_type const& what) const {
        AJG_SYNTH_THROW(bad_method("index"));
    }

    virtual boolean_type is_numeric() const {
        typedef abstract_numeric_adapter<traits_type> numeric_adapter;
        return dynamic_cast<numeric_adapter const*>(this) != 0;
    }

    virtual boolean_type is_string() const {
        return is<string_type>();
    }

    virtual boolean_type is_boolean() const {
        return is<boolean_type>();
    }

    virtual ~abstract_adapter() {}

  private:

    template <class T>
    inline boolean_type is() const {
        return this->type() == typeid(T);
    }

    boolean_type shares_type_with(abstract_type const& that) const {
        // FIXME: Figure out if this is reliable, otherwise defer to concrete adapters.
        return this->type() == that.type();
    }


    template <class Adapter>
    inline Adapter const* as() const {
        // TODO: Deal with forwarding_adapter's and with reference_wrapper's.
        Adapter const* const this_ = dynamic_cast<Adapter const*>(this);
        // BOOST_ASSERT(this_ != 0);
        return this_;
    }

  protected:

    template <class T, class A>
    friend struct adapter;

    template <class T, class _, class A, class D>
    friend struct forwarding_adapter;

    virtual boolean_type equal_adapted(abstract_type const& that) const = 0;
    virtual boolean_type less_adapted(abstract_type const& that) const  = 0;

    // virtual boolean_type equal_adapted(abstract_type const& that) const { AJG_SYNTH_THROW(bad_method("equal_adapted")); }
    // virtual boolean_type less_adapted(abstract_type const& that) const  { AJG_SYNTH_THROW(bad_method("less_adapted")); }

    template <class Adapter>
    inline boolean_type equal_as(abstract_type const& that) const {
        Adapter const* const this_ = this->template as<Adapter>();
        Adapter const* const that_ = that.template as<Adapter>();
        return this_ != 0 && that_ != 0 && std::equal_to<typename Adapter::adapted_type>()(this_->adapted_, that_->adapted_);
    }

    template <class Adapter>
    inline boolean_type less_as(abstract_type const& that) const {
        Adapter const* const this_ = this->template as<Adapter>();
        Adapter const* const that_ = that.template as<Adapter>();
        return this_ != 0 && that_ != 0 && std::less<typename Adapter::adapted_type>()(this_->adapted_, that_->adapted_);
    }

    void list(ostream_type& out, string_type const delimiter = traits_type::literal(", ")) const {
        size_type i = 0;

        BOOST_FOREACH(value_type const& value, *this) {
            if (i++) out << delimiter;
            out << value;
        }
    }

    boolean_type equal_sequence(abstract_type const& that) const {
        return this->equal_range(begin(), that.begin(), end(), that.end());
    }

    boolean_type equal_range( const_iterator i1, const_iterator i2
                            , const_iterator e1, const_iterator e2
                            ) const {
        for (; i1 != e1 && i2 != e2; ++i1, ++i2) {
            if (!i1->equal(*i2)) {
                return false;
            }
        }

        // Make sure |this| == |that|.
        return i1 == e1 && i2 == e2;
    }

    boolean_type less_sequence(abstract_type const& that) const {
        return this->less_range(begin(), that.begin(), end(), that.end());
    }

    boolean_type less_range( const_iterator i1, const_iterator i2
                           , const_iterator e1, const_iterator e2
                           ) const {
        for (; i1 != e1 && i2 != e2; ++i1, ++i2) {
            if (!i1->less(*i2)) {
                return false;
            }
        }

        // Make sure |this| <= |that|.
        return i1 == e1;
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_ABSTRACT_HPP_INCLUDED

