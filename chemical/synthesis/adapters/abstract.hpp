
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ADAPTERS_ABSTRACT_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ADAPTERS_ABSTRACT_HPP_INCLUDED

#include <string>
#include <istream>
#include <ostream>
#include <typeinfo>
#include <stdexcept>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

namespace chemical {
namespace synthesis {

//
// bad_method exception
////////////////////////////////////////////////////////////////////////////////

struct bad_method : public std::invalid_argument {
    std::string const name;

    bad_method(std::string const& name)
        : std::invalid_argument("bad method `" + name + '"')
        , name(name) {}

    ~bad_method() throw () {}
};

//
// abstract_adapter
////////////////////////////////////////////////////////////////////////////////

template <class Traits>
struct abstract_adapter {
  public:

    typedef abstract_adapter this_type;
    typedef abstract_adapter abstract_type;
    typedef Traits           traits_type;

    typedef typename traits_type::char_type    char_type;
    typedef typename traits_type::size_type    size_type;
    typedef typename traits_type::value_type   value_type;
    typedef typename traits_type::boolean_type boolean_type;
    typedef typename traits_type::number_type  number_type;
    typedef typename traits_type::string_type  string_type;
    typedef typename traits_type::istream_type istream_type;
    typedef typename traits_type::ostream_type ostream_type;

    typedef typename traits_type::iterator       iterator;
    typedef typename traits_type::const_iterator const_iterator;

    //typedef value_iterator<value_type const> iterator;
    //typedef value_iterator<value_type const> const_iterator;

  public:

    virtual const_iterator begin() const { throw_exception(bad_method("begin")); }
    virtual const_iterator end()   const { throw_exception(bad_method("end")); }

    virtual number_type  count() const { throw_exception(bad_method("count")); }
    virtual boolean_type test()  const { throw_exception(bad_method("test")); }

    virtual boolean_type equal(abstract_type const& that) const {
        throw_exception(bad_method("equal"));
    }

    virtual void input (istream_type& in)        { throw_exception(bad_method("input")); }
    virtual void output(ostream_type& out) const { throw_exception(bad_method("output")); }

    virtual std::type_info const& type() const { throw_exception(bad_method("type")); }

    virtual const_iterator find(value_type const& value) const {
        throw_exception(bad_method("find"));
    }

  public:

    virtual ~abstract_adapter() {}

  protected:

    template <class This, class That>
    inline static boolean_type compare(This const& this_, That const& that) {
        // TODO: Deal with forwarding_adapter's and with reference_wrapper's.
        This const* const self = dynamic_cast<This const*>(&that);
        return self && this_.adapted_ == self->adapted_;
    }

    void list( ostream_type&      out
             , string_type const& delimiter
                  = lexical_cast<string_type>(", ")
             ) const {
        size_type i = 0;

        BOOST_FOREACH(value_type const& value, *this) {
            if (i++) out << delimiter;
            out << value;
        }
    }

    boolean_type compare_sequence(abstract_type const& that) const {
        // TODO: See if we can refactor this using std::equal.
        const_iterator i1 = begin(), i2 = that.begin();
        const_iterator const e1 = end(), e2 = that.end();

        for (; i1 != e1 && i2 != e2; ++i1, ++i2) {
            if (!i1->equal(*i2)) {
                return false;
            }
        }

        // Make sure |this| == |that|.
        return i1 == e1 && i2 == e2;
    }
};

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_ADAPTERS_ABSTRACT_HPP_INCLUDED

