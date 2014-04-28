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
#include <boost/cstdint.hpp>
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

    // TODO: Consider using arbitrary-precision types.
    typedef std::size_t                                 size_type;
 // typedef boost::uintmax_t                            natural_type;
    typedef boost::intmax_t                             integer_type;
    typedef /* TODO: long */ double                     number_type; // TODO: Rename to floating_type.

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

// TODO: Move everything below to value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

///
/// to_path:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static path_type to_path(string_type const& s) {
        return transcode<typename path_type::value_type>(s);
    }

///
/// utc_time
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static time_type utc_time() {
        return boost::posix_time::second_clock::universal_time();
    }

///
/// local_time
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static time_type local_time() {
        return boost::posix_time::second_clock::local_time();
    }

///
/// utc_datetime
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static datetime_type utc_datetime() {
        return datetime_type(utc_time(), utc_timezone());
    }

///
/// local_datetime
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static datetime_type local_datetime() {
        // TODO: Try return datetime_type(local_time(), local_timezone());
        return boost::local_time::local_sec_clock::local_time(local_timezone());
    }

///
/// utc_timezone:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static timezone_type utc_timezone() {
        return to_timezone(literal("UTC"), duration_type());
    }

///
/// local_timezone:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static timezone_type local_timezone() {
        // Based on the following, in <time.h>/<ctime>:
        //
        // void tzset (void);
        // extern char *tzname[2]; // [name, dst_name]
        // extern long timezone;   // UTC offset in seconds
        // extern int daylight;    // Whether the time zone suffers from DST
        //

        duration_type const one_hour = to_duration(1, 0, 0);
        return to_timezone( widen(std::string(tzname[0]))
                          , to_duration(0, 0, timezone)
                          , widen(std::string(tzname[1]))
                          , one_hour // FIXME: This is just a guess.
                          );
    }

///
/// to_timezone:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static timezone_type to_timezone( string_type   const& name
                                           , duration_type const& offset
                                           , string_type   const& dst_name   = string_type()
                                           , duration_type const& dst_offset = duration_type()
                                           ) {
        // e.g. PST-5, PST-5PDT, or PST-5PDT01:00:00
        string_type s = name.substr(0, 3) + to_string(offset);
        string_type const period = literal(",M3.2.0/2,M11.1.0/2"); // FIXME: Roughly U.S.-only

        if (!dst_name.empty() && !is_empty(dst_offset)) {
            s += dst_name.substr(0, 3) + to_string(dst_offset) + period;
        }
        else if (!is_empty(dst_offset)) {
            s += name.substr(0, 3) + to_string(dst_offset) + period;
        }
        else if (!dst_name.empty()) {
            s += dst_name.substr(0, 3) + period;
        }


        return timezone_type(new boost::local_time::posix_time_zone(narrow(s)));
    }

///
/// to_string:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type to_string(timezone_type const& tz, boolean_type const is_dst) {
        return tz ? (is_dst ? tz->dst_zone_name() : tz->std_zone_name()) : string_type();
    }

    inline static string_type to_string(duration_type const& duration) {
        string_type const s = widen(boost::posix_time::to_simple_string(duration));
        return duration.is_negative() ? /* char_type('-') + */ s : char_type('+') + s;
    }

///
/// to_duration:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static duration_type to_duration( size_type const hours       = 0
                                           , size_type const minutes     = 0
                                           , size_type const seconds     = 0
                                           , size_type const nanoseconds = 0
                                           ) {
        return duration_type(hours, minutes, seconds, nanoseconds);
    }

    inline static duration_type to_duration(timezone_type const& tz, boolean_type const is_dst) {
        return tz ? (is_dst ? tz->dst_offset() : tz->base_utc_offset()) : duration_type();
    }

    inline static duration_type to_duration(time_type const& time) {
        return time.time_of_day();
    }

///
/// is_empty:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static boolean_type is_empty(duration_type const& duration) {
        return duration == duration_type();
    }

///
/// to_time:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static time_type to_time(std::time_t const t) {
        return boost::posix_time::from_time_t(t);
    }

    inline static time_type to_time( size_type const hour        = 0
                                   , size_type const minute      = 0
                                   , size_type const second      = 0
                                   , size_type const nanoseconds = 0
                                   ) {
        return time_type(date_type(), to_duration(hour, minute, second, nanoseconds));
    }

    inline static time_type to_time( date_type     const& date
                                   , duration_type const& duration = duration_type()
                                   ) {
        return time_type(date, duration);
    }

///
/// to_date:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static date_type to_date( size_type const year  = 0
                                   , size_type const month = 0
                                   , size_type const day   = 0
                                   ) {
        return date_type(year, month, day);
    }

///
/// to_datetime
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static datetime_type to_datetime(time_type const& time) {
        return datetime_type(time, timezone_type());
    }

    inline static datetime_type to_datetime( date_type     const& date     = date_type()
                                           , time_type     const& time     = time_type()
                                           , timezone_type const& timezone = timezone_type()
                                           ) {
        return datetime_type(to_time(date, to_duration(time)), timezone);
    }

///
/// format_time
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
