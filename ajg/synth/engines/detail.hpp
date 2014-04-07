//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

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

#include <boost/mpl/if.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/bool.hpp>

#include <boost/xpressive/basic_regex.hpp>
#include <boost/xpressive/match_results.hpp>

#include <boost/type_traits/is_integral.hpp>

#include <boost/iterator/filter_iterator.hpp>

#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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
#include <ajg/synth/engines/exceptions.hpp>

// TODO: Move more of this stuff to ../detail.hpp.

namespace ajg {
namespace synth {
namespace detail {

using boost::disable_if;
using boost::disable_if_c;
using boost::enable_if;
using boost::enable_if_c;
using boost::none;
using boost::optional;

namespace algorithm  = boost::algorithm;
namespace date_time  = boost::date_time;
namespace mpl        = boost::mpl;
namespace posix_time = boost::posix_time;
namespace xpressive  = boost::xpressive;

//
// [deprecated] string_literal:
//     Helper class to help widen literals on the spot when necessary.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class From, std::size_t Length>
struct string_literal {
    From const (&source)[Length];
    inline string_literal(From const (&source)[Length]) : source(source) {}

    // We define these to more easily interoperate
    // with the regex_* and algorithm::* functions.
    typedef From const* const_iterator;
    inline const_iterator begin() const { return source; }
    inline const_iterator end() const { return source + Length - 1; }

    template <class Char, class Traits, class Allocator>
    inline operator std::basic_string<Char, Traits, Allocator>() const {
        return std::basic_string<Char, Traits, Allocator>(source, source + Length - 1);
    }

    template <class Char, class Traits, class Allocator>
    inline bool operator ==(std::basic_string<Char, Traits, Allocator> const& that) const {
        return that.compare(*this) == 0;
    }

    template <class Char, class Traits, class Allocator>
    friend inline bool operator ==( std::basic_string<Char, Traits, Allocator> const& that
                                  , string_literal const& self
                                  ) {
        return that.compare(self) == 0;
    }

    template <class Char, class Traits, class Allocator>
    inline bool operator !=(std::basic_string<Char, Traits, Allocator> const& that) const {
        return that.compare(*this) != 0;
    }

    template <class Char, class Traits, class Allocator>
    friend inline bool operator !=( std::basic_string<Char, Traits, Allocator> const& that
                                  , string_literal const& self
                                  ) {
        return that.compare(self) != 0;
    }
};

//
// [deprecated] text:
//     Creates string_literal objects from native literals.
//     TODO: Replace remaining uses with traits_type::literal.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class From, std::size_t Length>
inline string_literal<From, Length> text(From const (&source)[Length]) {
    return string_literal<From, Length>(source);
}

//
// local_now
//     TODO: Offer a local_time::local_date_time version; e.g.
//           local_time::local_sec_clock::local_time(local_time::time_zone_ptr())
////////////////////////////////////////////////////////////////////////////////////////////////////

inline posix_time::ptime local_now() {
    return posix_time::second_clock::local_time();
}

//
// utc_now
////////////////////////////////////////////////////////////////////////////////////////////////////

inline posix_time::ptime utc_now() {
    return posix_time::second_clock::universal_time();
}

/*
//
// format_current_time
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline String format_current_time(String format, bool const autoprefix = true) {
    if (autoprefix) format = prefix_format_characters(format);
    typedef typename String::value_type char_type;
    typedef typename local_time::local_date_time time_type;
    typedef typename date_time::time_facet<time_type, char_type> facet_type;

    std::basic_ostringstream<char_type> stream;
    time_type t = local_time::local_sec_clock::
        local_time(local_time::time_zone_ptr());
    // The locale takes care of deleting this thing for us.
    // TODO: Figure out a way to allocate the facet on the stack.
    facet_type *const facet = new facet_type(format.c_str());
    stream.imbue(std::locale(stream.getloc(), facet));
    // Finally, stream out the time, properly formatted.
    return (stream << t), stream.str();
}
*/

//
// format_time
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String, class Time>
inline String format_time(String format, Time const& time) {
    typedef String                                               string_type;
    typedef Time                                                 time_type;
    typedef typename string_type::value_type                     char_type;
    typedef typename date_time::time_facet<time_type, char_type> facet_type;

    std::basic_ostringstream<char_type> stream;
    // The locale takes care of deleting this thing for us.
    // TODO: Figure out a way to allocate the facet on the stack.
    facet_type *const facet = new facet_type(format.c_str());
    stream.imbue(std::locale(stream.getloc(), facet));
    // Finally, stream out the time, properly formatted.
    return (stream << time), stream.str();
}

//
// format_size
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String, class Size>
inline String format_size(Size const size) {
    double bucket = 1;
    String unit;

         if (size >  (bucket = (std::pow)(2, 60.0))) unit = text("EB");
    else if (size >  (bucket = (std::pow)(2, 50.0))) unit = text("PB");
    else if (size >  (bucket = (std::pow)(2, 40.0))) unit = text("TB");
    else if (size >  (bucket = (std::pow)(2, 30.0))) unit = text("GB");
    else if (size >  (bucket = (std::pow)(2, 20.0))) unit = text("MB");
    else if (size >  (bucket = (std::pow)(2, 10.0))) unit = text("KB");
    else if (size >= (bucket = (std::pow)(2, 00.0))) unit = text("bytes");

    std::basic_ostringstream<typename String::value_type> stream;
    stream << std::fixed << std::setprecision(1);
    stream << (size / bucket) << ' ' << unit;

    BOOST_ASSERT(stream);
    return stream.str();
}

//
// to_hex
////////////////////////////////////////////////////////////////////////////////////////////////////

template <std::size_t Width, class Char>
inline static std::basic_string<Char> to_hex(Char const c) {
    // TODO: Ensure that given the width, the character passed in won't overflow as a number.
    // BOOST_STATIC_ASSERT(sizeof(Char) ... Width);
    std::basic_ostringstream<Char> stream;
    stream << std::hex << std::uppercase << std::setw(Width);
    stream << std::setfill(Char('0')) << static_cast<std::size_t>(c);
    BOOST_ASSERT(stream);
    return stream.str();
}

//
// url_encode
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline String uri_encode(String const& string) {
    String result;
    result.reserve(string.size()); // Assume no encodings.
    typedef typename String::value_type char_type;

    BOOST_FOREACH(char_type const c, string) {
        result += std::isalnum(c) || c == '_' || c == '-' || c == '.' || c == '/' ?
            String(1, c) : char_type('%') + to_hex<2>(c);
    }

    return result;
}

//
// iri_encode
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline String iri_encode(String const& string) {
    String result;
    result.reserve(string.size()); // Assume no encodings.
    typedef typename String::value_type char_type;

    BOOST_FOREACH(char_type const c, string) {
        result += std::isalnum(c) || algorithm::is_any_of("/#%[]=:;$&()+,!?")(c) ?
            String(1, c) : char_type('%') + to_hex<2>(c);
    }

    return result;
}

//
// escape_entities
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline String escape_entities(String const& string, bool const ascii = false) {
    typedef typename String::size_type size_type;
    typedef typename String::value_type char_type;

    if (size_type const escapes = std::count_if(string.begin(),
        string.end(), algorithm::is_any_of("<>&'\""))) {

        String result;
        BOOST_STATIC_CONSTANT(size_type, max_length = 6);
        result.reserve(string.size() + escapes * max_length);

        BOOST_FOREACH(char_type const c, string) {
            switch (c) {
                case char_type('<'):  result += text("&lt;");   break;
                case char_type('>'):  result += text("&gt;");   break;
                case char_type('&'):  result += text("&amp;");  break;
                case char_type('"'):  result += text("&quot;"); break;
                case char_type('\''): result += text("&apos;"); break;
                default: {
                    if (ascii) {
                        result += text("&#x");
                        result += to_hex<4>(c);
                    }
                    else {
                        result += c;
                    }
                }
            }
        }

        return result;
    }
    else {
        return string;
    }
}

//
// operator ==:
//     Provides a more readable way to compare match objects via regex_ids.
//     TODO: Rename to matches or similar to avoid confusion.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Iterator>
inline bool operator == ( xpressive::match_results<Iterator> const& match
                        , xpressive::basic_regex<Iterator>   const& regex
                        ) {
    return match.regex_id() == regex.regex_id();
}

//
// [deprecated] get_nested
////////////////////////////////////////////////////////////////////////////////////////////////////

template <std::size_t Index, class Match>
inline static Match const& get_nested(Match const& match) {
    BOOST_STATIC_ASSERT(Index != 0);
    std::size_t i = 0;

    // TODO: Use advance or the like.
    BOOST_FOREACH(Match const& nested, match.nested_results()) {
        if (++i == Index) return nested;
    }

    static const Match empty;
    return empty;
}

//
// unnest
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Match>
inline static Match const& unnest(Match const& match) {
    BOOST_ASSERT(match);
    BOOST_ASSERT(match.size() == 1);
    return *match.nested_results().begin();
}

//
// select_nested
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Match, class Regex>
inline std::pair
        < boost::filter_iterator<boost::xpressive::regex_id_filter_predicate<typename Regex::iterator_type>, typename Match::nested_results_type::const_iterator>
        , boost::filter_iterator<boost::xpressive::regex_id_filter_predicate<typename Regex::iterator_type>, typename Match::nested_results_type::const_iterator>
        >
select_nested(Match const& match, Regex const& regex) {
    typename Match::nested_results_type::const_iterator
        begin(match.nested_results().begin()),
        end(match.nested_results().end());
    boost::xpressive::regex_id_filter_predicate<typename Regex::iterator_type>
        predicate(regex.regex_id());
    return std::make_pair
        ( boost::make_filter_iterator(predicate, begin, end)
        , boost::make_filter_iterator(predicate, end,   end)
        );
}

//
// [deprecated] placeholders:
//     A symbolic way to refer to subresults within a match result object.
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace placeholders {

    BOOST_STATIC_CONSTANT(std::size_t, A = 1);
    BOOST_STATIC_CONSTANT(std::size_t, B = 2);
    BOOST_STATIC_CONSTANT(std::size_t, C = 3);
    BOOST_STATIC_CONSTANT(std::size_t, D = 4);
    BOOST_STATIC_CONSTANT(std::size_t, E = 5);
    BOOST_STATIC_CONSTANT(std::size_t, F = 6);
    BOOST_STATIC_CONSTANT(std::size_t, G = 7);
    BOOST_STATIC_CONSTANT(std::size_t, H = 8);
    BOOST_STATIC_CONSTANT(std::size_t, I = 9);

} // namespace placeholders

//
// insensitive_less:
//     Case-insensitive version of std::less<T>.
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
// uniform_random_number_generator
////////////////////////////////////////////////////////////////////////////////////////////////////

template < class Output = int
         , class Source = boost::mt19937
         >
struct uniform_random_number_generator {
  private:

    template <class T>
    struct select_distribution : mpl::identity<typename mpl::if_< boost::is_integral<T>
                                                                , boost::uniform_int<T>
                                                                , boost::uniform_real<T>
                                                                >::type> {};

  public:

    typedef Source                                                              source_type;
    typedef typename source_type::result_type                                   seed_type;
    typedef typename select_distribution<Output>::type                          distribution_type;
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
        seed_type const seed(generate_seed());
        source_type const source(seed);
        BOOST_ASSERT(seed);

        distribution_type const distribution(lower, upper);
        generator_type generator(source, distribution);
        return generator();
    }

  private:

    inline static seed_type generate_seed() {
        using namespace posix_time;

        ptime const epoch = from_time_t(std::time_t(0));
        ptime const now = microsec_clock::local_time();
        return static_cast<seed_type>((now - epoch).total_microseconds());
    }
};

//
// random_int,
// random_double:
//     Precreated convenience objects for random int and double generation.
////////////////////////////////////////////////////////////////////////////////////////////////////

uniform_random_number_generator<int>    const random_int;
uniform_random_number_generator<double> const random_double;

//
// find_*:
//     Set of convenience functions to locate specific items within collections.
//     TODO: Deprecate the esoteric ones and rename the rest get_* or such.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Needle, class Haystack, class Else>
inline typename Haystack::const_iterator find_or
        ( Needle   const& needle
        , Haystack const& haystack
        , Else     const& else_
        ) {
    typename Haystack::const_iterator const it = std::find(haystack.begin(), haystack.end(), needle);
    return it == haystack.end() ? else_ : it;
}

template <class Needle, class Haystack>
inline optional<typename Haystack::value_type const&> find_value
        ( Needle   const& needle
        , Haystack const& haystack
        ) {
    typename Haystack::const_iterator const it = std::find(haystack.begin(), haystack.end(), needle);
    if (it == haystack.end()) return none; else return *it;
}

template <class Container>
inline optional<typename Container::mapped_type const/*&*/> find_mapped_value
        ( typename Container::key_type const& needle
        , Container                    const& container
        ) {
    typename Container::const_iterator const it = container.find(needle);
    if (it == container.end()) return none; else return it->second;
}

template <class Needle, class Key, class Value, class Compare, class Allocator>
inline optional<Value const&> find_value
        ( Needle                                   const& needle
        , std::map<Key, Value, Compare, Allocator> const& map
        ) {
    typedef std::map<Key, Value, Compare, Allocator> map_type;
    typename map_type::const_iterator const it = map.find(needle);
    if (it == map.end()) return none; else return it->second;
}

//
// is_integer:
//     Determines whether a floating-point number is an integer.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class FloatingPoint>
inline static bool is_integer(FloatingPoint const& fp) {
    FloatingPoint integer_part;
    return std::modf(fp, &integer_part) == FloatingPoint(0.0);
}

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_ENGINES_DETAIL_HPP_INCLUDED
