//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_TEXT_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_TEXT_HPP_INCLUDED

#include <cctype>
#include <string>
#include <locale>
#include <sstream>
#include <iomanip>

#include <boost/foreach.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <ajg/synth/exceptions.hpp>

namespace ajg {
namespace synth {
namespace detail {
namespace {
namespace algo = boost::algorithm;
} // namespace

//
// text:
//     Utilities for non-configurable and trait-independent textual examination and manipulation.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
struct text {
  public:

    typedef String                                                              string_type;
    typedef bool                                                                boolean_type;
    typedef typename string_type::value_type                                    char_type;
    typedef typename string_type::size_type                                     size_type;
    typedef std::basic_ostringstream<char_type>                                 sstream_type;

  public:

///
/// literal:
////////////////////////////////////////////////////////////////////////////////////////////////////

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

    typedef char_type Char; // To improve the formatting below.

    template <class C> inline static std::basic_string<char> narrow(std::basic_string<C> const& s) { return transcode<char, C>(s); }
    template <class C> inline static std::basic_string<Char> widen (std::basic_string<C> const& s) { return transcode<Char, C>(s); }

    inline static std::basic_string<char> const& narrow(std::basic_string<char> const& s) { return s; }
    inline static std::basic_string<Char> const& widen (std::basic_string<Char> const& s) { return s; }


//
// uri_encode
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type uri_encode(string_type const& string) {
        sstream_type ss;

        BOOST_FOREACH(char_type const c, string) {
            boolean_type const allowed = (std::isalnum)(c) || c == '_' || c == '-' || c == '.' || c == '/';
            allowed ? ss << c : ss << "%" << hexize(c, 2);
        }

        BOOST_ASSERT(ss);
        return ss.str();
    }

//
// iri_encode
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type iri_encode(string_type const& string) {
        sstream_type ss;

        BOOST_FOREACH(char_type const c, string) {
            boolean_type const allowed = (std::isalnum)(c) || boost::algorithm::is_any_of("/#%[]=:;$&()+,!?")(c);
            allowed ? ss << c : ss << "%" << hexize(c, 2);
        }

        BOOST_ASSERT(ss);
        return ss.str();
    }

//
// escape_controls
//     XXX: Should this actually be equivalent to quote()?
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type escape_controls(string_type const& string) {
        sstream_type ss;

        BOOST_FOREACH(char_type const c, string) {
            boolean_type const allowed = c >= 32;
            allowed ? ss << c : ss << "\\x" << hexize(c, 2);
        }

        BOOST_ASSERT(ss);
        return ss.str();
    }

//
// escape_entities
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type escape_entities( string_type  const& string
                                             , boolean_type const  ascii = false
                                             ) {
        sstream_type ss;

        BOOST_FOREACH(char_type const c, string) {
            switch (c) {
            case char_type('<'):  ss << "&lt;";   break;
            case char_type('>'):  ss << "&gt;";   break;
            case char_type('&'):  ss << "&amp;";  break;
            case char_type('"'):  ss << "&quot;"; break;
            case char_type('\''): ss << "&apos;"; break;
            default: ascii ? ss << "&#x" << hexize(c, 4) : ss << c;
            }
        }

        BOOST_ASSERT(ss);
        return ss.str();
    }

//
// quote:
//     Can handle "string" or 'string'.
//     TODO: Quote sequences (\n, ...), etc.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type quote( string_type const& string
                                   , char_type   const  quotation
                                   ) {
        sstream_type ss;
        ss << quotation;

        BOOST_FOREACH(char_type const c, string) {
            boolean_type const allowed = c != quotation;
            allowed ? ss << c : ss << "\\" << quotation;
        }

        ss << quotation;
        BOOST_ASSERT(ss);
        return ss.str();
    }

//
// unquote:
//     Can handle "string" or 'string'.
//     TODO: Unquote sequences (\n, ...), etc.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type unquote(string_type const& string) {
        BOOST_ASSERT(string.size() >= 2);
        BOOST_ASSERT(string[string.size() - 1] == string[0]);
        return string.substr(1, string.size() - 2);
    }

//
// stringize
////////////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
    inline static string_type stringize(T const& t) {
        sstream_type ss;
        ss << t;
        BOOST_ASSERT(ss);
        return ss.str();
    }

//
// join
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type join(std::vector<string_type> const& strings, string_type const& delimiter) {
        return algo::join(strings, delimiter);
    }

//
// split
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static std::vector<string_type> split( string_type const& string
                                                , string_type const& delimiters
                                                , size_type   const  hint = 0
                                                ) {
        std::vector<string_type> result;
        if (hint > 0) result.reserve(hint);
        algo::split(result, string, algo::is_any_of(delimiters));
        return result;
    }

//
// space
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static std::vector<string_type> space( string_type const& string
                                                , size_type   const  hint = 0
                                                ) {
        std::vector<string_type> result;
        if (hint > 0) result.reserve(hint);
        int (*predicate)(int) = std::isspace;
        string_type const s = algo::trim_copy(string); // Prevent empties.
        algo::split(result, s, predicate);
        return result;
    }

//
// erase
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type remove(string_type const& string, string_type const& what) {
        return algo::erase_all_copy(string, what);
    }

//
// replace
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type replace(string_type const& string, string_type const& what, string_type const& with) {
        return algo::replace_all_copy(string, what, with);
    }

//
// lower
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static boolean_type begins_with(string_type const& s, string_type const& prefix) {
        return algo::starts_with(s, prefix);
    }

//
// upper
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static boolean_type ends_with(string_type const& s, string_type const& suffix) {
        return algo::ends_with(s, suffix);
    }

//
// lower
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type lower(string_type const& s) {
        return algo::to_lower_copy(s);
    }

//
// upper
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type upper(string_type const& s) {
        return algo::to_upper_copy(s);
    }

//
// strip
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type strip(string_type const& s) {
        return algo::trim_copy(s);
    }

//
// strip_left
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type strip_left(string_type const& s) {
        return algo::trim_left_copy(s);
    }

//
// strip_right
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type strip_right(string_type const& s) {
        return algo::trim_right_copy(s);
    }

//
// trim
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type trim(string_type const& s, string_type const& characters) {
        return algo::trim_copy_if(s, algo::is_any_of(characters));
    }

//
// trim_left
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type trim_left(string_type const& s, string_type const& characters) {
        return algo::trim_left_copy_if(s, algo::is_any_of(characters));
    }

//
// trim_right
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type trim_right(string_type const& s, string_type const& characters) {
        return algo::trim_right_copy_if(s, algo::is_any_of(characters));
    }

//
// trim_leading_zeros
//     Similar to trim_left, except will return a one-zero string in place of the empty string.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type trim_leading_zeros(string_type const& s) {
        static string_type const zero(1, char_type('0'));
        string_type const trimmed = trim_left(s, zero);
        return trimmed.empty() ? zero : trimmed;
    }

//
// digitize
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type digitize(size_type const n, size_type const width) {
        if (width != 2) {
            AJG_SYNTH_THROW(not_implemented("width != 2"));
        }
        else if (n >= 100) {
            AJG_SYNTH_THROW(std::out_of_range("n"));
        }
        else if (n < 10) {
            return char_type('0') + stringize(n);
        }
        else {
            return stringize(n);
        }
    }

//
// hexize
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type hexize( char_type    const c
                                    , size_type    const width
                                    , boolean_type const lowercase = false
                                    ) {
        sstream_type ss;
        ss << (lowercase ? std::nouppercase : std::uppercase);
        ss << std::hex << std::setw(width) << std::setfill(static_cast<char_type>('0'));
        ss << static_cast<size_type>(c);
        BOOST_ASSERT(ss);
        return ss.str();
    }
};

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_TEXT_HPP_INCLUDED
