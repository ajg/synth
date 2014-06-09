//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_MAP_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_MAP_HPP_INCLUDED

#include <map>

#include <boost/foreach.hpp>

#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for std::map
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class K, class V>
struct adapter<Value, std::map<K, V> >     : container_adapter<Value, std::map<K, V>, associative> {
    adapter(std::map<K, V> const& adapted) : container_adapter<Value, std::map<K, V>, associative>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Value);

    virtual attribute_type attribute(value_type const& key) const {
        K const k = key.template to<K>();
        typename std::map<K, V>::const_iterator const it = this->adapted().find(k);
        if (it == this->adapted().end()) {
            return boost::none;
        }
        return value_type(it->second);
    }

    virtual void attribute(value_type const& key, attribute_type const& attr) const {
        K const k = key.template to<K>();
        this->adapted().erase(k);

        if (attr) {
            V const v = attr->template to<V>();
            this->adapted().insert(typename std::map<K, V>::value_type(k, v));
        }
    }

    virtual attributes_type attributes() const {
        attributes_type attributes;
        typedef typename std::map<K, V>::value_type pair_type;
        BOOST_FOREACH(pair_type const& kv, this->adapted()) {
            attributes.insert(kv.first);
        }
        return attributes;
    }
};

//
// specialization for std::multimap
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class K, class V>
struct adapter<Value, std::multimap<K, V> >     : container_adapter<Value, std::multimap<K, V>, associative> {
    adapter(std::multimap<K, V> const& adapted) : container_adapter<Value, std::multimap<K, V>, associative>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Value);

    // TODO: Implement attribute(k), but returning a sequence or set of values, or the first one?
    // TODO: Implement attribute(k, v)

    virtual attributes_type attributes() const {
        attributes_type attributes;
        typedef typename std::multimap<K, V>::value_type pair_type;
        BOOST_FOREACH(pair_type const& kv, this->adapted()) {
            attributes.insert(kv.first);
        }
        return attributes;
    }
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_MAP_HPP_INCLUDED


