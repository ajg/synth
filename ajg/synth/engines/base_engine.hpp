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

#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/engines/exceptions.hpp>

namespace ajg {
namespace synth {
namespace {
namespace x = boost::xpressive;
}

using detail::is;

template <class Traits>
struct base_engine : detail::nonconstructible {

    typedef base_engine                                                         engine_type;
    typedef Traits                                                              traits_type;

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
    struct kernel;

}; // base_engine

template <class Traits>
template <class Iterator>
struct base_engine<Traits>::kernel : boost::noncopyable {
  public:

    typedef kernel                                                              kernel_type;
    typedef Iterator                                                            iterator_type;
    typedef std::pair<iterator_type, iterator_type>                             range_type;

  protected:

    typedef x::regex_id_type                                                    id_type;
	typedef x::basic_regex<iterator_type>                                       regex_type;
    typedef x::match_results<iterator_type>                                     match_type;
    typedef x::sub_match<iterator_type>                                         sub_match_type;
    typedef x::placeholder<iterator_type>                                       placeholder_type;

    // Define string iterators/regexes specifically. This is useful when they are different from the
    // main iterator_type and regex_type (e.g. when the latter two involve the use of a file_iterator.)
    typedef typename string_type::const_iterator                                string_iterator_type;
    typedef x::basic_regex<string_iterator_type>                                string_regex_type;
    typedef x::match_results<string_iterator_type>                              string_match_type;

    typedef match_type                                                          frame_type;

  public:

    BOOST_STATIC_CONSTANT(size_type, error_line_limit = 30);

  protected:

    kernel()
        : nothing(x::as_xpr('\0')) {} // Xpressive barfs when default-constructed.

    void initialize_grammar() {
        typename x::function<set_furthest_iterator>::type const set_furthest = {{}};

        this->text = +(~x::before(this->skipper) >> x::_);

        // block = skip(text[...])(*tag[...]); // Using skip is slightly slower than this:
        this->block = *x::keep // Causes actions (i.e. furthest) to execute eagerly.
            ( x::ref(this->tag)  [set_furthest(iterator_, x::_)]
            | x::ref(this->text) [set_furthest(iterator_, x::_)]
            );
    }

  public:

    template <class I>
    void parse(std::pair<I, I> const& range, frame_type& frame) const {
        return this->parse(range.first, range.second, frame);
    }

    template <class I>
    void parse(I const& begin, I const& end, frame_type& frame) const {
        iterator_type const  begin_   = begin;
        iterator_type const  end_     = end;
        iterator_type        furthest = begin_;

        frame.let(this->iterator_ = furthest);
        if (x::regex_match(begin_, end_, frame, this->block)) {
            // On success, all input should have been consumed.
            BOOST_ASSERT(furthest == end_);
            return;
        }

        // On failure, throw a semi-informative exception.
        size_type   const room(std::distance(furthest, end_)), limit(error_line_limit);
        string_type const site(furthest, detail::advance_to(furthest, (std::min)(room, limit)));
        string_type const line(site.begin(), std::find(site.begin(), site.end(), char_type('\n')));
        throw_exception(parsing_error(traits_type::narrow(line)));
    }

  AJG_SYNTH_IF_MSVC(public, protected):

    regex_type tag;
    regex_type text;
    regex_type block;
    regex_type nothing;
    regex_type skipper;

  private:

    placeholder_type iterator_;

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

}; // kernel

}} // namespace ajg::synth

#endif // AJG_SYNTH_ENGINES_BASE_ENGINE_HPP_INCLUDED
