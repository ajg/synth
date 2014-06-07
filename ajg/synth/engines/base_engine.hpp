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
#include <algorithm>

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
#include <ajg/synth/detail/container.hpp>
#include <ajg/synth/engines/base_state.hpp>
#include <ajg/synth/engines/base_value.hpp>
#include <ajg/synth/engines/base_context.hpp>
#include <ajg/synth/engines/base_options.hpp>

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
    typedef base_value<traits_type>                                             value_type;
    typedef base_context<value_type>                                            context_type;
    typedef base_options<context_type>                                          options_type;

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

    typedef base_state<match_type, range_type, options_type>                    state_type;

  public:

    BOOST_STATIC_CONSTANT(size_type, error_line_limit = 30);

  protected:

    base_kernel() : nothing(as_xpr('\0')) {} // Xpressive barfs when default-constructed.

  protected:

    void initialize_grammar() {
        // TODO: Invoke set_furthest in some (maybe all) the derived engine regexes (like markers)
        //       to present more precise error message lines.
        typename x::function<set_furthest_iterator>::type const set_furthest = {{}};

        this->plain = +(~x::before(this->skipper) >> _);

        // block = skip(plain[...])(*tag[...]); // Using skip is slightly slower than this:
        this->block = *x::keep // Causes actions (i.e. furthest) to execute eagerly.
            ( x::ref(this->tag)   [set_furthest(this->_iterator, _)]
            | x::ref(this->plain) [set_furthest(this->_iterator, _)]
            );
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

  public:

    inline void parse(state_type& state) const {
        range_type     r = state.range;
        iterator_type it = r.first;

        state.match.let(this->_state    = state);
        state.match.let(this->_iterator = it);

        if (x::regex_match(r.first, r.second, state.match, this->block)) {
            // On success, all input should have been consumed.
            BOOST_ASSERT(it == r.second);
            return;
        }

        // On failure, throw a semi-informative exception.
        size_type   const buffer(std::distance(it, r.second));
        size_type   const limit(error_line_limit);
        string_type const site(it, detail::advance_to(it, (std::min)(buffer, limit)));
        string_type const line(site.begin(), std::find(site.begin(), site.end(), char_type('\n')));
        AJG_SYNTH_THROW(parsing_error(text::narrow(line)));
    }

  AJG_SYNTH_IF_MSVC(public, protected):

    regex_type tag;
    regex_type plain;
    regex_type block;
    regex_type nothing;
    regex_type skipper;

  public: // TODO: protected

    x::placeholder<state_type>    _state;
    x::placeholder<iterator_type> _iterator;

  private:

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

};

}}} // namespace ajg::synth::engines

#endif // AJG_SYNTH_ENGINES_BASE_ENGINE_HPP_INCLUDED
