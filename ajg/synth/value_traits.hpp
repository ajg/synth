//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED
#define AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED

#include <map>
#include <set>
#include <ctime>
#include <vector>
#include <string>
#include <istream>
#include <ostream>
#include <sstream>
#include <utility>

#include <boost/none_t.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>

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

struct void_t { typedef void_t type; };

//
// default_traits
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Char>
struct default_traits {

  public:

    typedef default_traits                              traits_type;
    typedef void_t                                      void_type;
    typedef boost::none_t                               none_type;
    typedef bool                                        boolean_type;

    typedef Char                                        char_type;

    typedef std::size_t                                 size_type;
    // TODO: The type number is too ambiguous; break down into integral (integer, natural), floating (long double), complex, etc.
    //       Either that or make number general enough to hold the majority of numbers, e.g. std::complex<long double> or
    //       even an arbitrary precision type.
    typedef double                                      number_type;

    typedef boost::gregorian::date                      date_type;
    typedef boost::posix_time::ptime                    time_type;     // Note: No timezone.
    typedef boost::posix_time::second_clock             clock_type;
    typedef boost::posix_time::time_duration            duration_type;
    typedef boost::local_time::local_date_time          datetime_type;
 // typedef boost::local_time::local_time_period        period_type;
    typedef boost::local_time::time_zone_ptr            timezone_type;

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

///
/// to_path:
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static path_type to_path(string_type const& s) {
        return transcode<typename path_type::value_type>(s);
    }

///
/// to_time:
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static time_type to_time(std::time_t const t) {
        return boost::posix_time::from_time_t(t);
    }

///
/// to_datetime
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static datetime_type to_datetime(time_type const& time) {
        return datetime_type(time, no_timezone());
    }

///
/// utc_time
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static time_type utc_time() {
        return boost::posix_time::second_clock::universal_time();
    }

///
/// local_time
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static time_type local_time() {
        return boost::posix_time::second_clock::local_time();
    }

///
/// utc_datetime
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static datetime_type utc_datetime() {
        return datetime_type(utc_time(), utc_timezone());
    }

///
/// local_datetime
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static datetime_type local_datetime() {
        // TODO: Try return datetime_type(local_time(), local_timezone());
        return boost::local_time::local_sec_clock::local_time(local_timezone());
    }

///
/// no_timezone:
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static timezone_type no_timezone() {
        return timezone_type();
    }

///
/// utc_timezone:
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static timezone_type utc_timezone() {
        // TODO: make static const
        return timezone_type(new boost::local_time::posix_time_zone("UTC"));
    }

///
/// local_timezone:
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static /*timezone_type*/ boost::local_time::time_zone_ptr local_timezone() {
        // TODO: make static const
        return timezone_type(new boost::local_time::posix_time_zone(narrow(local_timezone_name())));
    }

///
/// local_timezone_name:
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type local_timezone_name() {
        std::time_t time = (std::time)(0);
        std::string name((std::localtime)(&time)->tm_isdst ? tzname[1] : tzname[0]);
        return traits_type::widen(name);
    }

///
/// format_time
///     TODO: Move to value_behavior.
///     TODO: Repeated in format_datetime.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type format_time(string_type const& format, time_type const& time) {
        typedef typename boost::date_time::time_facet<time_type, char_type> facet_type;

        std::basic_ostringstream<char_type> stream;
        // The locale takes care of deleting this thing for us.
        // TODO: Figure out a way to allocate the facet on the stack or reuse it.
        facet_type *const facet = new facet_type(format.c_str());
        stream.imbue(std::locale(stream.getloc(), facet));

        stream << time;
        BOOST_ASSERT(stream);
        return stream.str();
    }

///
/// format_datetime
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type format_datetime(string_type const& format, datetime_type const& datetime) {
        typedef typename boost::date_time::time_facet<datetime_type, char_type> facet_type;

        std::basic_ostringstream<char_type> stream;
        // The locale takes care of deleting this thing for us.
        // TODO: Figure out a way to allocate the facet on the stack or reuse it.
        facet_type *const facet = new facet_type(format.c_str());
        stream.imbue(std::locale(stream.getloc(), facet));

        stream << datetime;
        BOOST_ASSERT(stream);
        return stream.str();
    }

///
/// format_size
///     TODO: Move to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type format_size(size_type const size) {
        std::basic_ostringstream<char_type> stream;
        stream << std::fixed << std::setprecision(1);

        double bucket = 1;
        string_type unit;

             if (size >  (bucket = (std::pow)(2, 60.0))) stream << (size / bucket) << " EB";
        else if (size >  (bucket = (std::pow)(2, 50.0))) stream << (size / bucket) << " PB";
        else if (size >  (bucket = (std::pow)(2, 40.0))) stream << (size / bucket) << " TB";
        else if (size >  (bucket = (std::pow)(2, 30.0))) stream << (size / bucket) << " GB";
        else if (size >  (bucket = (std::pow)(2, 20.0))) stream << (size / bucket) << " MB";
        else if (size >  (bucket = (std::pow)(2, 10.0))) stream << (size / bucket) << " KB";
        else if (size >= (bucket = (std::pow)(2, 00.0))) stream << (size / bucket) << " bytes";

        BOOST_ASSERT(stream);
        return stream.str();
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED
