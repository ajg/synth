//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_BASE_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_BASE_ENGINE_HPP_INCLUDED

#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <ostream>
#include <numeric>

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

#include <boost/iterator/filter_iterator.hpp>

#include <boost/xpressive/basic_regex.hpp>
#include <boost/xpressive/match_results.hpp>
#include <boost/xpressive/regex_actions.hpp>
#include <boost/xpressive/regex_compiler.hpp>
#include <boost/xpressive/regex_algorithms.hpp>
#include <boost/xpressive/regex_primitives.hpp>

#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/detail/text.hpp>
#include <ajg/synth/detail/advance_to.hpp>
#include <ajg/synth/engines/state.hpp>
#include <ajg/synth/engines/value.hpp>
#include <ajg/synth/engines/context.hpp>
#include <ajg/synth/engines/options.hpp>

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

namespace x = boost::xpressive;

template <class Traits>
struct base_engine {

    typedef Traits                                                              traits_type;
    typedef base_engine                                                         engine_type;
    typedef value<traits_type>                                                  value_type;
    typedef context<value_type>                                                 context_type;
    typedef options<context_type>                                               options_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::paths_type                                    paths_type;
    typedef typename traits_type::symbols_type                                  symbols_type;

    template <class Iterator>
    struct base_kernel;

  private:

    base_engine(); // TODO[c++11]: = delete;

}; // base_engine

template <class Traits>
template <class Iterator>
struct base_engine<Traits>::base_kernel : boost::noncopyable {
  public:

    typedef base_kernel                                                         base_kernel_type;
    typedef Iterator                                                            iterator_type;
    typedef std::pair<iterator_type, iterator_type>                             range_type;

  protected:

    typedef x::regex_id_type                                                    id_type;
    typedef x::basic_regex<iterator_type>                                       regex_type;
    typedef x::match_results<iterator_type>                                     match_type;
    typedef x::sub_match<iterator_type>                                         sub_match_type;

    // Define string iterators/regexes specifically. This is useful when they are different from the
    // main iterator_type and regex_type (e.g. when the latter two involve the use of a file_iterator.)
    typedef typename string_type::const_iterator                                string_iterator_type;
    typedef x::basic_regex<string_iterator_type>                                string_regex_type;
    typedef x::match_results<string_iterator_type>                              string_match_type;
    typedef detail::text<string_type>                                           text;

  public:

    typedef state<match_type, range_type, options_type>                         state_type;

  public:

    BOOST_STATIC_CONSTANT(size_type, error_line_limit = 30);

  protected:

    base_kernel() : nothing(as_xpr('\0')) {} // Xpressive barfs when default-constructed.

  protected:

//
// [deprecated] initialize_grammar_using_skipper
//     NOTE: Using the skipper leads to stack overflows due to this bug in Xpressive:
//           https://svn.boost.org/trac/boost/ticket/3936
////////////////////////////////////////////////////////////////////////////////////////////////////

    void initialize_grammar_using_skipper(regex_type const& skipper) {
        // TODO: Invoke set_furthest in some (maybe all) the derived engine regexes (like markers)
        //       to present more precise error message lines.
        typename x::function<set_furthest_iterator>::type const set_furthest = {{}};
        this->plain = +(~x::before(skipper) >> _);

        // block = skip(plain[...])(*tag[...]); // Using skip is slightly slower than this:
        this->block = *x::keep // Causes actions (i.e. furthest) to execute eagerly.
            ( x::ref(this->tag)   [set_furthest(*this->_state, _)]
            | x::ref(this->plain) [set_furthest(*this->_state, _)]
            );
    }

//
// initialize_grammar
////////////////////////////////////////////////////////////////////////////////////////////////////

    void initialize_grammar(regex_type const& allowed) {
        // TODO: Invoke set_furthest in some (maybe all) the derived engine regexes (like markers)
        //       to present more precise error message lines.
        typename x::function<set_furthest_iterator>::type const set_furthest = {{}};
        this->plain = x::keep(allowed[set_furthest(*this->_state, _)]);
        this->block = this->plain
            >> *(x::keep(x::ref(this->tag)[set_furthest(*this->_state, _)]) >> this->plain);
    }

//
// is
//     Returns whether the match and regex share regex_ids.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static boolean_type is(match_type const& match, regex_type const& regex) {
        return match.regex_id() == regex.regex_id();
    }

//
// is_
//     Note that this isn't an overload of `is` because string_match_type can equal match_type.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static boolean_type is_(string_match_type const& match, string_regex_type const& regex) {
        return match.regex_id() == regex.regex_id();
    }

//
// unnest
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static match_type const& unnest(match_type const& match) {
        BOOST_ASSERT(match);
        BOOST_ASSERT(match.size() >= 1);
        return *match.nested_results().begin();
    }

//
// unnest_
//     Note that this isn't an overload of `unnest` because string_match_type can equal match_type.
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static string_match_type const& unnest_(string_match_type const& match) {
        BOOST_ASSERT(match);
        BOOST_ASSERT(match.size() >= 1);
        return *match.nested_results().begin();
    }

  public:

//
// select_nested
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static std::pair
            < boost::filter_iterator<x::regex_id_filter_predicate<typename regex_type::iterator_type>, typename match_type::nested_results_type::const_iterator>
            , boost::filter_iterator<x::regex_id_filter_predicate<typename regex_type::iterator_type>, typename match_type::nested_results_type::const_iterator>
            >
    select_nested(match_type const& match, regex_type const& regex) {
        typename match_type::nested_results_type::const_iterator begin(match.nested_results().begin());
        typename match_type::nested_results_type::const_iterator end(match.nested_results().end());
        x::regex_id_filter_predicate<typename regex_type::iterator_type> predicate(regex.regex_id());
        return std::make_pair( boost::make_filter_iterator(predicate, begin, end)
                             , boost::make_filter_iterator(predicate, end,   end)
                             );
    }

//
// parse
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline void parse(state_type* state, string_type const& origin = string_type()) const { // Pointer to make clear it's mutable.
        state->match().let(this->_state = state);

        if (!x::regex_match(state->begin(), state->end(), state->match(), this->block)) {
            // On failure, throw a semi-informative exception.
            string_type const vicinity = text::narrow(state->vicinity(error_line_limit));
            std::size_t const line = static_cast<std::size_t>(state->line());
            if (origin.empty())
                AJG_SYNTH_THROW(parsing_error(line, vicinity));
            else
                AJG_SYNTH_THROW(parsing_error(origin, line, vicinity));
        }

        // On success, all input should have been consumed.
        AJG_SYNTH_ASSERT(state->consumed());
    }

  AJG_SYNTH_IF_MSVC(public, protected):

    regex_type tag;
    regex_type plain;
    regex_type block;
    regex_type nothing;

  public: // TODO: protected

    x::placeholder<state_type*> _state;

  private:

//
// set_furthest_iterator:
//     A functor that sets the iterator to the furthest, either itself or the submatch's end.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct set_furthest_iterator {
        typedef void result_type;

        void operator()(state_type& state, sub_match_type const& sub_match) const {
            state.furthest(sub_match.second);
        }
    };

};

}}} // namespace ajg::synth::engines

#endif // AJG_SYNTH_ENGINES_BASE_ENGINE_HPP_INCLUDED
