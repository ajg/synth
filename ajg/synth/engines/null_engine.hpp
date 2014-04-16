//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED

#include <map>

#include <ajg/synth/engines/null_value.hpp>
#include <ajg/synth/engines/base_engine.hpp>

namespace ajg {
namespace synth {

template <class Traits>
struct null_engine : base_engine<Traits> {
  public:

    typedef null_engine                                                         engine_type;
    typedef Traits                                                              traits_type;

    typedef typename traits_type::void_type                                     void_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef null_value<traits_type>                                             value_type;
    typedef std::map<string_type, value_type>                                   context_type;
    typedef void_type                                                           options_type;

    template <class Iterator>
    struct kernel;

}; // null_engine


template <class Traits>
template <class Iterator>
struct null_engine<Traits>::kernel : base_engine<Traits>::AJG_SYNTH_TEMPLATE kernel<Iterator> {
  public:

    typedef kernel                                                              kernel_type;
    typedef Iterator                                                            iterator_type;
    typedef typename kernel_type::frame_type                                    frame_type;
    typedef null_engine                                                         engine_type;

  public:

    kernel() {}

  public:

    template <class I> void parse(std::pair<I, I> const&, frame_type&) const {}
    template <class I> void parse(I const&, I const&, frame_type&)     const {}

    void render(ostream_type&, frame_type const&, context_type const&, options_type const&) const {}

}; // kernel

}} // namespace ajg::synth

#endif // AJG_SYNTH_ENGINES_NULL_ENGINE_HPP_INCLUDED

