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

template <class Traits>
struct engine : base_engine<Traits> {
  public:

    typedef engine                                                              engine_type;
    typedef Traits                                                              traits_type;

    typedef typename traits_type::void_type                                     void_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef null::value<traits_type>                                            value_type;
    typedef std::map<string_type, value_type>                                   context_type;
    typedef void_type                                                           options_type;

    template <class Iterator>
    struct kernel;

}; // engine


template <class Traits>
template <class Iterator>
struct engine<Traits>::kernel : base_engine<Traits>::AJG_SYNTH_TEMPLATE kernel<Iterator> {
  public:

    typedef kernel                                                              kernel_type;
    typedef Iterator                                                            iterator_type;
    typedef engine                                                              engine_type;
    typedef typename kernel_type::result_type                                   result_type;

  public:

    kernel() {}

  public:

    template <class I> void parse(std::pair<I, I> const&, result_type&) const {}
    template <class I> void parse(I const&, I const&, result_type&)     const {}

    void render(ostream_type&, result_type const&, context_type const&, options_type const&) const {}

}; // kernel

}}}} // namespace ajg::synth::engines::null

#endif // AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED

