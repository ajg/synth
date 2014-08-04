//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_FORMATTER_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_FORMATTER_HPP_INCLUDED

#include <map>
#include <sstream>

#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/assign/list_of.hpp>

#include <ajg/synth/detail/text.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace django {

//
// formatter
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Options>
struct formatter {
  public:

    typedef Options                                                             options_type;
    typedef typename options_type::traits_type                                  traits_type;
    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::arguments_type                               arguments_type;
    typedef typename options_type::context_type                                 context_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::integer_type                                  integer_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::date_type                                     date_type;
    typedef typename traits_type::time_type                                     time_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::timezone_type                                 timezone_type;
    typedef typename traits_type::string_type                                   string_type;

    typedef typename value_type::range_type                                     range_type;
    typedef typename value_type::sequence_type                                  sequence_type;

  private:

    typedef detail::text<string_type>                                           text;

  private:

///
/// native_flags:
///     The flag specifiers native to Boost.DateTime.
///     See [http://www.boost.org/doc/html/date_time/date_time_io.html#date_time.format_flags]
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct native_flags {
        static size_type const size = 31;
        // NOTE:
        //   - Fields marked [#] are not used because their availability is platform-dependent.
        //   - Fields marked [*] are not used because Windows is garbage and barfs otherwise.

        // Date flags:
        string_type a;
        string_type A;
        string_type b;
        string_type B;
        string_type c;
     // string_type C; [#]
        string_type d;
     // string_type D; [#]
     // string_type e; [#]
     // string_type g; [*]
        string_type G;
     // string_type h; [#]
        string_type j;
        string_type m;
     // string_type u; [*]
        string_type U;
     // string_type V; [#]
        string_type w;
        string_type W;
        string_type x;
        string_type y;
        string_type Y;

        // Time flags:
        string_type f;
        string_type F;
        string_type H;
        string_type I;
     // string_type k; [*]
     // string_type l; [*]
        string_type M;
     // string_type O; [*]
        string_type p;
     // string_type P; [#]
     // string_type r; [#]
        string_type R;
        string_type s;
        string_type S;
        string_type T;
        string_type q;
        string_type Q;
        string_type X;
        string_type z;
        string_type Z;
        string_type ZP;

        inline static native_flags from_datetime(datetime_type datetime) {
            static std::vector<string_type> const format_flags = boost::assign::list_of<string_type>
                (text::literal("%a"))
                (text::literal("%A"))
                (text::literal("%b"))
                (text::literal("%B"))
                (text::literal("%c"))
                (text::literal("%d"))
                (text::literal(AJG_SYNTH_IF_WINDOWS("%y", "%G"))) // TODO: Find workaround for Windows.
                (text::literal("%j"))
                (text::literal("%m"))
                (text::literal("%U"))
                (text::literal("%w"))
                (text::literal("%W"))
                (text::literal("%x"))
                (text::literal("%y"))
                (text::literal("%Y"))
                (text::literal("%f"))
                (text::literal("%F"))
                (text::literal("%H"))
                (text::literal("%I"))
                (text::literal("%M"))
                (text::literal("%p"))
                (text::literal("%R"))
                (text::literal("%s"))
                (text::literal("%S"))
                (text::literal("%T"))
                (text::literal("%q"))
                (text::literal("%Q"))
                (text::literal("%X"))
                (text::literal("%z"))
                (text::literal("%Z"))
                (text::literal("%ZP"))
                ;

            static string_type const format_string     = text::join(format_flags, text::literal("|"));
            string_type const formatted_string         = traits_type::format_datetime(format_string, datetime);
            std::vector<string_type> const& specifiers = text::split(formatted_string, text::literal("|"), native_flags::size);
            AJG_SYNTH_ASSERT(specifiers.size() == native_flags::size);

            native_flags const flags =
                { specifiers[0]
                , specifiers[1]
                , specifiers[2]
                , specifiers[3]
                , specifiers[4]
                , specifiers[5]
                , specifiers[6]
                , specifiers[7]
                , specifiers[8]
                , specifiers[9]
                , specifiers[10]
                , specifiers[11]
                , specifiers[12]
                , specifiers[13]
                , specifiers[14]
                , specifiers[15]
                , specifiers[16]
                , specifiers[17]
                , specifiers[18]
                , specifiers[19]
                , specifiers[20]
                , specifiers[21]
                , specifiers[22]
                , specifiers[23]
                , specifiers[24]
                , specifiers[25]
                , specifiers[26]
                , specifiers[27]
                , specifiers[28]
                , specifiers[29]
                , specifiers[30]
                };
            return flags;
        }
    };

///
/// cooked_flags:
///     The flag specifiers used by Django.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct cooked_flags {
        string_type a;
        string_type A;
        string_type b;
        string_type B;
        string_type c;
        string_type d;
        string_type D;
        string_type e;
        string_type E;
        string_type f;
        string_type F;
        string_type g;
        string_type G;
        string_type h;
        string_type H;
        string_type i;
        string_type I;
        string_type j;
        string_type l;
        string_type L;
        string_type m;
        string_type M;
        string_type n;
        string_type N;
        string_type o;
        string_type O;
        string_type P;
        string_type r;
        string_type s;
        string_type S;
        string_type t;
        string_type T;
        string_type u;
        string_type U;
        string_type w;
        string_type W;
        string_type y;
        string_type Y;
        string_type z;
        string_type Z;

        inline static cooked_flags cook_flags(native_flags const& flags, datetime_type const& datetime) {
            date_type     const date        = traits_type::to_date(datetime);
            timezone_type const timezone    = traits_type::to_timezone(datetime);
            size_type     const day         = static_cast<size_type>(date.day());
            size_type     const year        = static_cast<size_type>(date.year());
            size_type     const iso_week    = static_cast<size_type>(date.week_number());
            size_type     const month_days  = static_cast<size_type>(date.end_of_month().day());
            boolean_type  const is_am       = flags.p == text::literal("AM");
            boolean_type  const is_pm       = flags.p == text::literal("PM");
            boolean_type  const has_minutes = flags.M != text::literal("00");
            boolean_type  const is_midnight = flags.H == text::literal("00") && !has_minutes;
            boolean_type  const is_noon     = flags.H == text::literal("12") && !has_minutes;
            boolean_type  const is_leapyear = ((year & 3) == 0 && ((year % 25) != 0 || (year & 15) == 0));
            boolean_type  const is_dst      = traits_type::is_dst(datetime);
            duration_type const offset      = traits_type::to_duration_at(timezone, datetime);
            integer_type  const offset_secs = static_cast<integer_type>(offset.total_seconds());
            string_type   const meridiem    = is_am ? text::literal("a.m.")
                                            : is_pm ? text::literal("p.m.")
                                            : string_type();
            string_type   const succint     = text::trim_leading_zeros(flags.I)
                                            + (has_minutes ? char_type(':') + flags.M : string_type());
            string_type   const informal    = is_midnight ? text::literal("midnight")
                                            : is_noon     ? text::literal("noon")
                                            : (succint + char_type(' ') + meridiem);
            string_type   const iso_offset  = traits_type::to_boolean(timezone) ? stringify(offset, true) : string_type();
            string_type   const iso8601     = flags.Y + char_type('-')
                                            + flags.m + char_type('-')
                                            + flags.d + char_type('T')
                                            + flags.H + char_type(':')
                                            + flags.M + char_type(':')
                                            + flags.S
                                            + iso_offset;
            string_type   const rfc_offset  = stringify(offset, false);
            string_type   const rfc2822     = flags.a + text::literal(", ")
                                            + flags.d + char_type(' ')
                                            + flags.b + char_type(' ')
                                            + flags.Y + char_type(' ')
                                            + flags.T + char_type(' ')
                                            + rfc_offset;

         // time_type     const utc_epoch   = traits_type::to_time(std::time_t(0));
            time_type     const utc_epoch   = traits_type::to_time(traits_type::to_date(1970, 1, 1));
            duration_type const since_epoch = traits_type::to_utc_time(datetime) - utc_epoch;
            timezone_type const machine_tz  = traits_type::machine_timezone();
            datetime_type const machine_dt  = traits_type::local_datetime(machine_tz);
            size_type     const unix_stamp  = static_cast<size_type>(since_epoch.seconds());

            cooked_flags cooked;
            cooked.a = meridiem;
            cooked.A = flags.p;
            cooked.b = text::lower(flags.b);
            cooked.B = string_type(); // NOTE: "Not implemented" per the spec.
            cooked.c = iso8601;
            cooked.d = flags.d;
            cooked.D = flags.a;
            cooked.e = flags.z; // XXX: Compare against traits_type::to_string_at(timezone, datetime);
            cooked.E = flags.B; // TODO: Make locale-aware.
            cooked.f = informal;
            cooked.F = flags.B;
            cooked.g = text::trim_leading_zeros(flags.I);
            cooked.G = text::trim_leading_zeros(flags.H);
            cooked.h = flags.I;
            cooked.H = flags.H;
            cooked.i = flags.M;
            cooked.I = text::literal(is_dst ? "1" : "0");
            cooked.j = text::trim_leading_zeros(flags.d);
            cooked.l = flags.A;
            cooked.L = text::literal(is_leapyear ? "True" : "False");
            cooked.m = flags.m;
            cooked.M = flags.b;
            cooked.n = text::trim_leading_zeros(flags.m);
            cooked.N = flags.b; // TODO: Use A.P. style.
            cooked.o = flags.G;
            cooked.O = rfc_offset;
            cooked.P = informal;
            cooked.r = rfc2822;
            cooked.s = flags.S;
            cooked.S = ordinal_suffix(static_cast<int>(day));
            cooked.t = text::stringize(month_days);
            cooked.T = traits_type::to_string_at(machine_tz, machine_dt);
            cooked.u = text::trim_left(flags.f, text::literal("."));
            cooked.U = text::stringize(unix_stamp);
            cooked.w = flags.w;
            cooked.W = text::stringize(iso_week);
            cooked.y = flags.y;
            cooked.Y = flags.Y;
            cooked.z = text::trim_leading_zeros(flags.j);
            cooked.Z = text::stringize(offset_secs);
            return cooked;
        }

        inline static string_type stringify(duration_type const& offset, boolean_type const colon) {
            return (offset.is_negative() ? char_type('-') : char_type('+'))
                 + (text::digitize(static_cast<size_type>(offset.hours()), 2))
                 + (colon ? text::literal(":") : string_type())
                 + (text::digitize(static_cast<size_type>(offset.minutes()), 2))
                 ;
        }

        inline static string_type ordinal_suffix(int const n) {
            AJG_SYNTH_ASSERT(n > 0 && n <= 31);
            switch (n) {
            case 1: case 21: return text::literal("st");
            case 2: case 22: return text::literal("nd");
            case 3: case 23: return text::literal("rd");
            default:         return text::literal("th");
            }
        }
    };

  public:

    static string_type format_datetime(string_type const& format, datetime_type const& datetime) {
        native_flags const native = native_flags::from_datetime(datetime);
        cooked_flags const cooked = cooked_flags::cook_flags(native, datetime);

        // TODO: This might not be UTF8-safe; consider using a utf8_iterator.
        std::basic_ostringstream<char_type> stream;
        BOOST_FOREACH(char_type const c, format) {
            switch (c) {
            case char_type('a'): stream << cooked.a; break;
            case char_type('A'): stream << cooked.A; break;
            case char_type('b'): stream << cooked.b; break;
            case char_type('B'): stream << cooked.B; break;
            case char_type('c'): stream << cooked.c; break;
            case char_type('d'): stream << cooked.d; break;
            case char_type('D'): stream << cooked.D; break;
            case char_type('e'): stream << cooked.e; break;
            case char_type('E'): stream << cooked.E; break;
            case char_type('f'): stream << cooked.f; break;
            case char_type('F'): stream << cooked.F; break;
            case char_type('g'): stream << cooked.g; break;
            case char_type('G'): stream << cooked.G; break;
            case char_type('h'): stream << cooked.h; break;
            case char_type('H'): stream << cooked.H; break;
            case char_type('i'): stream << cooked.i; break;
            case char_type('I'): stream << cooked.I; break;
            case char_type('j'): stream << cooked.j; break;
            case char_type('l'): stream << cooked.l; break;
            case char_type('L'): stream << cooked.L; break;
            case char_type('m'): stream << cooked.m; break;
            case char_type('M'): stream << cooked.M; break;
            case char_type('n'): stream << cooked.n; break;
            case char_type('N'): stream << cooked.N; break;
            case char_type('o'): stream << cooked.o; break;
            case char_type('O'): stream << cooked.O; break;
            case char_type('P'): stream << cooked.P; break;
            case char_type('r'): stream << cooked.r; break;
            case char_type('s'): stream << cooked.s; break;
            case char_type('S'): stream << cooked.S; break;
            case char_type('t'): stream << cooked.t; break;
            case char_type('T'): stream << cooked.T; break;
            case char_type('u'): stream << cooked.u; break;
            case char_type('U'): stream << cooked.U; break;
            case char_type('w'): stream << cooked.w; break;
            case char_type('W'): stream << cooked.W; break;
            case char_type('y'): stream << cooked.y; break;
            case char_type('Y'): stream << cooked.Y; break;
            case char_type('z'): stream << cooked.z; break;
            case char_type('Z'): stream << cooked.Z; break;
            default: stream << c;
            }
        }

        AJG_SYNTH_ASSERT(stream);
        return stream.str();
    }

  public:

    // TODO: Proper, localizable formatting.
    static string_type format_duration(duration_type const& duration) {
        static size_type const N = 6;

        static size_type const seconds[N] = { 60 * 60 * 24 * 365
                                            , 60 * 60 * 24 * 30
                                            , 60 * 60 * 24 * 7
                                            , 60 * 60 * 24
                                            , 60 * 60
                                            , 60
                                            };
        static string_type const units[N] = { text::literal("year")
                                            , text::literal("month")
                                            , text::literal("week")
                                            , text::literal("day")
                                            , text::literal("hour")
                                            , text::literal("minute")
                                            };
        integer_type const total = traits_type::to_seconds(duration);

        if (duration.is_negative() || total <= 0) {
            return formatter::pluralize_unit(0, units[N - 1]);
        }

        string_type result;
        integer_type count = 0;
        size_type i = 0;

        for (; i < N; ++i) {
            count = total / seconds[i];
            if (count != 0) {
                break;
            }
        }

        result += formatter::pluralize_unit(count, units[i]);

        if (i + 1 < N) {
            count = (total - (seconds[i] * count)) / seconds[i + 1];
            if (count != 0) {
                result += text::literal(", ") + pluralize_unit(count, units[i + 1]);
            }
        }

        return result;
    }

  private:

    /*inline static string_type nonbreaking(string_type const& s) {
        return s.replace(' ', '\xa0');
    }*/

    inline static string_type pluralize_unit(integer_type const n, string_type const& s) {
        string_type const suffix = n == 1 ? string_type() : text::literal("s");
        return text::stringize(n) + text::literal("\xc2\xa0") + s + suffix;
    }
};

}}}} // namespace ajg::synth::engines::django

#endif // AJG_SYNTH_ENGINES_DJANGO_FORMATTER_HPP_INCLUDED
