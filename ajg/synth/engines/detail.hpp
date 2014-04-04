//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifdef _MSC_VER
#    pragma once
#endif

#ifndef AJG_SYNTH_ENGINES_DETAIL_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DETAIL_HPP_INCLUDED

#include <ajg/synth/config.hpp>
#include <ajg/synth/vector.hpp>

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

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>

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
namespace fusion     = boost::fusion;
namespace mpl        = boost::mpl;
namespace posix_time = boost::posix_time;
namespace xpressive  = boost::xpressive;

//
// [deprecated] lit:
//     String literal helper.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class To, class From, std::size_t Length>
inline std::basic_string<To> lit(From const (&source)[Length]) {
    return std::basic_string<To>(source, source + Length - 1);
}

//
// string_literal:
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
// AJG_CASE_OF, AJG_CASE_OF_ELSE
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_TERNARY_OPERATOR(r, value, elem) \
    (value == BOOST_PP_TUPLE_ELEM(2, 0, elem)) ? \
        BOOST_PP_TUPLE_ELEM(2, 1, elem) :

#define AJG_CASE_OF_ELSE(value, cases, default_) \
    (BOOST_PP_SEQ_FOR_EACH(AJG_TERNARY_OPERATOR, value, cases) (default_))

// TODO: Figure out how to use AJG_UNREACHABLE but without
//       triggering warning C4702; or, how to silence the warning.

#define AJG_CASE_OF(value, cases) \
    AJG_CASE_OF_ELSE(value, cases, ajg::synth::detail::unreachable())
        // (BOOST_ASSERT(0), throw 0, ajg::synth::detail::unreachable(value)))

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
// AJG_VECTOR_0_IF
////////////////////////////////////////////////////////////////////////////////////////////////////

#if (BOOST_VERSION > 104000) // 1.40+
    #define AJG_VECTOR_0_IF(n, symbol) symbol
#else
    #define AJG_VECTOR_0_IF(n, symbol) BOOST_PP_EXPR_IF(n, symbol)
#endif

//
// apply_at:
//     Provides a way to index a compile-time Sequence at runtime
//     using a suitable Functor, which is called exactly once. It is
//     similar to at_c, but allows the index to be specified at runtime.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <std::size_t Size>
struct apply_at;

#define AJG_CASE(z, n, nil) case n: return functor(fusion::at_c<n>(sequence));

#define AJG_APPLY_AT(z, n, nil) \
    template <> \
    struct apply_at <n> { \
        template <class Sequence, class Functor> \
        inline static typename Functor::result_type \
                fn( std::size_t const  index \
                  , Sequence    const& sequence \
                  , Functor     const& functor \
                  ) { \
            if (index >= n) { \
                throw_exception(std::out_of_range("index")); \
            } \
            \
            switch (index) { \
                BOOST_PP_REPEAT(n, AJG_CASE, nil) \
                default: AJG_UNREACHABLE; \
            } \
        } \
    };

BOOST_PP_REPEAT(AJG_SYNTH_SEQUENCE_LIMIT, AJG_APPLY_AT, nil)
#undef AJG_APPLY_AT
#undef AJG_CASE

//
// create_definitions
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Engine, class Sequence, std::size_t Size>
struct create_definitions;

#define DEFINITION(z, n, nil) \
    BOOST_PP_COMMA_IF(n) typename fusion::result_of:: \
        value_at_c<Sequence, n>::type::template \
            definition<Engine>

#define CREATE_DEFINITIONS(z, n, nil) \
    template <class Engine, class Sequence> \
    struct create_definitions <Engine, Sequence, n> { \
        typedef typename fusion::AJG_VECTOR_0_IF(n, template) BOOST_PP_CAT(vector, n) \
            AJG_VECTOR_0_IF(n, <) BOOST_PP_REPEAT(n, DEFINITION, nil) \
            AJG_VECTOR_0_IF(n, >) \
        type; \
    };

BOOST_PP_REPEAT(AJG_SYNTH_SEQUENCE_LIMIT, CREATE_DEFINITIONS, nil)
#undef CREATE_DEFINITIONS
#undef DEFINITION

//
// create_definitions_extended
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Engine, class Sequence, std::size_t Size>
struct create_definitions_extended;

#define DEFINITION(z, n, nil) \
    BOOST_PP_COMMA_IF(n) typename fusion::result_of:: \
        value_at_c<Sequence, n>::type::template \
            definition \
                < typename Engine::char_type \
                , typename Engine::regex_type \
                , typename Engine::string_type \
                , typename Engine::context_type \
                , typename Engine::value_type \
                , typename Engine::size_type \
                , typename Engine::match_type \
                , typename Engine::this_type \
                , typename Engine::options_type \
                , typename Engine::sequence_type \
                >

#define CREATE_DEFINITIONS(z, n, nil) \
    template <class Engine, class Sequence> \
    struct create_definitions_extended <Engine, Sequence, n> { \
        typedef typename fusion::AJG_VECTOR_0_IF(n, template) BOOST_PP_CAT(vector, n) \
            AJG_VECTOR_0_IF(n, <) BOOST_PP_REPEAT(n, DEFINITION, nil) \
            AJG_VECTOR_0_IF(n, >) \
        type; \
    };


BOOST_PP_REPEAT(AJG_SYNTH_SEQUENCE_LIMIT, CREATE_DEFINITIONS, nil)
#undef CREATE_DEFINITIONS
#undef DEFINITION

#undef AJG_VECTOR_0_IF

//
// indexable_sequence
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Engine, class Sequence, class Key,
          template <class E, class S, std::size_t K> class Definer>
struct indexable_sequence {
  public:

    typedef Engine                engine_type;
    typedef Sequence              sequence_type;
    typedef Key                   key_type;
    typedef std::vector<key_type> index_type;
    typedef typename Sequence::size::value_type size_type;
    BOOST_STATIC_CONSTANT(size_type, size = Sequence::size::value);
    typedef typename Definer<Engine, Sequence, size>::type definition_type;

  public:

    definition_type definition;
    index_type      index;
};

//
// index_sequence
////////////////////////////////////////////////////////////////////////////////////////////////////

template < class Engine
         , class Indexable
         , Indexable Engine::*Sequence
         , std::size_t Size>
struct index_sequence;

#define PUSH_SYNTAX(z, n, nil) \
    typename Engine::regex_type const BOOST_PP_CAT(r, n) = \
        fusion::at_c<n>((engine.*Sequence).definition).syntax(engine); \
    (engine.*Sequence).index.push_back(BOOST_PP_CAT(r, n).regex_id());

#define ALTERNATIVES(z, n, nil) \
    BOOST_PP_IF(n, |, engine.tag =) BOOST_PP_CAT(r, n)

#define INDEX_SEQUENCE(z, n, nil) \
    template < class Engine \
             , class Indexable \
             , Indexable Engine::*Sequence> \
    struct index_sequence <Engine, Indexable, Sequence, n> { \
        Engine& engine; \
        index_sequence(Engine& engine) : engine(engine) { \
            (engine.*Sequence).index.reserve(n); \
            BOOST_PP_REPEAT(n, PUSH_SYNTAX, nil) \
            BOOST_PP_REPEAT(n, ALTERNATIVES, nil) BOOST_PP_EXPR_IF(n, ;) \
        } \
    };

BOOST_PP_REPEAT(AJG_SYNTH_SEQUENCE_LIMIT, INDEX_SEQUENCE, nil)
#undef INDEX_SEQUENCE
#undef ALTERNATIVES
#undef PUSH_SYNTAX

//
// set_furthest_iterator:
//     Function object that sets the iterator to the furthest, either itself or the submatch's end.
////////////////////////////////////////////////////////////////////////////////////////////////////

struct set_furthest_iterator {
    typedef void result_type;

    template <class Iterator, class Submatch>
    void operator()
            ( Iterator&       iterator
            , Submatch const& submatch
            ) const {
        iterator = (std::max)(iterator, submatch.second);
    }
};

//
// HAS_MEMBER_FUNCTION:
//     Adapted from Johannes Schaub (litb)'s.
//     NOTE: Apparently will not detect inherited methods.
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_DEFINE_METHOD_PREDICATE(name) \
    template <class T, class Signature> \
    struct BOOST_PP_CAT(base_has_, name) { \
        template <typename U, U> struct check; \
        template <typename V> static char (&f(check<Signature, &V::name>*))[1]; \
        template <typename> static char (&f(...))[2]; \
        static bool const value = sizeof(f<T>(0)) == 1; \
    }; \
    template <class T, class Signature> \
    struct BOOST_PP_CAT(has_, name) : \
        mpl::bool_<BOOST_PP_CAT(base_has_, name)<T, Signature>::value>::type {}

//
// element_initializer:
//     Calls initialize() on the element unless the function doesn't exist.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Engine>
struct element_initializer {
  private:
    AJG_DEFINE_METHOD_PREDICATE(initialize);

  public:
    Engine const& self;

    template <class T>
    struct has_initializer : mpl::or_
        < has_initialize<T, void(T::*)(Engine const&)>
        , has_initialize<T, void(T::*)(Engine const&) const>
        >::type {};

    template <class Element>
    void operator ()(Element& element, typename
            enable_if<has_initializer<Element> >::type* = 0) const {
        element.initialize(self);
    }

    template <class Element>
    void operator ()(Element& element, typename
            disable_if<has_initializer<Element> >::type* = 0) const {
        // Do nothing.
    }
};

//
// tag_renderer (only used by tmpl now)
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Engine, bool Merge = false>
struct tag_renderer {
    typename Engine::this_type    const& self;
    typename Engine::stream_type  const& stream_;
    typename Engine::match_type   const& match_;
    typename Engine::context_type const& context_;
    typename Engine::options_type const& options_;

    typedef void result_type;

    template <class Tag>
    void operator ()(Tag const& tag, typename disable_if_c<Merge, Tag>::type* = 0) const {
        typedef typename Engine::options_type options_type;
        typedef typename Engine::stream_type stream_type;
        options_type& options = const_cast<options_type&>(options_);
        stream_type& stream = const_cast<stream_type&>(stream_);
        tag.render(match_, self, context_, options, stream);
    }

    template <class Tag>
    void operator ()(Tag const& tag, typename enable_if_c<Merge, Tag>::type* = 0) const {
        typename Engine::args_type const args = { self, match_,
            const_cast<typename Engine::context_type&>(context_),
            const_cast<typename Engine::options_type&>(options_),
            const_cast<typename Engine::stream_type&>(stream_) };
        tag.render(args);
    }
};

//
// abbreviate_size
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String, class Size>
inline String abbreviate_size(Size const size) {
    double bucket = 1;
    String unit;

         if (size > (bucket = (std::pow)(2, 60.0))) unit = text("EB");
    else if (size > (bucket = (std::pow)(2, 50.0))) unit = text("PB");
    else if (size > (bucket = (std::pow)(2, 40.0))) unit = text("TB");
    else if (size > (bucket = (std::pow)(2, 30.0))) unit = text("GB");
    else if (size > (bucket = (std::pow)(2, 20.0))) unit = text("MB");
    else if (size > (bucket = (std::pow)(2, 10.0))) unit = text("KB");
    else if (size >=(bucket = (std::pow)(2, 00.0))) unit = text("bytes");

    std::basic_ostringstream<typename String::value_type> stream;
    stream << std::fixed << std::setprecision(1);
    stream << (size / bucket) << ' ' << unit;

    BOOST_ASSERT(stream);
    return stream.str();
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
// is_one_of
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline bool is_one_of( String const& string
                     , std::vector<std::string> const& options
                     ) {
    BOOST_FOREACH(std::string const& option, options) {
        if (string == String(option.begin(), option.end())) return true;
    }

    return false;
}

//
// validate_enumeration
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class String>
inline void validate_option( String const& value
                           , std::string const& name
                           , std::vector<std::string> const& options
                           ) {
    if (!is_one_of(value, options)) {
        throw_exception(invalid_attribute(name));
    }
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
        std::isalnum(c) || c == '_' || c == '-'
                        || c == '.' || c == '/'
            ? result += c
            : result += char_type('%') + to_hex<2>(c);
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
        std::isalnum(c) || algorithm::is_any_of("/#%[]=:;$&()+,!?")(c)
            ? result += c
            : result += char_type('%') + to_hex<2>(c);
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

/*
//
// may_find_by_index
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Engine, class Sequence, class Index, class Needle, class Functor>
inline optional<typename Functor::result_type> may_find_by_index( Engine   const& engine
                                                                , Sequence const& sequence
                                                                , Index    const& index
                                                                , Needle   const& needle
                                                                , Functor  const& functor
                                                                ) {
    typename Index::const_iterator const it = std::find(index.begin(), index.end(), needle);

    if (it == index.end()) {
        return boost::none;
    }

    typename Engine::size_type const distance = std::distance(index.begin(), it);
    BOOST_STATIC_CONSTANT(typename Engine::size_type, size = Sequence::size::value);
    return detail::template apply_at<size>::fn(distance, sequence, functor);
}
*/

//
// must_find_by_index
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, class Sequence, class Index, class Needle, class Functor>
inline typename Functor::result_type must_find_by_index( Sequence const& sequence
                                                       , Index    const& index
                                                       , Needle   const& needle
                                                       , Functor  const& functor
                                                       ) {
    typename Index::const_iterator const it = std::find(index.begin(), index.end(), needle);

    if (it == index.end()) {
        // TODO: Throw missing_tag exception.
        std::string const name = Traits::narrow(Traits::to_string(needle));
        std::string const message = name + " not found";
        AJG_SYNTH_THROW(std::runtime_error(message));
    }

    typename Traits::size_type const distance = std::distance(index.begin(), it);
    BOOST_STATIC_CONSTANT(typename Traits::size_type, size = Sequence::size::value);
    return detail::template apply_at<size>::fn(distance, sequence, functor);
}

//
// advance:
//     Simulates operator + for container iterators which lack it.
//     NOTE: Doesn't do any bounds checking; ensure distance is valid.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Container, class Distance>
inline static typename Container::const_iterator
advance(Container const& container, Distance const distance) {
    typename Container::const_iterator it = container.begin();
    std::advance(it, distance);
    return it;
}

//
// operator ==:
//     Provides a more readable way to compare match objects via regex_id's.
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
// drop
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Container, class Number>
inline std::pair
        < typename Container::const_iterator
        , typename Container::const_iterator
        >
drop(Container const& container, Number const number) {
    return std::make_pair
        ( advance(container, number)
        , container.end()
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
// construct:
//     Instantiates simple objects without constructors.
//     TODO[c++11]: Replace with aggregate initializers, e.g. T{x, y, z}.
////////////////////////////////////////////////////////////////////////////////////////////////////

// The base case (nullary.)
template <class T> inline T construct() { T const t; return t; }

#define PARAM(n) \
    (BOOST_PP_CAT(P, n) &               BOOST_PP_CAT(p, n)) \
    (BOOST_PP_CAT(P, n) const&          BOOST_PP_CAT(p, n))
 // (BOOST_PP_CAT(P, n) volatile&       BOOST_PP_CAT(p, n))
 // (BOOST_PP_CAT(P, n) const volatile& BOOST_PP_CAT(p, n))

#define PARAM_(z, n, nil) (PARAM(n))

#define CONSTRUCT(r, product) \
    template <class T BOOST_PP_ENUM_TRAILING_PARAMS \
        (BOOST_PP_SEQ_SIZE(product), class P)> \
    inline T construct BOOST_PP_SEQ_TO_TUPLE(product) { \
        T const t = { BOOST_PP_ENUM_PARAMS(BOOST_PP_SEQ_SIZE(product), p) }; \
        return t; \
    }

#define CONSTRUCT_N(z, n, nil) \
    BOOST_PP_SEQ_FOR_EACH_PRODUCT(CONSTRUCT, \
        BOOST_PP_REPEAT(BOOST_PP_ADD(n, 1), PARAM_, nil))

BOOST_PP_REPEAT(AJG_SYNTH_CONSTRUCT_LIMIT, CONSTRUCT_N, nil)

#undef CONSTRUCT_N
#undef CONSTRUCT
#undef PARAM_
#undef PARAM

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
