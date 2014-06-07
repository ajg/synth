//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_ADAPTER_HPP_INCLUDED

#include <ajg/synth/adapters/base_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// Shortcut macros
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_ADAPTER_TYPEDEFS(value) \
  public: \
    typedef value                                   value_type; \
    typedef typename value_type::iterator           iterator; \
    typedef typename value_type::const_iterator     const_iterator; \
    typedef typename value_type::range_type         range_type; \
    typedef typename value_type::attribute_type     attribute_type; \
    typedef typename value_type::attributes_type    attributes_type; \
    typedef typename value_type::adapter_type       adapter_type; \
    typedef typename value_type::traits_type        traits_type; \
    \
    typedef typename traits_type::boolean_type      boolean_type; \
    typedef typename traits_type::char_type         char_type; \
    typedef typename traits_type::size_type         size_type; \
    typedef typename traits_type::integer_type      integer_type; \
    typedef typename traits_type::floating_type     floating_type; \
    typedef typename traits_type::number_type       number_type; \
    typedef typename traits_type::string_type       string_type; \
    typedef typename traits_type::datetime_type     datetime_type; \
    typedef typename traits_type::duration_type     duration_type; \
    typedef typename traits_type::istream_type      istream_type; \
    typedef typename traits_type::ostream_type      ostream_type

//
// adapter (unspecialized)
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class Adapted>
struct adapter;

template <class Value>
struct adapter<Value, base_adapter<Value> >; // Left undefined.

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_ADAPTER_HPP_INCLUDED

