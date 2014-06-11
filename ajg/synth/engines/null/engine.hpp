//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED

#include <map>

#include <ajg/synth/engines/base_engine.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace null {

template <class Traits>
struct engine : base_engine<Traits> {
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

    typedef typename options_type::context_type                                 context_type;

    template <class Iterator>
    struct kernel;

  public:

    inline static char const* name() { return "null"; }

}; // engine


template <class Traits>
template <class Iterator>
struct engine<Traits>::kernel : base_engine<Traits>::AJG_SYNTH_TEMPLATE base_kernel<Iterator> {
  public:

    typedef kernel                                                              kernel_type;
    typedef Iterator                                                            iterator_type;
    typedef engine                                                              engine_type;
    typedef typename kernel_type::state_type                                    state_type;
    typedef typename kernel_type::range_type                                    range_type;

  public:

    kernel() {}

  public:

    inline void parse(state_type&) const {}
    inline void render(ostream_type&, options_type const&, state_type const&, context_type&) const {}
    inline static void initialize_state(state_type& state) {}

}; // kernel

}}}} // namespace ajg::synth::engines::null

#endif // AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED

