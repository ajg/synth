//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DETAIL_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DETAIL_HPP_INCLUDED

#include <ajg/synth/config.hpp>

#include <map>
#include <ctime>
#include <vector>
#include <utility>
#include <algorithm>

#include <boost/random.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include <boost/xpressive/basic_regex.hpp>
#include <boost/xpressive/match_results.hpp>
#include <boost/xpressive/regex_actions.hpp>
#include <boost/xpressive/regex_compiler.hpp>
#include <boost/xpressive/regex_algorithms.hpp>
#include <boost/xpressive/regex_primitives.hpp>

#include <boost/type_traits/is_integral.hpp>

#include <boost/iterator/filter_iterator.hpp>

#include <boost/algorithm/string/classification.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/logical/not.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/arithmetic/mod.hpp>
#include <boost/preprocessor/control/expr_if.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_product.hpp>

#include <ajg/synth/detail.hpp>
#include <ajg/synth/exceptions.hpp>

// TODO: Move more of this stuff to ../detail.hpp.

namespace ajg {
namespace synth {
namespace engines {

using boost::xpressive::_;
using boost::xpressive::_b;
using boost::xpressive::_d;
using boost::xpressive::_ln;
using boost::xpressive::_n;
using boost::xpressive::_s;
using boost::xpressive::_w;
using boost::xpressive::as_xpr;
using boost::xpressive::s1;
using boost::xpressive::s2;

namespace algo = boost::algorithm;
namespace x    = boost::xpressive;

namespace detail {

//
// to_hex
////////////////////////////////////////////////////////////////////////////////////////////////////

template <std::size_t Width, class Char>
inline static std::basic_string<Char> to_hex(Char const c, bool lowercase = false) {
    // TODO: Ensure that given the width, the character passed in won't overflow as a number.
    // BOOST_STATIC_ASSERT(sizeof(Char) ... Width);
    std::basic_ostringstream<Char> stream;
    stream << (lowercase ? std::nouppercase : std::uppercase);
    stream << std::hex << std::setw(Width) << std::setfill(Char('0'));
    stream << static_cast<std::size_t>(c);
    BOOST_ASSERT(stream);
    return stream.str();
}

//
// url_encode
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline String uri_encode(String const& string) {
    typedef typename String::value_type char_type;
    std::basic_ostringstream<char_type> stream;

    BOOST_FOREACH(char_type const c, string) {
        bool const allowed = std::isalnum(c) || c == '_' || c == '-' || c == '.' || c == '/';
        allowed ? stream << c : stream << "%" << to_hex<2>(c);
    }

    BOOST_ASSERT(stream);
    return stream.str();
}

//
// iri_encode
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline String iri_encode(String const& string) {
    typedef typename String::value_type char_type;
    std::basic_ostringstream<char_type> stream;

    BOOST_FOREACH(char_type const c, string) {
        bool const allowed = std::isalnum(c) || boost::algorithm::is_any_of("/#%[]=:;$&()+,!?")(c);
        allowed ? stream << c : stream << "%" << to_hex<2>(c);
    }

    BOOST_ASSERT(stream);
    return stream.str();
}

//
// escape_controls
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline String escape_controls(String const& string) {
    typedef typename String::value_type char_type;
    std::basic_ostringstream<char_type> stream;

    BOOST_FOREACH(char_type const c, string) {
        bool const allowed = c >= 32;
        allowed ? stream << c : stream << "\\x" << to_hex<2>(c);
    }

    BOOST_ASSERT(stream);
    return stream.str();
}

//
// escape_entities
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline String escape_entities(String const& string, bool const ascii = false) {
    typedef typename String::size_type  size_type;
    typedef typename String::value_type char_type;
    std::basic_ostringstream<char_type> stream;

    BOOST_FOREACH(char_type const c, string) {
        switch (c) {
            case char_type('<'):  stream << "&lt;";   break;
            case char_type('>'):  stream << "&gt;";   break;
            case char_type('&'):  stream << "&amp;";  break;
            case char_type('"'):  stream << "&quot;"; break;
            case char_type('\''): stream << "&apos;"; break;
            default: ascii ? stream << "&#x" << to_hex<4>(c) : stream << c;
        }
    }

    BOOST_ASSERT(stream);
    return stream.str();
}

//
// quote:
//     Can handle "string" or 'string'.
//     TODO: Quote sequences (\n, ...), etc.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline String quote(String const& string, typename String::value_type quotation) {
    typedef typename String::value_type char_type;
    std::basic_ostringstream<char_type> stream;
    stream << quotation;

    BOOST_FOREACH(char_type const c, string) {
        bool const allowed = c != quotation;
        allowed ? stream << c : stream << "\\" << quotation;
    }

    stream << quotation;
    BOOST_ASSERT(stream);
    return stream.str();
}

//
// unquote:
//     Can handle "string" or 'string'.
//     TODO: Unquote sequences (\n, ...), etc.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline String unquote(String const& string) {
    BOOST_ASSERT(string.size() >= 2);
    BOOST_ASSERT(string[string.size() - 1] == string[0]);
    return string.substr(1, string.size() - 2);
}

//
// is:
//     Returns whether the match and regex share regex_ids.
//     TODO: Move to base_engine::kernel
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Iterator>
inline bool is(x::match_results<Iterator> const& match, x::basic_regex<Iterator> const& regex) {
    return match.regex_id() == regex.regex_id();
}

//
// unnest
//     TODO: Move to base_engine::kernel
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Match>
inline Match const& unnest(Match const& match) {
    BOOST_ASSERT(match);
    BOOST_ASSERT(match.size() >= 1);
    return *match.nested_results().begin();
}

//
// select_nested
//     TODO: Move to base_engine::kernel
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Match, class Regex>
inline std::pair
        < boost::filter_iterator<x::regex_id_filter_predicate<typename Regex::iterator_type>, typename Match::nested_results_type::const_iterator>
        , boost::filter_iterator<x::regex_id_filter_predicate<typename Regex::iterator_type>, typename Match::nested_results_type::const_iterator>
        >
select_nested(Match const& match, Regex const& regex) {
    typename Match::nested_results_type::const_iterator begin(match.nested_results().begin());
    typename Match::nested_results_type::const_iterator end(match.nested_results().end());
    x::regex_id_filter_predicate<typename Regex::iterator_type> predicate(regex.regex_id());
    return std::make_pair( boost::make_filter_iterator(predicate, begin, end)
                         , boost::make_filter_iterator(predicate, end,   end)
                         );
}

//
// insensitive_less:
//     Case-insensitive version of std::less<T>.
//     TODO: Move to synth/detail.hpp or value_traits/value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
struct insensitive_less : std::binary_function<T, T, bool> {
  public:

    bool operator ()(T const& t1, T const& t2) const {
        return std::lexicographical_compare
            ( t1.begin(), t1.end()
            , t2.begin(), t2.end()
            , comparer()
            );
    }

  private:

    typedef typename T::value_type V;

    struct comparer : std::binary_function<V, V, bool> {
        bool operator ()(V const& v1, V const& v2) const {
            return std::tolower(v1) < std::tolower(v2);
        }
    };
};

//
// has_mapped_type
//     TODO: Move to synth/detail.hpp
////////////////////////////////////////////////////////////////////////////////////////////////////

struct one { char c[1]; };
struct two { char c[2]; };

template <class T> one has_mapped_type_(...);
template <class T> two has_mapped_type_(typename T::mapped_type const volatile *);

template <class T> struct has_mapped_type {
    BOOST_STATIC_CONSTANT(bool, value = sizeof(has_mapped_type_<T>(0)) == sizeof(two));
};

//
// find:
//     Uniform interface for mapped and non-mapped containers.
//     TODO: Move to synth/detail.hpp
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Needle, class Container>
inline boost::optional<typename Container::value_type> find
        ( Needle    const& needle
        , Container const& container
        , typename boost::disable_if<has_mapped_type<Container> >::type* = 0
        ) {
    typename Container::const_iterator const it = std::find(container.begin(), container.end(), needle);
    if (it == container.end()) return boost::none; else return *it;
}

template <class Container>
inline boost::optional<typename Container::mapped_type> find
        ( typename Container::key_type const& needle
        , Container                    const& container
        ) {
    typename Container::const_iterator const it = container.find(needle);
    if (it == container.end()) return boost::none; else return it->second;
}

//
// if_c
//     TODO: Move to synth/detail.hpp
////////////////////////////////////////////////////////////////////////////////////////////////////

template <bool C, class X, class Y> struct if_c { typedef X type; };
template <class X, class Y>         struct if_c<false, X, Y> { typedef Y type; };

//
// uniform_random_number_generator
//     FIXME: This is broken:
//         * the seed (and source) must be generated once.
//         * either the engine, kernel or options should hold a (mutable) instance of this.
//     TODO: Move to value_traits/value_behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////

template < class Output = int
         , class Source = boost::mt19937
         >
struct uniform_random_number_generator {
  private:

    template <class T>
    struct select_distribution : if_c< boost::is_integral<T>::value
                                     , boost::uniform_int<T>
                                     , boost::uniform_real<T>
                                     > {};

  public:

    typedef Output                                                              output_type;
    typedef Source                                                              source_type;
    typedef typename source_type::result_type                                   seed_type;
    typedef typename select_distribution<output_type>::type                     distribution_type;
    typedef typename distribution_type::input_type                              input_type;
    typedef typename distribution_type::result_type                             result_type;
    typedef boost::variate_generator<source_type, distribution_type>            generator_type;

  public:

    uniform_random_number_generator() {}

  public:

    result_type operator ()
            ( input_type const lower = (std::numeric_limits<Output>::min)()
            , input_type const upper = (std::numeric_limits<Output>::max)()
            ) const {
        seed_type   const seed(generate_seed());
        source_type const source(seed);
        BOOST_ASSERT(seed);

        distribution_type const distribution(lower, upper);
        generator_type generator(source, distribution);
        return generator();
    }

  private:

    inline static seed_type generate_seed() {
        // TODO: Use traits_type clock/time types.
        namespace pt = boost::posix_time;

        pt::ptime const epoch = pt::from_time_t(std::time_t(0));
        pt::ptime const now = pt::microsec_clock::local_time();
        return static_cast<seed_type>((now - epoch).total_microseconds());
    }
};

//
// random_int:
//     FIXME: Remove; there can be no such const instance since the internal state must mutate.
////////////////////////////////////////////////////////////////////////////////////////////////////

uniform_random_number_generator<int> const random_int;

}}}} // namespace ajg::synth::engines::detail

#endif // AJG_SYNTH_ENGINES_DETAIL_HPP_INCLUDED
