//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED
#define AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED

#include <map>
#include <set>
#include <ctime>
#include <cctype>
#include <vector>
#include <string>
#include <istream>
#include <ostream>
#include <sstream>
#include <utility>

#include <boost/none_t.hpp>
#include <boost/foreach.hpp>
#include <boost/cstdint.hpp>
#include <boost/optional.hpp>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>

#include <ajg/synth/value_iterator.hpp>
#include <ajg/synth/detail/text.hpp>

//
// TODO: Construct a consistent taxonomy--
//
//  C++                | Python          | Python concepts  | C++ concepts
// --------------------+-----------------+------------------+----------------------
//  void               | None            |                  | unit
//  NULL/nullptr       | None            |                  | unit
//  none_t             | None            |                  | unit
//  bool               | True/False      |                  | boolean
//  char               | -               |                  | character,textual?
//  wchar_t            | -               |                  | character,textual?
//  int                | long            |                  | numeric,integral,signed
//  unsigned int       | long            |                  | numeric,integral,unsigned
//  float              | float           |                  | numeric,floating,signed
//  double             | float           |                  | numeric,floating,signed
//  complex            | complex         |                  | numeric?
//  string             | str             | String           | textual,sequential
//  wstring?           | unicode         | ?                | textual,sequential
//  gregorian::date    | date            |                  | chronologic
//  ptime              | time            |                  | chronologic
//  local_date_time    | datetime        |                  | chronologic
//  array              | tuple           | Sequence         | sequential,homogenous,fixed
//  vector             | list            | Sequence         | sequential,homogenous,dynamic
//  tuple              | tuple           | Sequence         | sequential,heterogenous,fixed
//  list               | list            | Sequence         | sequential,heterogenous,dynamic
//  set                | set             | Mapping          | associative,sorted,unique,simple
//  map                | dict            | Mapping          | associative,sorted,unique,pair
//  multiset           |                 |                  | associative,sorted,multiple,simple
//  multimap           |                 |                  | associative,sorted,multiple,pair
//  unordered_set      |                 |                  | associative,unique,simple
//  unordered_map      |                 |                  | associative,unique,pair
//  unordered_multiset |                 |                  | associative,multiple,simple
//  unordered_multimap |                 |                  | associative,multiple,pair
// --------------------+-----------------+------------------+----------------------
//  sequence_type      |                 |                  | sequential
//  association_type   |                 |                  | associative

namespace ajg {
namespace synth {

//
// default_traits
//     TODO: Rename value_traits or rename file to default_traits.hpp.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Char>
struct default_traits {
  public:

    typedef default_traits                                                      self_type;
    typedef default_traits                                                      traits_type;
    typedef boost::none_t                                                       none_type;
    typedef bool                                                                boolean_type; // TODO: Rename bool_type.
    typedef Char                                                                char_type;

    // TODO: Consider using arbitrary-precision types for integer_type and natural_type.
    typedef std::size_t                                                         size_type;
 // typedef boost::uintmax_t                                                    natural_type;
    typedef boost::intmax_t                                                     integer_type;
    typedef /* TODO? long */ double                                             floating_type;
    typedef floating_type                                                       number_type;

    // XXX: Consider using something else for time_type because ptime (a) needs a date and (b) has no timezone.
    typedef boost::gregorian::date                                              date_type;
    typedef boost::posix_time::ptime                                            time_type;
    typedef boost::posix_time::microsec_clock                                   clock_type;
    typedef boost::posix_time::time_duration                                    duration_type;
    typedef boost::local_time::local_date_time                                  datetime_type;
 // typedef boost::local_time::local_time_period                                period_type;

    typedef std::pair<char_type const*, size_type>                              buffer_type;
    typedef std::basic_string<char_type>                                        string_type;

    typedef string_type                                                         region_type;
    typedef std::pair<region_type, boost::local_time::time_zone_ptr>            timezone_type; // TODO: Rename zone_type?

    typedef string_type                                                         path_type;
    typedef std::vector<path_type>                                              paths_type;

    typedef boost::optional<string_type>                                        url_type;

    typedef std::set<string_type>                                               symbols_type; // TODO[c++11]: unordered?
    typedef std::vector<string_type>                                            names_type;   // TODO: scope_type? namespace_type?

    typedef boost::optional<std::pair<string_type, boolean_type> >              language_type; // (lang_code, is_rtl)

    // TODO[c++11]: unordered_map
    typedef string_type                                                         format_type;
    typedef std::map<string_type, format_type>                                  formats_type;

    typedef std::basic_istream<char_type>                                       istream_type;
    typedef std::basic_ostream<char_type>                                       ostream_type;

  private:

    typedef detail::text<string_type>                                           text;

  public:

// TODO: Move everything below to value or context.
////////////////////////////////////////////////////////////////////////////////////////////////////

///
/// to_path:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static path_type to_path(string_type const& s) {
        return text::template transcode<typename path_type::value_type>(s);
    }

///
/// utc_time
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static time_type utc_time() {
        return clock_type::universal_time();
    }

/*
///
/// machine_time
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static time_type machine_time() {
        return clock_type::local_time();
    }
*/

///
/// utc_datetime
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static datetime_type utc_datetime() {
        return datetime_type(utc_time(), utc_timezone().second);
    }

///
/// local_datetime
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static datetime_type local_datetime(timezone_type const& timezone) {
        return boost::local_time::local_sec_clock::local_time(timezone.second);
    }

///
/// utc_timezone:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static timezone_type utc_timezone() {
        return self_type::to_posix_timezone(text::literal("UTC"), empty_duration());
    }

///
/// machine_timezone:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static timezone_type machine_timezone() {
        // Based on the following, in <time.h>/<ctime>:
        //
        // void tzset (void);
        // extern char *tzname[2]; // [name, dst_name]
        // extern long timezone;   // UTC offset in seconds
        // extern int daylight;    // Whether the time zone suffers from DST at any point
        //

        if (!daylight) {
            // The happy path for sane timezones:
            return self_type::to_posix_timezone
                ( text::widen(std::string(tzname[0]))
                , self_type::to_duration(0, 0, timezone)
                );
        }
        // The sad path for timezones afflicted with DST:
        return self_type::to_posix_timezone
            ( text::widen(std::string(tzname[0]))
            , self_type::to_duration(0, 0, timezone)
            , text::widen(std::string(tzname[1]))
            , self_type::to_duration(1, 0, 0) // One hour guesstimate.
            );
    }

///
/// is_dst:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static boolean_type is_dst(datetime_type const& datetime) {
        return datetime.is_dst();
    }

///
/// is_region:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static boolean_type is_region(string_type const& s) {
        // XXX: Potentially faster, less robust version:
        // size_type const a = index_of('/'), b = index_of(','); return a != -1 && (b == -1 || a < b)

        size_type n = 0;
        BOOST_FOREACH(char_type const c, s) {
            if ((std::isalnum)(static_cast<int>(c)) || c == char_type('_')) {
                if (n == 0) n = 1;
                else if (n == 2) n = 3;
            }
            else if (c == char_type('/')) {
                if (n++ != 1) return false;
            }
            else {
                return false;
            }
        }
        return n == 3;
    }

///
/// to_timezone:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static timezone_type to_timezone(datetime_type const& datetime, region_type const& region = region_type()) {
        return timezone_type(region, datetime.zone());
    }

    inline static timezone_type to_timezone(string_type const& s) {
        if (s.empty()) {
            return self_type::empty_timezone();
        }
        else if (self_type::is_region(s)) { // TODO: Cut off anything after Foo/Bar.
            return self_type::to_region_timezone(s);
        }
        else {
            return self_type::to_posix_timezone(s);
        }
    }

///
/// load_tz_db:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static boost::local_time::tz_database load_tz_db() {
        boost::local_time::tz_database tz_db;

        std::stringstream ss(
            #include <external/other/date_time_zonespec.hpp>
        );

        // tz_db.load_from_file("external/boost/libs/date_time/data/date_time_zonespec.csv");
        tz_db.load_from_stream(ss);
        return tz_db;
    }

///
/// to_region_timezone, to_posix_timezone:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static timezone_type to_region_timezone(region_type const& region) {
        static boost::local_time::tz_database const tz_db(self_type::load_tz_db());

        if (region.empty()) {
            return self_type::empty_timezone();
        }

        BOOST_ASSERT(self_type::is_region(region));

        return timezone_type(region, tz_db.time_zone_from_region(text::narrow(region)));
    }

    inline static timezone_type to_posix_timezone(string_type const& posix_string) {
        if (posix_string.empty()) {
            return self_type::empty_timezone();
        }

        BOOST_ASSERT(!self_type::is_region(posix_string));

        // TODO: Consider using caching here (even thread-locally) to reduce allocations.
        boost::local_time::time_zone_ptr const ptr(new boost::local_time::posix_time_zone(text::narrow(posix_string)));
        return timezone_type(region_type(), ptr);
    }

    inline static timezone_type to_posix_timezone( string_type   const& std_name
                                                 , duration_type const& std_offset
                                                 , string_type   const& dst_name   = string_type()
                                                 , duration_type const& dst_offset = duration_type()
                                                 // Note: This is a rough, U.S.-centric guess:
                                                 // TODO: Switch to datetime_type for these:
                                                 , string_type   const& dst_start  = text::literal("M3.2.0/2")
                                                 , string_type   const& dst_finish = text::literal("M11.1.0/2")
                                                 ) {
        if (std_name.empty() && is_empty(std_offset)) {
            return self_type::empty_timezone();
        }

        // e.g. PST-5, PST-5PDT, or PST-5PDT01:00:00
        string_type s = std_name.substr(0, 3) + self_type::to_string(std_offset);
        string_type const period = char_type(',') + dst_start + char_type(',') + dst_finish;

        if (!dst_name.empty() && !is_empty(dst_offset)) {
            s += dst_name.substr(0, 3) + self_type::to_string(dst_offset) + period;
        }
        else if (!is_empty(dst_offset)) {
            s += std_name.substr(0, 3) + self_type::to_string(dst_offset) + period;
        }
        else if (!dst_name.empty()) {
            s += dst_name.substr(0, 3) + period;
        }

        return self_type::to_posix_timezone(s);
    }

///
/// to_string:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type to_string_at(timezone_type const& tz, datetime_type const& datetime) {
        if (!tz.first.empty()) {
            return tz.first;
        }
        else if (tz.second) {
            return datetime.is_dst() ? tz.second->dst_zone_name() : tz.second->std_zone_name();
        }
        return string_type();
    }

    inline static string_type to_string(timezone_type const& tz) {
        if (!tz.first.empty()) {
            return tz.first;
        }
        else if (tz.second) {
            return tz.second->to_posix_string();
        }
        return string_type();
    }

    inline static string_type to_string(duration_type const& duration) {
        string_type const s = text::widen(boost::posix_time::to_simple_string(duration));
        return duration.is_negative() ? /* char_type('-') + */ s : char_type('+') + s;
    }

///
/// to_boolean:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static boolean_type to_boolean(timezone_type const& tz) {
        return !tz.first.empty() || tz.second;
    }

///
/// to_duration:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static duration_type to_duration( size_type const hours
                                           , size_type const minutes
                                           , size_type const seconds
                                           , size_type const microseconds = 0
                                           ) {
        return duration_type(hours, minutes, seconds, microseconds);
    }

    inline static duration_type to_duration_at(timezone_type const& tz, datetime_type const& datetime) {
        if (boost::local_time::time_zone_ptr const ptr = tz.second ? tz.second : to_region_timezone(tz.first).second) {
            return datetime.is_dst() ? ptr->dst_offset() : ptr->base_utc_offset();
        }

        return self_type::empty_duration();
    }

    inline static duration_type to_duration(time_type const& time) {
        return time.time_of_day();
    }

///
/// to_seconds:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static integer_type to_seconds(duration_type const duration) {
        // XXX: return static_cast<integer_type>(duration.total_seconds());
        return static_cast<integer_type>(duration.ticks() / duration_type::ticks_per_second());
    }

///
/// empty_*:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static date_type     empty_date()     { return date_type(); }
    inline static time_type     empty_time()     { return time_type(); }
    inline static duration_type empty_duration() { return duration_type(); }
    inline static timezone_type empty_timezone() { return timezone_type(); }
    inline static datetime_type empty_datetime() { return self_type::to_datetime(date_type()); }

///
/// is_empty:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static boolean_type is_empty(duration_type const& duration) {
        return duration == empty_duration();
    }

///
/// to_utc_time, to_local_time:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static time_type to_utc_time(datetime_type const& datetime) {
        return time_type(datetime.utc_time());
    }

    inline static time_type to_local_time(datetime_type const& datetime) {
        return time_type(datetime.local_time());
    }

///
/// to_time:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static time_type to_time(std::time_t const t) {
        return boost::posix_time::from_time_t(t);
    }

    inline static time_type to_time( size_type const hour
                                   , size_type const minute
                                   , size_type const second
                                   , size_type const microseconds = 0
                                   ) {
        AJG_SYNTH_THROW(not_implemented("to_time"));
        // return time_type(empty_date(), self_type::to_duration(hour, minute, second, microseconds));
    }

    inline static time_type to_time( date_type     const& date
                                   , duration_type const& duration = traits_type::empty_duration()
                                   ) {
        return time_type(date, duration);
    }

///
/// to_date:
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static date_type to_date( size_type const year
                                   , size_type const month
                                   , size_type const day
                                   ) {
        return date_type(year, month, day);
    }

    inline static date_type to_date(datetime_type const& datetime) {
        return date_type(datetime.date());
    }

///
/// to_datetime
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static datetime_type to_datetime(time_type const& time) {
        return datetime_type(time, self_type::empty_timezone().second);
    }

    inline static datetime_type to_datetime( date_type     const& date     // =  self_type::empty_date()
                                           , time_type     const& time     // =  self_type::empty_time()
                                           , timezone_type const& timezone // =  self_type::empty_timezone()
                                           ) {
        return datetime_type(self_type::to_time(date, self_type::to_duration(time)), timezone.second);
    }

    inline static datetime_type to_datetime( date_type     const& date
                                           , duration_type const& duration = self_type::empty_duration()
                                           , timezone_type const& timezone = self_type::empty_timezone()
                                           ) {
        return datetime_type(self_type::to_time(date, duration), timezone.second);
    }

    /*
    inline static void set_global_facets() {
        typedef typename boost::date_time::time_facet<datetime_type, char_type> facet_type;
        std::locale::global(std::locale(std::locale::classic(),
            new facet_type(facet_type::iso_format_extended_specifier)));
    }*/

    inline static std::locale const& standard_locale() {
        typedef typename boost::date_time::date_facet<date_type, char_type>     date_facet_type;
        typedef typename boost::date_time::time_facet<datetime_type, char_type> time_facet_type;
        // TODO: Matching input facets.
        static std::locale const l(std::locale(std::locale(),
            new date_facet_type(date_facet_type::iso_format_extended_specifier)),
            new time_facet_type(time_facet_type::iso_time_format_extended_specifier));
        return l;
    }

    template <class T>
    inline static std::locale formatted_locale(std::locale const& original, char_type const* const format) {
        typedef typename boost::date_time::time_facet<T, char_type> facet_type;
        // TODO: Matching input facet.
        // TODO: date_facet?
        BOOST_ASSERT(format);

        #if AJG_SYNTH_HAS_CXX11

            // TODO: Re-use the locale or even the whole ostringstream rather than just the facet.
            // The refcount below is one 1 to prevent it from being deleted prematurely.
            static thread_local facet_type facet(1);
            facet.format(format);
            return std::locale(original, &facet);

        #else

            return std::locale(original, new facet_type(format));

        #endif
    }

///
/// TODO: format_date
////////////////////////////////////////////////////////////////////////////////////////////////////

///
/// format_time
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type format_time(string_type const& format, time_type const& time) {
        std::basic_ostringstream<char_type> stream;
        stream.imbue(formatted_locale<time_type>(stream.getloc(), format.c_str()));
        stream << time;
        BOOST_ASSERT(stream);
        return stream.str();
    }

///
/// format_datetime
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type format_datetime(string_type const& format, datetime_type const& datetime) {
        std::basic_ostringstream<char_type> stream;
        stream.imbue(formatted_locale<datetime_type>(stream.getloc(), format.c_str()));
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
