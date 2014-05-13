//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_MAP_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_MAP_HPP_INCLUDED

#include <map>

#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for std::map
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class K, class V>
struct adapter<Behavior, std::map<K, V> >  : container_adapter<Behavior, std::map<K, V> > {
    adapter(std::map<K, V> const& adapted) : container_adapter<Behavior, std::map<K, V> >(adapted) {}

    optional<typename Behavior::value_type> index(typename Behavior::value_type const& what) const {
        K const key = Behavior::template to<K>(what);
        typename std::map<K, V>::const_iterator const it = this->adapted().find(key);
        if (it == this->adapted().end()) {
            return boost::none;
        }
        return typename Behavior::value_type(it->second);
    }
};

//
// specialization for std::multimap
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class K, class V>
struct adapter<Behavior, std::multimap<K, V> >  : container_adapter<Behavior, std::multimap<K, V> > {
    adapter(std::multimap<K, V> const& adapted) : container_adapter<Behavior, std::multimap<K, V> >(adapted) {}
    // TODO: Implement index(), but returning a sequence or set of values, or the first one?
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_MAP_HPP_INCLUDED


