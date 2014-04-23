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
namespace engines {
namespace detail {

#define AJG_SYNTH_EMPTY_   // Nothing.
#define AJG_SYNTH_TEMPLATE AJG_SYNTH_IF_MSVC(AJG_SYNTH_EMPTY_, template)

using boost::none;
using boost::optional;

namespace date_time  = boost::date_time;
namespace mpl        = boost::mpl;
namespace posix_time = boost::posix_time;
namespace xpressive  = boost::xpressive;

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
    // TODO: Figure out a way to allocate the facet on the stack or reuse it.
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
    typedef typename String::value_type char_type;
    std::basic_ostringstream<char_type> stream;
    stream << std::fixed << std::setprecision(1);

    double bucket = 1;
    String unit;

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
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Iterator>
inline bool is(xpressive::match_results<Iterator> const& match, xpressive::basic_regex<Iterator> const& regex) {
    return match.regex_id() == regex.regex_id();
}

//
// unnest
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Match>
inline Match const& unnest(Match const& match) {
    BOOST_ASSERT(match);
    BOOST_ASSERT(match.size() >= 1);
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
// has_mapped_type
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
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Needle, class Container>
inline optional<typename Container::value_type> find
        ( Needle    const& needle
        , Container const& container
        , typename boost::disable_if<has_mapped_type<Container> >::type* = 0
        ) {
    typename Container::const_iterator const it = std::find(container.begin(), container.end(), needle);
    if (it == container.end()) return none; else return *it;
}

template <class Container>
inline optional<typename Container::mapped_type> find
        ( typename Container::key_type const& needle
        , Container                    const& container
        ) {
    typename Container::const_iterator const it = container.find(needle);
    if (it == container.end()) return none; else return it->second;
}

}}}} // namespace ajg::synth::engines::detail

#endif // AJG_SYNTH_ENGINES_DETAIL_HPP_INCLUDED
