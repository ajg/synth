//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_BASE_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_BASE_ENGINE_HPP_INCLUDED

#include <map>
#include <set>
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

#include <ajg/synth/templates.hpp>
#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/engines/exceptions.hpp>

namespace ajg {
namespace synth {

namespace xpressive = boost::xpressive;

struct base_engine : detail::nonconstructible {

template < class BidirectionalIterator
	     , class Definition
		 // This argument is only necessary because MSVC chokes on iterator_type::value_type:
	     , class Char = typename BidirectionalIterator::value_type
         >
struct definition : boost::noncopyable {
  public:

    // Parametrized types:
    typedef BidirectionalIterator iterator_type;
    typedef Definition            definition_type;

    // Derived types:
    typedef definition                                    this_type;
    typedef bool                                          boolean_type; // TODO: Use Traits::boolean_type
    typedef std::size_t                                   size_type;    // TODO: Use Traits::size_type
    typedef Char /* typename iterator_type::value_type */ char_type;    // TODO: Use Traits::char_type

    typedef xpressive::regex_id_type                id_type;
	typedef xpressive::basic_regex<iterator_type>   regex_type;
    typedef xpressive::match_results<iterator_type> frame_type;
    typedef xpressive::match_results<iterator_type> match_type;
    typedef xpressive::sub_match<iterator_type>     sub_match_type;
    typedef std::basic_string<char_type>            string_type; // TODO: Use Traits::string_type.
    typedef std::basic_ostream<char_type>           stream_type; // TODO: Use Traits::stream_type.
    typedef std::set<string_type>                   symbols_type;

    // Define string iterators/regexes specifically. This is useful when
    // they are different from the main iterator_type and regex_type (e.g.
    // when the latter two involve the use of a file_iterator.)
    typedef typename string_type::const_iterator           string_iterator_type;
    typedef xpressive::basic_regex<string_iterator_type>   string_regex_type;
    typedef xpressive::match_results<string_iterator_type> string_match_type;

  public:

    BOOST_STATIC_CONSTANT(size_type, error_line_limit = 30);

  protected:

//
// set_furthest_iterator:
//     A functor that sets the iterator to the furthest, either itself or the submatch's end.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct set_furthest_iterator {
        typedef void result_type;

        void operator()(iterator_type& iterator, sub_match_type const& sub_match) const {
            iterator = (std::max)(iterator, sub_match.second);
        }
    };

    void initialize_grammar() {
        using namespace xpressive;

        typename xpressive::function<set_furthest_iterator>::type const set_furthest = {{}};
        definition_type& self = static_cast<definition_type&>(*this);

        self.text = +(~before(self.skipper) >> _);

        // block = skip(text[...])(*tag[...]); // Using skip is slightly slower than this:
        self.block = *keep // Causes actions (i.e. furthest) to execute eagerly.
            ( xpressive::ref(self.tag)  [set_furthest(iterator_, _)]
            | xpressive::ref(self.text) [set_furthest(iterator_, _)]
            );
    }

  public:

    template <class I>
    void parse(std::pair<I, I> const& range, frame_type& frame) const {
        return parse(range.first, range.second, frame);
    }

    template <class I>
    void parse(I const& begin, I const& end, frame_type& frame) const {
        typedef typename definition_type::traits_type traits_type;

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
        size_type   const room(std::distance(furthest, end_)), limit(error_line_limit);
        string_type const site(furthest, furthest + (std::min)(room, limit));
        string_type const line(site.begin(), detail::find_or('\n', site, site.end()));
        throw_exception(parsing_error(traits_type::narrow(line)));
    }

  protected:

    xpressive::placeholder<iterator_type> iterator_;

}; // definition

}; // base_engine

}} // namespace ajg::synth

#endif // AJG_SYNTH_ENGINES_BASE_ENGINE_HPP_INCLUDED
