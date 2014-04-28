//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_TRANSFORMER_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_TRANSFORMER_HPP_INCLUDED

#include <string>
#include <sstream>
#include <cctype>

#include <boost/foreach.hpp>

#include <boost/algorithm/string/classification.hpp>

namespace ajg {
namespace synth {
namespace detail {

template <class String>
struct transformer {

    typedef transformer                                                         transformer_type;
    typedef String                                                              string_type;

    typedef bool                                                                boolean_type;
    typedef typename string_type::value_type                                    char_type;
    typedef typename string_type::size_type                                     size_type;
    typedef std::basic_ostringstream<char_type>                                 sstream_type;

//
// uri_encode
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type uri_encode(string_type const& string) {
        sstream_type ss;

        BOOST_FOREACH(char_type const c, string) {
            boolean_type const allowed = (std::isalnum)(c) || c == '_' || c == '-' || c == '.' || c == '/';
            allowed ? ss << c : ss << "%" << to_hex(c, 2);
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
            allowed ? ss << c : ss << "%" << to_hex(c, 2);
        }

        BOOST_ASSERT(ss);
        return ss.str();
    }

//
// escape_controls
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type escape_controls(string_type const& string) {
        sstream_type ss;

        BOOST_FOREACH(char_type const c, string) {
            boolean_type const allowed = c >= 32;
            allowed ? ss << c : ss << "\\x" << to_hex(c, 2);
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
            default: ascii ? ss << "&#x" << to_hex(c, 4) : ss << c;
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
// to_hex
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_type to_hex( char_type    const c
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

#endif // AJG_SYNTH_DETAIL_TRANSFORMER_HPP_INCLUDED
