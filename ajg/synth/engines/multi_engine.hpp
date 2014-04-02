//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_MULTI_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_MULTI_ENGINE_HPP_INCLUDED

#if AJG_SYNTH_OBSOLETE

#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <numeric>
#include <cstdlib>
#include <utility>
#include <algorithm>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>

#include <ajg/synth/templates.hpp>
#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/engines/exceptions.hpp>
#include <ajg/synth/engines/base_definition.hpp>
#include <ajg/synth/engines/ssi/value.hpp>
#include <ajg/synth/engines/ssi/library.hpp>

namespace ajg {
namespace synth {

template <class String>
struct options {
    typedef String string_type;

    string_type engine;

    options(string_type const& engine)
        : engine(engine) {}
};

template <>
struct multi_engine : detail::nonconstructible {

typedef multi_engine engine_type;

template <class BidirectionalIterator>
struct definition : base_definition<BidirectionalIterator, definition<BidirectionalIterator> > {
  public:

    typedef base_definition<BidirectionalIterator, definition> base_type;

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

  public:

    definition() {}

  public:

    void render( stream_type&        stream
               , frame_type   const& frame
               , context_type const& context
               , options_type const& options) const {

        if (options.engine == engine::name()) {

        }
    }

}; // definition

}; // multi_engine

}} // namespace ajg::synth

#endif // AJG_SYNTH_OBSOLETE

#endif // AJG_SYNTH_ENGINES_MULTI_ENGINE_HPP_INCLUDED
