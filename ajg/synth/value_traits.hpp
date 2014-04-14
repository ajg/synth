//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED
#define AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED

#include <map>
#include <set>
#include <vector>
#include <string>
#include <istream>
#include <ostream>
#include <sstream>
#include <utility>

#include <boost/none_t.hpp>
#include <boost/foreach.hpp>
#include <boost/mpl/void.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ajg/synth/value_iterator.hpp>

//
// TODO: Construct a consistent taxonomy--
//
//        specific | general
//       ----------+----------
//        void     | unit?
//        none_t   | none/nil/nullary/nothing?
//        bool     | boolean
//        char     | character
//        number   | numeric
//        string   | text/textual?
//        datetime | chronological
//        array    | sequential+homogenous+fixed?
//        vector   | sequential+homogenous+dynamic?
//        tuple    | sequential+heterogenous+fixed?
//        list     | sequential+heterogenous+dynamic?
//        set      | mapping/indexed/associative+simple?
//        map      | mapping/indexed/associative+pair?
//

namespace ajg {
namespace synth {

//
// default_traits
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Char>
struct default_traits {

  public:

    typedef default_traits                              traits_type;
    typedef boost::mpl::void_                           void_type;
    typedef boost::none_t                               none_type;
    typedef bool                                        boolean_type;

    typedef Char                                        char_type;

    typedef std::size_t                                 size_type;
    // TODO: The type number is too ambiguous; break down into integral (integer, natural), floating (long double), complex, etc.
    //       Either that or make number general enough to hold the majority of numbers, e.g. std::complex<long double> or
    //       even an arbitrary precision type.
    typedef double                                      number_type;

    // TODO: Add a time_type with optional time zome, and incorporate it into datetime_type.
    typedef boost::gregorian::date                      date_type;
    typedef boost::posix_time::ptime                    datetime_type;
    typedef boost::posix_time::time_duration            duration_type;

    typedef std::basic_string<char_type>                string_type;

    typedef string_type                                 path_type;
    typedef std::vector<path_type>                      paths_type;

    typedef std::set<string_type>                       symbols_type; // TODO[c++11]: unordered?
    typedef std::vector<string_type>                    names_type;   // TODO: scope_type? namespace_type?

    typedef std::basic_istream<char_type>               istream_type;
    typedef std::basic_ostream<char_type>               ostream_type;

  public:

    inline static string_type literal(char const* const s) {
        return widen(std::string(s));
    }

    /* TODO: Investigate if we can sacrifice compile-time for runtime.
    template <size_type N>
    inline static ... literal(char const (&n)[N]) { ... }
    */

///
/// transcode:
///     Centralizes string conversions in one place.
////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class To, class From>
    inline static std::basic_string<To> transcode(std::basic_string<From> const& s) {
        return std::basic_string<To>(s.begin(), s.end());
    }

///
/// narrow, widen:
///     These are misnomers since `Char` doesn't have to be 'wider' than `char`; they are shorcuts
///     to `transcode` and useful for interacting with APIs that only support one or the other.
////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class C> inline static std::basic_string<char> narrow(std::basic_string<C> const& s) { return transcode<char, C>(s); }
    template <class C> inline static std::basic_string<Char> widen (std::basic_string<C> const& s) { return transcode<Char, C>(s); }

    inline static std::basic_string<char> const& narrow(std::basic_string<char> const& s) { return s; }
    inline static std::basic_string<Char> const& widen (std::basic_string<Char> const& s) { return s; }
};


template <class Traits>
struct base_adapter;

///
/// default_value_behavior.
///     TODO: Move to own file
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, template <class T> class Value>
struct default_value_behavior {

  public:

    typedef Traits                                                              traits_type;
    typedef Value<traits_type>                                                  value_type;
    typedef default_value_behavior                                              behavior_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef typename value_type::iterator                                       iterator;
    typedef typename value_type::const_iterator                                 const_iterator;
    typedef typename value_type::range_type                                     range_type;

    typedef std::vector<value_type>                                             sequence_type;
    typedef std::map<string_type, value_type>                                   mapping_type;

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
    inline static number_type to_number(From const& from) {
        return to<number_type>(from);
    }

    inline static size_type to_size(value_type const& value) {
        number_type const number = value.to_number();
        if (number <= 0) return 0;
        return static_cast<size_type>(number);
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
        else if (value.is_numeric()) return T(value.to_number());
        else if (value.is_string())  return T(value.to_string());
        // TODO: Sequences, mappings, etc.
        else AJG_SYNTH_THROW(not_implemented("value_traits::to"));
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

    inline static boolean_type equal(value_type const& a, value_type const& b) {
        // TODO: Defer to adapter even in non-same_as cases:
        //       if (...) return a.adapter()->equal(*that.adapter());
        if (a.typed_like(b))                        return a.typed_equal(b);
        else if (a.is_boolean() && b.is_boolean())  return a.to_boolean() == b.to_boolean();
        else if (a.is_numeric() && b.is_numeric())  return a.to_number()  == b.to_number();
        else if (a.is_string()  && b.is_string())   return a.to_string()  == b.to_string();
        // TODO: Sequences, mappings, etc.
        else return false;
    }

    inline static boolean_type less(value_type const& a, value_type const& b) {
        if (a.typed_like(b))                        return a.typed_less(b);
        else if (a.is_boolean() && b.is_boolean())  return a.to_boolean() < b.to_boolean();
        else if (a.is_numeric() && b.is_numeric())  return a.to_number()  < b.to_number();
        else if (a.is_string()  && b.is_string())   return a.to_string()  < b.to_string();
        // TODO: Sequences, mappings, etc.
        else return false;
    }
    inline static boolean_type equal_sequence( base_adapter<behavior_type> const& a
                                             , base_adapter<behavior_type> const& b
                                             ) {
        return equal_range(a.begin(), b.begin(), a.end(), b.end());
    }

    inline static boolean_type less_sequence( base_adapter<behavior_type> const& a
                                            , base_adapter<behavior_type> const& b
                                            ) {
        return less_range(a.begin(), b.begin(), a.end(), b.end());
    }

    // TODO: Use the right STL function.
    inline static boolean_type equal_range( const_iterator i1, const_iterator i2
                                          , const_iterator e1, const_iterator e2
                                          ) {
        for (; i1 != e1 && i2 != e2; ++i1, ++i2) {
            if (!i1->equal(*i2)) {
                return false;
            }
        }

        // Make sure |this| == |that|.
        return i1 == e1 && i2 == e2;
    }

    // TODO: Use std::lexicographic_compare.
    inline static boolean_type less_range( const_iterator i1, const_iterator i2
                                         , const_iterator e1, const_iterator e2
                                         ) {
        for (; i1 != e1 && i2 != e2; ++i1, ++i2) {
            if (!i1->less(*i2)) {
                return false;
            }
        }

        // Make sure |this| <= |that|.
        return i1 == e1;
    }

    inline static void enumerate( base_adapter<behavior_type> const& adapter
                                , ostream_type&                    ostream
                                , string_type               const& delimiter = traits_type::literal(", ")
                                ) {
        size_type i = 0;
        BOOST_FOREACH(value_type const& value, adapter) {
            if (i++) ostream << delimiter;
            ostream << value;
        }
    }

    inline static boolean_type contains(value_type const& a, value_type const& b) {
        return !a.find(b).equal(a.end()); // TODO: Defer to adapter.
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED
