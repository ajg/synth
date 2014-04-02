//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED

#include <map>
#include <functional>

#include <boost/mpl/void.hpp>

#include <ajg/synth/value_facade.hpp>
#include <ajg/synth/engines/base_definition.hpp>

namespace ajg {
namespace synth {
namespace detail {

// TODO: Move to null_value.hpp.
template <class Char>
struct null_value : value_facade<Char, null_value<Char> > {
  public:

    typedef value_facade<Char, null_value> base_type;

  public:

    AJG_SYNTH_VALUE_CONSTRUCTORS(null_value, base_type, {})
};

}}} // namespace ajg::synth::detail

namespace std {

template<class Char>
struct equal_to<ajg::synth::detail::null_value<Char> > {
    bool operator()(ajg::synth::detail::null_value<Char> const& a, ajg::synth::detail::null_value<Char> const& b) const {
        return true;
    }
};

template<class Char>
struct less<ajg::synth::detail::null_value<Char> > {
    bool operator()(ajg::synth::detail::null_value<Char> const& a, ajg::synth::detail::null_value<Char> const& b) const {
        return false;
    }
};

} // namespace std

namespace std {

template<class T>
struct less<complex<T> > {
    bool operator()(complex<T> const& a, complex<T> const& b) const {
        return a.real() < b.real() && a.imag() < b.imag();
    }
};

} // namespace std


namespace ajg {
namespace synth {

struct null_engine : detail::nonconstructible {

typedef null_engine engine_type;

template <class BidirectionalIterator>
struct definition : base_definition< BidirectionalIterator
                                   , definition<BidirectionalIterator>
                                   > {
  public:

    typedef definition this_type;
    typedef base_definition< BidirectionalIterator
                           , this_type> base_type;

    typedef typename base_type::id_type         id_type;
    typedef typename base_type::size_type       size_type;
    typedef typename base_type::char_type       char_type;
    typedef typename base_type::match_type      match_type;
    typedef typename base_type::regex_type      regex_type;
    typedef typename base_type::frame_type      frame_type;
    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stream_type     stream_type;
    typedef typename base_type::iterator_type   iterator_type;
    typedef typename base_type::definition_type definition_type;

    typedef detail::null_value<char_type>     value_type;
    typedef std::vector<value_type>           sequence_type; // TODO: Use Traits::sequence_type
    typedef std::map<string_type, value_type> context_type;  // TODO: Use Traits::context_type
    typedef boost::mpl::void_                 options_type;

  public:

    definition() {}

  public:

    void render( stream_type&        stream
               , frame_type   const& frame
               , context_type const& context
               , options_type const& options) const {}

}; // definition

}; // null_engine

}} // namespace ajg::synth

#endif // AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED

