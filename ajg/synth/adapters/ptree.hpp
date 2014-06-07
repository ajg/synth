//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_PTREE_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_PTREE_HPP_INCLUDED

#include <algorithm>
#include <functional>

#include <boost/property_tree/ptree_fwd.hpp>

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for boost::property_tree::basic_ptree
//     TODO: Factor out a common associative_adapter and share it with map/multimap.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class K, class V>
struct adapter<Value, boost::property_tree::basic_ptree<K, V> >  : container_adapter<Value, boost::property_tree::basic_ptree<K, V>, type_flags(sequential | associative)> {
    adapter(boost::property_tree::basic_ptree<K, V> const& adapted) : container_adapter<Value, boost::property_tree::basic_ptree<K, V>, type_flags(sequential | associative)>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Value);

    virtual optional<boolean_type> get_boolean() const {
        return !this->adapted().empty() || boolean_type(value_type(this->adapted().data()));
    }

    virtual attribute_type attribute(value_type const& key) const {
        K const k = key.template to<K>();
        typename boost::property_tree::basic_ptree<K, V>::const_assoc_iterator const it = this->adapted().find(k);
        if (it == this->adapted().not_found()) {
            return boost::none;
        }
        return value_type(it->second);
    }

    // TODO: attribute(k, v)
    // TODO: attributes()

    virtual boolean_type output(ostream_type& ostream) const {
        if (this->adapted().empty()) {
            ostream << value_type(this->adapted().data());
        }
        else {
            value_type::delimited(ostream, range_type(this->adapted().begin(), this->adapted().end()));
        }
        return true;
    }
};

}}} // namespace ajg::synth::adapters

namespace boost {
namespace property_tree {

template <class K, class V>
inline bool operator <( basic_ptree<K, V> const& a
                      , basic_ptree<K, V> const& b
                      ) {
    return a.data() < b.data() && std::lexicographical_compare( a.ordered_begin(), a.not_found()
                                                              , b.ordered_begin(), b.not_found()
                                                              );
}

// Needed by the command_line binding.
template <class Char, class K, class V>
inline bool operator <<( std::basic_ostream<Char>& ostream
                       , basic_ptree<K, V> const&
                       ) {
    return ostream << "TODO";
}

}} // namespace boost::property_tree

#endif // AJG_SYNTH_ADAPTERS_PTREE_HPP_INCLUDED


