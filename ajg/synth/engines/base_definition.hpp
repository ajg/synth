//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_BASE_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_BASE_ENGINE_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <numeric>
#include <algorithm>

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

#include <boost/xpressive/basic_regex.hpp>
#include <boost/xpressive/match_results.hpp>
#include <boost/xpressive/regex_actions.hpp>
#include <boost/xpressive/regex_compiler.hpp>
#include <boost/xpressive/regex_algorithms.hpp>
#include <boost/xpressive/regex_primitives.hpp>

#include <ajg/synth/template.hpp>
#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/engines/exceptions.hpp>

namespace ajg {
namespace synth {

template <class BidirectionalIterator, class Definition>
struct base_definition : noncopyable {
  public:

    // Parametrized types:
    typedef BidirectionalIterator iterator_type;
    typedef Definition            definition_type;

    // Derived types:
    typedef base_definition                         this_type;
    typedef std::size_t                             size_type;
    typedef typename iterator_type::value_type      char_type;

    typedef xpressive::regex_id_type                id_type;
    typedef xpressive::basic_regex<iterator_type>   regex_type;
    typedef xpressive::match_results<iterator_type> frame_type;
    typedef xpressive::match_results<iterator_type> match_type;
    typedef std::basic_string<char_type>            string_type;
    typedef std::basic_ostream<char_type>           stream_type;

    // Define string iterators/regexes specifically. This is useful when
    // they are different from the main iterator_type and regex_type (e.g.
    // when the latter two involve the use of a file_iterator.)
    typedef typename string_type::const_iterator           string_iterator_type;
    typedef xpressive::basic_regex<string_iterator_type>   string_regex_type;
    typedef xpressive::match_results<string_iterator_type> string_match_type;

  protected:

    void initialize_grammar() {
        using namespace xpressive;

        typedef detail::furthest_iterator<iterator_type> f;
        typename xpressive::function<f>::type const furthest = {{}};
        definition_type& self = static_cast<definition_type&>(*this);

        self.text = +(~before(self.skipper) >> _);

        // Using skip is slightly slower than the below.
        // block = skip(text[...])(*tag[...]);

        self.block = *keep // causes actions (i.e. furthest) to execute eagerly.
            ( xpressive::ref(self.tag)  [iterator_ = furthest(iterator_, _)]
            | xpressive::ref(self.text) [iterator_ = furthest(iterator_, _)]
            );
    }

  public:

    // This function allows us to centralize string conversion
    // in order to properly, yet orthogonally, support Unicode.

    template <class Char, class String>
    inline std::basic_string<Char> convert(String const& string) const {
        return std::basic_string<Char>(string.begin(), string.end());
    }

/*
    template <class String>
    inline string_type convert(String const& string) const {
        return lexical_cast<string_type>(string);
    }

    template <class String>
    inline std::string narrow(String const& string) const {
        //return lexical_cast<std::string>(string);
        return std::string(string.begin(), string.end());
    }

    template <class String>
    inline std::wstring widen(String const& string) const {
        //return lexical_cast<std::wstring>(string);
        return std::wstring(string.begin(), string.end());
    }*/

    template <class Iterator>
    void parse(std::pair<Iterator, Iterator> const& range, frame_type& frame) const {
        return parse(range.first, range.second, frame);
    }

    template <class Iterator>
    void parse(Iterator const& begin, Iterator const& end, frame_type& frame) const {
        definition_type const& self = static_cast<definition_type const&>(*this);
        iterator_type const begin_ = begin;
        iterator_type const end_ = end;
        iterator_type furthest = begin_;
        frame.let(self.iterator_ = furthest);

        if (xpressive::regex_match(begin_, end_, frame, self.block)) {
            // On success, all input should have been consumed.
            BOOST_ASSERT(furthest == end_);
            return;
        }

        // On failure, throw a semi-informative exception.
        size_type   const room(std::distance(furthest, end_)), limit(30);
        string_type const site(furthest, furthest + (std::min)(room, limit));
        string_type const line(site.begin(), detail::find_or('\n', site, site.end()));
        throw_exception(parsing_error(convert<char>(line)));
    }

  protected:

    xpressive::placeholder<iterator_type> iterator_;

}; // base_definition

}} // namespace ajg::synth

#endif // AJG_SYNTH_ENGINES_BASE_ENGINE_HPP_INCLUDED
