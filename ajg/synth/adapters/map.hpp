//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_MAP_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_MAP_HPP_INCLUDED

#include <map>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for std::map
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, class K, class V>
struct adapter<Traits, std::map<K, V> >
    : public base_adapter<Traits> {

    typedef K                     key_type;
    typedef std::map<key_type, V> map_type;
    AJG_SYNTH_ADAPTER(map_type)
    adapted_type adapted_;

  public:

    boolean_type to_boolean() const { return !adapted_.empty(); }
    void output(ostream_type& out) const { traits_type::adapter_traits::enumerate(*this, out); }

    iterator begin() { return iterator(adapted_.begin()); }
    iterator end()   { return iterator(adapted_.end()); }

    const_iterator begin() const { return const_iterator(adapted_.begin()); }
    const_iterator end()   const { return const_iterator(adapted_.end()); }

    optional<value_type> index(value_type const& what) const {
        key_type const key = traits_type::template to<key_type>(what);
        typename map_type::const_iterator const it = adapted_.find(key);
        if (it == adapted_.end()) {
            return boost::none;
        }
        return value_type(it->second);
    }
};

//
// specialization for std::multimap
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, class K, class V>
struct adapter<Traits, std::multimap<K, V> >
    : public base_adapter<Traits> {

    typedef std::multimap<K, V> map_type;
    AJG_SYNTH_ADAPTER(map_type)
    adapted_type adapted_;

  public:

    boolean_type to_boolean() const { return !adapted_.empty(); }
    void output(ostream_type& out) const { traits_type::adapter_traits::enumerate(*this, out); }

    iterator begin() { return iterator(adapted_.begin()); }
    iterator end()   { return iterator(adapted_.end()); }

    const_iterator begin() const { return const_iterator(adapted_.begin()); }
    const_iterator end()   const { return const_iterator(adapted_.end()); }

    /* TODO: Return a sequence or set of values, or the first one?
    optional<value_type> index(value_type const& what) const {
        key_type const key = traits_type::template to<key_type>(what);
        typename map_type::const_iterator it const = adapted_.find(key);
        if (it == adapted_.end()) {
            return boost::none;
        }
        return it->second;
    }
    */
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_MAP_HPP_INCLUDED


