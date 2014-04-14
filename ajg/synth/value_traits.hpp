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

}} // namespace ajg::synth

#endif // AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED
