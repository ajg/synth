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

//
// specialization for boost::property_tree::basic_ptree
//     TODO: Factor out a common associative_adapter and share it with map/multimap.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class K, class V>
struct adapter<Behavior, boost::property_tree::basic_ptree<K, V> >  : concrete_adapter<Behavior, boost::property_tree::basic_ptree<K, V> > {
    adapter(boost::property_tree::basic_ptree<K, V> const& adapted) : concrete_adapter<Behavior, boost::property_tree::basic_ptree<K, V> >(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);

    boolean_type to_boolean() const {
        if (this->adapted().empty()) {
            return boolean_type(value_type(this->adapted().data()));
        }
        else {
            return true;
        }
    }

    void output(ostream_type& out) const {
        if (this->adapted().empty()) {
            out << value_type(this->adapted().data());
        }
        else {
            behavior_type::enumerate(*this, out);
        }
    }

    iterator begin() { return iterator(this->adapted().begin()); }
    iterator end()   { return iterator(this->adapted().end()); }

    const_iterator begin() const { return const_iterator(this->adapted().begin()); }
    const_iterator end()   const { return const_iterator(this->adapted().end()); }

    optional<value_type> index(value_type const& what) const {
        K const key = behavior_type::template to<K>(what);
        typename boost::property_tree::basic_ptree<K, V>::const_assoc_iterator const it = this->adapted().find(key);
        if (it == this->adapted().not_found()) {
            return boost::none;
        }
        return value_type(it->second);
    }
};

}} // namespace ajg::synth

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

}} // namespace boost::property_tree

#endif // AJG_SYNTH_ADAPTERS_PTREE_HPP_INCLUDED


