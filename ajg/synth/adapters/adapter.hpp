//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_ADAPTER_HPP_INCLUDED

#include <ajg/synth/adapters/base_adapter.hpp>

namespace ajg {
namespace synth {

//
// Shortcut macros
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_ADAPTER_TYPEDEFS(adaptedT, adapterT) \
  public: \
    typedef adapterT                                adapter_type; \
    typedef adaptedT                                adapted_type; \
    typedef Behavior                                behavior_type; \
    typedef base_adapter<behavior_type>             base_type; \
    typedef typename behavior_type::value_type      value_type; \
    typedef typename behavior_type::traits_type     traits_type; \
    \
    typedef typename traits_type::boolean_type      boolean_type; \
    typedef typename traits_type::char_type         char_type; \
    typedef typename traits_type::size_type         size_type; \
    typedef typename traits_type::integer_type      integer_type; \
    typedef typename traits_type::number_type       number_type; \
    typedef typename traits_type::string_type       string_type; \
    typedef typename traits_type::datetime_type     datetime_type; \
    typedef typename traits_type::duration_type     duration_type; \
    typedef typename traits_type::istream_type      istream_type; \
    typedef typename traits_type::ostream_type      ostream_type; \
    \
    typedef typename value_type::iterator           iterator; \
    typedef typename value_type::const_iterator     const_iterator; \
    typedef typename value_type::range_type         range_type; \
    \
    friend struct base_adapter<behavior_type>

// TODO: Refactor this into a concrete_adapter<T>.
#define AJG_SYNTH_ADAPTER(adaptedT) \
    AJG_SYNTH_ADAPTER_TYPEDEFS(adaptedT, adapter); \
  protected: \
    virtual boolean_type equal_adapted(base_type const& that) const { return this->template equal_as<adapter>(that); } \
    virtual boolean_type less_adapted(base_type const& that) const { return this->template less_as<adapter>(that); } \
  public: \
    adapter(adapted_type const& adapted) : adapted_(adapted) {} \
    std::type_info const& type() const { return typeid(adaptedT); }

//
// adapter (unspecialized)
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class Adapted>
struct adapter;

template <class Behavior>
struct adapter<Behavior, base_adapter<Behavior> >; // Left undefined.

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_ADAPTER_HPP_INCLUDED

