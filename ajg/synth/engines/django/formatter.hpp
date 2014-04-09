//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_FORMATTER_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_FORMATTER_HPP_INCLUDED

#include <map>
#include <sstream>

#include <boost/assign/list_of.hpp>

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
    typedef typename options_type::datetime_type                                datetime_type;
    typedef typename options_type::duration_type                                duration_type;
    typedef typename options_type::string_type                                  string_type;
    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::range_type                                   range_type;
    typedef typename options_type::sequence_type                                sequence_type;
    typedef typename options_type::arguments_type                               arguments_type;
    typedef typename options_type::context_type                                 context_type;

  public:

    static string_type format_datetime( options_type  const& options
                                      , string_type   const& format
                                      , datetime_type const& datetime
                                      ) {
        typedef std::map<char_type, string_type>            transliterations_type;
        typedef typename transliterations_type::value_type  transliteration_type;

        static transliterations_type const transliterations = boost::assign::list_of<transliteration_type>
            (char_type('%'), traits_type::literal("%%"))
            (char_type('a'), traits_type::literal(AJG_SYNTH_IF_WINDOWS("", "%P"))) // TODO: Periods; implement on Windows.
            (char_type('A'), traits_type::literal("%p"))
            (char_type('b'), traits_type::literal("%b")) // TODO: Lowercase
            (char_type('B'), traits_type::literal(""))   // "Not implemented" per spec.
            (char_type('c'), traits_type::literal("%Y-%m-%dT%H:%M:%S%z"))
            (char_type('d'), traits_type::literal("%d"))
            (char_type('D'), traits_type::literal("%a"))
            (char_type('e'), traits_type::literal("%z"))    // TODO: Ignored with ptimes
            (char_type('E'), traits_type::literal("%B"))    // TODO: Make locale-aware
            (char_type('f'), traits_type::literal("%l:%M")) // TODO: No leading blank, no zero minutes
            (char_type('F'), traits_type::literal("%B"))
            (char_type('g'), traits_type::literal("%l"))    // TODO: No leading blank
            (char_type('G'), traits_type::literal("%k"))    // TODO: No leading blank
            (char_type('h'), traits_type::literal("%I"))
            (char_type('H'), traits_type::literal("%H"))
            (char_type('i'), traits_type::literal("%M"))
            (char_type('I'), traits_type::literal(""))   // TODO: Implement
            (char_type('j'), traits_type::literal(AJG_SYNTH_IF_WINDOWS("", "%e"))) // TODO: No leading blank; implement on Windows.
            (char_type('l'), traits_type::literal("%A"))
            (char_type('L'), traits_type::literal(""))   // TODO: Implement
            (char_type('m'), traits_type::literal("%m"))
            (char_type('M'), traits_type::literal("%b"))
            (char_type('n'), traits_type::literal("%m")) // TODO: No leading zeros
            (char_type('N'), traits_type::literal("%b")) // TODO: Abbreviations/periods
            (char_type('o'), traits_type::literal("%G"))
            (char_type('O'), traits_type::literal(""))   // TODO: Implement
            (char_type('P'), traits_type::literal(AJG_SYNTH_IF_WINDOWS("", "%r"))) // TODO: Periods, no zero minutes, "midnight"/"noon"; implement on Windows.
            (char_type('r'), traits_type::literal("%a, %d %b %Y %T %z"))
            (char_type('s'), traits_type::literal("%S"))
            (char_type('S'), traits_type::literal(""))   // TODO: Implement
            (char_type('t'), traits_type::literal(""))   // TODO: Implement
            (char_type('T'), traits_type::literal(""))   // TODO: Implement
            (char_type('u'), traits_type::literal("%f")) // TODO: No leading period
            (char_type('U'), traits_type::literal(""))   // TODO: Implement
            (char_type('w'), traits_type::literal("%w"))
            (char_type('W'), traits_type::literal(AJG_SYNTH_IF_WINDOWS("", "%V"))) // TODO: No leading zeros; implement on Windows.
            (char_type('y'), traits_type::literal("%y"))
            (char_type('Y'), traits_type::literal("%Y"))
            (char_type('z'), traits_type::literal("%j")) // TODO: No leading zeros
            (char_type('Z'), traits_type::literal(""))   // TODO: Implement
            ;

        std::basic_ostringstream<char_type> stream;
        typename options_type::formats_type::const_iterator const it = options.formats.find(format);
        string_type const original = it == options.formats.end() ? format : it->second;

        // TODO: This might not be UTF8-safe; consider using a utf8_iterator.
        BOOST_FOREACH(char_type const c, original) {
            stream << detail::find_mapped_value(c, transliterations).get_value_or(string_type(1, c));
        }

        return synth::detail::format_time<string_type>(stream.str(), datetime);
    }

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
