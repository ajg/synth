//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED

#include <map>

#include <boost/mpl/void.hpp>

#include <ajg/synth/engines/null_value.hpp>
#include <ajg/synth/engines/base_engine.hpp>

namespace ajg {
namespace synth {

struct null_engine : base_engine {

template <class BidirectionalIterator>
struct definition : base_engine::definition<BidirectionalIterator, definition<BidirectionalIterator> > {
  public:

    typedef base_engine::definition<BidirectionalIterator, definition> base_type;

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

    typedef null_value<char_type>             value_type;    // TODO: Use Traits::value_type?
    typedef std::vector<value_type>           sequence_type; // TODO: Use Traits::sequence_type
    typedef std::map<string_type, value_type> context_type;  // TODO: Use Traits::context_type
    typedef boost::mpl::void_                 options_type;

  public:

    definition() {}

  public:

    void render( stream_type&        stream
               , frame_type   const& frame
               , context_type const& context
               , options_type const& options
               ) const {}

}; // definition

}; // null_engine

}} // namespace ajg::synth

#endif // AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED

