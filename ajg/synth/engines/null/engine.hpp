//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED

#include <map>

#include <ajg/synth/engines/null/value.hpp>
#include <ajg/synth/engines/base_engine.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace null {

//
// base_options
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct base_options { // TODO: Move to own file and make other engines' options derive from this.
  public:

    typedef Value                                                               value_type;

    typedef typename value_type::behavior_type                                  behavior_type;
    typedef typename value_type::range_type                                     range_type;
    typedef typename value_type::sequence_type                                  sequence_type;
    typedef typename value_type::mapping_type                                   mapping_type;
    typedef typename value_type::traits_type                                    traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::date_type                                     date_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::paths_type                                    paths_type;
    typedef typename traits_type::names_type                                    names_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;
};

template <class Traits, class Options = base_options<value<Traits> > >
struct engine : base_engine<Options> {
  public:

    typedef engine                                                              engine_type;
    typedef Traits                                                              traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef typename engine_type::value_type                                    value_type;
    typedef typename engine_type::options_type                                  options_type;

    typedef std::map<string_type, value_type>                                   context_type; // TODO: Move to options.

    template <class Iterator>
    struct kernel;

}; // engine


template <class Traits, class Options>
template <class Iterator>
struct engine<Traits, Options>::kernel : base_engine<Options>::AJG_SYNTH_TEMPLATE kernel<Iterator> {
  public:

    typedef kernel                                                              kernel_type;
    typedef Iterator                                                            iterator_type;
    typedef engine                                                              engine_type;
    typedef typename kernel_type::result_type                                   result_type;

  public:

    kernel() {}

  public:

    template <class I> void parse(std::pair<I, I> const&, result_type&, options_type const&) const {}
    template <class I> void parse(I const&, I const&, result_type&, options_type const&)     const {}

    void render(ostream_type&, result_type const&, context_type const&, options_type const&) const {}

}; // kernel

}}}} // namespace ajg::synth::engines::null

#endif // AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED

