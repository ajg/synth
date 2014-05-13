//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_VALUE_BEHAVIOR_HPP_INCLUDED
#define AJG_SYNTH_VALUE_BEHAVIOR_HPP_INCLUDED

#include <map>
#include <set>
#include <vector>
#include <string>
#include <istream>
#include <ostream>
#include <sstream>
#include <utility>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <ajg/synth/adapters/base_adapter.hpp>

namespace ajg {
namespace synth {

///
/// value_behavior
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, template <class T> class Value>
struct value_behavior {
  public:

    typedef Traits                                                              traits_type;
    typedef Value<traits_type>                                                  value_type;
    typedef value_behavior                                                      behavior_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef typename value_type::iterator                                       iterator;
    typedef typename value_type::const_iterator                                 const_iterator;
    typedef typename value_type::range_type                                     range_type;

    typedef std::vector<value_type>                                             sequence_type;
    typedef std::map<string_type, value_type>                                   mapping_type;

    typedef boost::shared_ptr<base_adapter<behavior_type> const>                adapter_type;

  public:

    // NOTE:
    //     is<T>: whether exactly T (modulo const/volatile)
    //     as<T>: cast to exactly T& (modulo const/volatile) or throw
    //     to<T>: convert to T or throw

    template <class From>
    inline static string_type to_string(From const& from) {
        try {
            return boost::lexical_cast<string_type>(from);
        }
        catch (boost::bad_lexical_cast const&) {
            AJG_SYNTH_THROW(conversion_error(typeid(from), typeid(string_type)));
        }
    }

    template <class From>
    inline static floating_type to_floating(From const& from) {
        return to<floating_type>(from);
    }

    inline static size_type to_size(value_type const& value) {
        floating_type const f = value.to_floating();
        if (f <= 0) return 0;
        return static_cast<size_type>(f);
    }

    inline static string_type to_string(base_adapter<behavior_type> const& adapter) {
        std::basic_ostringstream<char_type> ostream;
        adapter.output(ostream);
        return ostream.str();
    }

    // NOTE: This requires T to have constructors for ALL these bunch of types, which is unusual.
    template <class T>
    inline static T to_(value_type const& value) {
        if (value.template is<T>())  return value.template as<T>();
        else if (value.is_boolean()) return T(value.to_boolean());
        else if (value.is_numeric()) return T(value.to_floating());
        else if (value.is_string())  return T(value.to_string());
        // TODO: Sequences, mappings, etc.
        else AJG_SYNTH_THROW(not_implemented("value_traits::to_"));
    }

    // Extremely crude implementation for the general case:
    template <class To, class From>
    inline static To to(From const& from) {
        To to;
        // TODO: This overlaps with behavior_type::to.
        std::basic_stringstream<char_type> stream;
        stream << from;
        stream >> to;
        return to;
    }

    template <class T>
    inline static boolean_type is(value_type const& value) {
        return value.type() == typeid(T);
    }

    template <class T>
    inline static T const& as(value_type const& value) {
        return value.template typed_as<T>();
    }

    inline static void enumerate( base_adapter<behavior_type> const& adapter
                                , ostream_type&                      ostream
                                ) {
        size_type i = 0;
        BOOST_FOREACH(value_type const& value, adapter.to_range()) {
            if (i++) ostream << ", ";
            ostream << value;
        }
    }

    inline static boolean_type contains(value_type const& a, value_type const& b) {
        return !a.find(b).equal(a.end()); // TODO: Defer to adapter.
    }

    inline static boolean_type equal(value_type const& a, value_type const& b) {
        // TODO: Defer to adapter even in non-same_as cases:
        //       if (...) return a.adapter()->equal(*that.adapter());
        if (a.typed_like(b))                        return a.typed_equal(b);
        else if (a.is_boolean() && b.is_boolean())  return a.to_boolean()  == b.to_boolean();
        else if (a.is_numeric() && b.is_numeric())  return a.to_floating() == b.to_floating();
        else if (a.is_string()  && b.is_string())   return a.to_string()   == b.to_string();
        // TODO: Sequences, mappings, etc.
        else return false;
    }

    inline static boolean_type less(value_type const& a, value_type const& b) {
        if (a.typed_like(b))                        return a.typed_less(b);
        else if (a.is_boolean() && b.is_boolean())  return a.to_boolean()  < b.to_boolean();
        else if (a.is_numeric() && b.is_numeric())  return a.to_floating() < b.to_floating();
        else if (a.is_string()  && b.is_string())   return a.to_string()   < b.to_string();
        // TODO: Sequences, mappings, etc.
        else return false;
    }

    /*
    inline static boolean_type equal_sequence( base_adapter<behavior_type> const& a
                                             , base_adapter<behavior_type> const& b
                                             ) {
        return equal_range(a.to_range(), b.to_range());
    }

    inline static boolean_type less_sequence( base_adapter<behavior_type> const& a
                                            , base_adapter<behavior_type> const& b
                                            ) {
        return less_range(a.to_range(), b.to_range());
    }

    // TODO: Use the right STL function.
    inline static boolean_type equal_range(range_type const& a, range_type const& b) {
        const_iterator i1 = a.first;
        const_iterator i2 = b.first;
        const_iterator e1 = a.second;
        const_iterator e2 = b.second;

        for (; i1 != e1 && i2 != e2; ++i1, ++i2) {
            if (!i1->equal(*i2)) {
                return false;
            }
        }

        // Make sure |this| == |that|.
        return i1 == e1 && i2 == e2;
    }

    // TODO: Use std::lexicographic_compare.
    inline static boolean_type less_range(range_type const& a, range_type const& b) {
        const_iterator i1 = a.first;
        const_iterator i2 = b.first;
        const_iterator e1 = a.second;
        const_iterator e2 = b.second;

        for (; i1 != e1 && i2 != e2; ++i1, ++i2) {
            if (!i1->less(*i2)) {
                return false;
            }
        }

        // Make sure |this| <= |that|.
        return i1 == e1;
    }
    */
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_VALUE_BEHAVIOR_HPP_INCLUDED
