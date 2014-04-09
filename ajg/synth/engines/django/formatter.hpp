//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_FORMATTER_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_FORMATTER_HPP_INCLUDED

#include <map>
#include <ctime>
#include <sstream>

#include <boost/array.hpp>
#include <boost/function.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace django {

//
// formatter
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Options>
struct formatter {
  public:

    typedef Options                                                             options_type;
    typedef typename options_type::traits_type                                  traits_type;
    typedef typename options_type::boolean_type                                 boolean_type;
    typedef typename options_type::char_type                                    char_type;
    typedef typename options_type::size_type                                    size_type;
    typedef typename options_type::number_type                                  number_type;
    typedef typename options_type::date_type                                    date_type;
    typedef typename options_type::datetime_type                                datetime_type;
    typedef typename options_type::duration_type                                duration_type;
    typedef typename options_type::string_type                                  string_type;
    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::range_type                                   range_type;
    typedef typename options_type::sequence_type                                sequence_type;
    typedef typename options_type::arguments_type                               arguments_type;
    typedef typename options_type::context_type                                 context_type;

  private:

    typedef typename string_type::const_iterator                                string_iterator_type;
    typedef xpressive::match_results<string_iterator_type>                      string_match_type;

///
/// datetime_flags:
///     See http://www.boost.org/doc/libs/release/doc/html/date_time/date_time_io.html#date_time.format_flags
///     NOTE: Fields marked [*] are not used because their availability is platform-dependent.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct datetime_flags {
        BOOST_STATIC_CONSTANT(size_type, total   = 45);
        BOOST_STATIC_CONSTANT(size_type, skipped = 8);
        BOOST_STATIC_CONSTANT(size_type, size    = total - skipped);

        // Date flags
        string_type a;
        string_type A;
        string_type b;
        string_type B;
        string_type c;
     // string_type C; [*]
        string_type d;
        string_type D;
     // string_type e; [*]
        string_type g;
        string_type G;
     // string_type h; [*]
        string_type j;
        string_type m;
        string_type u;
        string_type U;
     // string_type V; [*]
        string_type w;
        string_type W;
        string_type x;
        string_type y;
        string_type Y;

        // Time flags
     // string_type -;
     // string_type +;
        string_type f;
        string_type F;
        string_type H;
        string_type I;
        string_type k;
        string_type l;
        string_type M;
        string_type O;
        string_type p;
     // string_type P; [*]
     // string_type r; [*]
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
    };

  public:

    static string_type format_datetime( options_type  const& options
                                      , string_type   const& string
                                      , datetime_type const& datetime
                                      ) {
        namespace algo = boost::algorithm;
        static boost::array<string_type, datetime_flags::size> const format_flags = {
            { traits_type::literal("%a")
            , traits_type::literal("%A")
            , traits_type::literal("%b")
            , traits_type::literal("%B")
            , traits_type::literal("%c")
            , traits_type::literal("%d")
            , traits_type::literal("%D")
            , traits_type::literal("%g")
            , traits_type::literal("%G")
            , traits_type::literal("%j")
            , traits_type::literal("%m")
            , traits_type::literal("%u")
            , traits_type::literal("%U")
            , traits_type::literal("%w")
            , traits_type::literal("%W")
            , traits_type::literal("%x")
            , traits_type::literal("%y")
            , traits_type::literal("%Y")
            , traits_type::literal("%f")
            , traits_type::literal("%F")
            , traits_type::literal("%H")
            , traits_type::literal("%I")
            , traits_type::literal("%k")
            , traits_type::literal("%l")
            , traits_type::literal("%M")
            , traits_type::literal("%O")
            , traits_type::literal("%p")
            , traits_type::literal("%R")
            , traits_type::literal("%s")
            , traits_type::literal("%S")
            , traits_type::literal("%T")
            , traits_type::literal("%q")
            , traits_type::literal("%Q")
            , traits_type::literal("%X")
            , traits_type::literal("%z")
            , traits_type::literal("%Z")
            , traits_type::literal("%ZP")
            }
        };
        static string_type const delimiter     = string_type(1, char_type('&'));
        static string_type const format_string = algo::join(format_flags, delimiter);
        string_type const formatted_string = detail::format_time(format_string, datetime);

        std::vector<string_type> specifiers;
        specifiers.reserve(datetime_flags::size);
        algo::split(specifiers, formatted_string, algo::is_any_of(delimiter));
        BOOST_ASSERT(specifiers.size() == datetime_flags::size);

        datetime_flags const flags =
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
            , specifiers[31]
            , specifiers[32]
            , specifiers[33]
            , specifiers[34]
            , specifiers[35]
            , specifiers[36]
            };

        std::basic_ostringstream<char_type> stream;
        typename options_type::formats_type::const_iterator const it = options.formats.find(string);
        string_type const format = it == options.formats.end() ? string : it->second;

        boolean_type const is_am       = flags.p == traits_type::literal("AM");
        boolean_type const is_pm       = flags.p == traits_type::literal("PM");
        boolean_type const has_minutes = flags.M != traits_type::literal("00");
        boolean_type const is_midnight = flags.H == "00" && !has_minutes;
        boolean_type const is_noon     = flags.H == "12" && !has_minutes;

        string_type const a = is_am ? traits_type::literal("a.m.") :
                              is_pm ? traits_type::literal("p.m.") : string_type();
        string_type const b = algo::to_lower_copy(flags.b);
        string_type const c = flags.Y + '-' + flags.m + '-' + flags.d + 'T' + flags.H + ':' + flags.M + ':' + flags.S;
        string_type const f = algo::trim_left_copy(flags.l) + (has_minutes ? ':' + flags.M : string_type());
        string_type const g = algo::trim_left_copy(flags.l);
        string_type const j = algo::trim_left_copy_if(flags.d, algo::is_any_of("0"));
        string_type const z = algo::trim_left_copy_if(flags.j, algo::is_any_of("0"));
        string_type const n = algo::trim_left_copy_if(flags.m, algo::is_any_of("0"));
        string_type const u = algo::trim_left_copy_if(flags.f, algo::is_any_of("."));
        string_type const r = flags.a + ',' + ' ' + flags.d + ' ' + flags.b + ' ' + flags.Y + ' ' + flags.T;
        string_type const G = algo::trim_left_copy(flags.k);
        string_type const U = traits_type::to_string(to_time_t(datetime));
        string_type const P = is_midnight ? traits_type::literal("midnight") :
                              is_noon     ? traits_type::literal("noon")     : (f + ' ' + a);

        // TODO: This might not be UTF8-safe; consider using a utf8_iterator.
        BOOST_FOREACH(char_type const _, format) {
            switch (_) {
            case char_type('a'): stream << a;        break;
            case char_type('A'): stream << flags.p;  break;
            case char_type('b'): stream << b;        break;
            case char_type('B'): stream << "";       break; // NOTE: "Not implemented" per spec.
            case char_type('c'): stream << c;        break;
            case char_type('d'): stream << flags.d;  break;
            case char_type('D'): stream << flags.a;  break;
            case char_type('e'): stream << flags.z;  break; // TODO: Ignored with ptimes.
            case char_type('E'): stream << flags.B;  break; // TODO: Make locale-aware.
            case char_type('f'): stream << f;        break;
            case char_type('F'): stream << flags.B;  break;
            case char_type('g'): stream << g;        break;
            case char_type('G'): stream << G;        break;
            case char_type('h'): stream << flags.I;  break;
            case char_type('H'): stream << flags.H;  break;
            case char_type('i'): stream << flags.M;  break;
            case char_type('I'): stream << "";       break; // TODO: Implement.
            case char_type('j'): stream << j;        break;
            case char_type('l'): stream << flags.A;  break;
            case char_type('L'): stream << "";       break; // TODO: Implement.
            case char_type('m'): stream << flags.m;  break;
            case char_type('M'): stream << flags.b;  break;
            case char_type('n'): stream << n;        break;
            case char_type('N'): stream << flags.b;  break; // TODO: Use A.P. style.
            case char_type('o'): stream << flags.G;  break;
            case char_type('O'): stream << "";       break; // TODO: Implement.
            case char_type('P'): stream << P;        break;
            case char_type('r'): stream << r;        break; // TODO: Include non-UTC timezones.
            case char_type('s'): stream << flags.S;  break;
            case char_type('S'): stream << "";       break; // TODO: Implement.
            case char_type('t'): stream << "";       break; // TODO: Implement.
            case char_type('T'): stream << "";       break; // TODO: Implement.
            case char_type('u'): stream << u;        break;
            case char_type('U'): stream << U;        break;
            case char_type('w'): stream << flags.w;  break;
            case char_type('W'): stream << "";       break; // TODO: Like %V but without leading zeros.
            case char_type('y'): stream << flags.y;  break;
            case char_type('Y'): stream << flags.Y;  break;
            case char_type('z'): stream << z;        break;
            case char_type('Z'): stream << "";       break; // TODO: Implement.
            default: stream << _;
            }
        }

        BOOST_ASSERT(stream);
        return stream.str();
    }

  private:

    inline static std::time_t to_time_t(datetime_type datetime) {
        duration_type const duration = datetime - datetime_type(date_type(1970, 1, 1));
        return static_cast<std::time_t>(duration.seconds());
    }

  public:

    // TODO: Proper, localizable formatting.
    static string_type format_duration( options_type  const& options
                                      , duration_type const& duration
                                      ) {
        BOOST_STATIC_CONSTANT(size_type, N = 6);

        static size_type const seconds[N] = { 60 * 60 * 24 * 365
                                            , 60 * 60 * 24 * 30
                                            , 60 * 60 * 24 * 7
                                            , 60 * 60 * 24
                                            , 60 * 60
                                            , 60
                                            };
        static string_type const units[N] = { traits_type::literal("year")
                                            , traits_type::literal("month")
                                            , traits_type::literal("week")
                                            , traits_type::literal("day")
                                            , traits_type::literal("hour")
                                            , traits_type::literal("minute")
                                            };

        if (duration.is_negative()) {
            return formatter::pluralize_unit(0, units[N - 1], options);
        }

        string_type result;
        size_type const total = duration.total_seconds();
        size_type count = 0, i = 0;

        for (; i < N; ++i) {
            if ((count = total / seconds[i])) {
                break;
            }
        }

        result += formatter::pluralize_unit(count, units[i], options);

        if (i + 1 < N) {
            if ((count = (total - (seconds[i] * count)) / seconds[i + 1])) {
                result += traits_type::literal(", ") + pluralize_unit(count, units[i + 1], options);
            }
        }

        return result;
    }

  private:

    /*inline static string_type nonbreaking(string_type const& s) {
        return boost::algorithm::replace_all_copy(s, traits_type::literal(" "), options.nonbreaking_space);
    }*/

    inline static string_type pluralize_unit( size_type    const  n
                                            , string_type  const& s
                                            , options_type const& options
                                            ) {
        string_type const suffix = n == 1 ? string_type() : traits_type::literal("s");
        return traits_type::to_string(n) + options.nonbreaking_space + s + suffix;
    }
};

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_FORMATTER_HPP_INCLUDED
