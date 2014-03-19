//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_PTREE_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_PTREE_HPP_INCLUDED

#include <boost/property_tree/ptree_fwd.hpp>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for boost::property_tree::basic_ptree
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class K, class V>
struct adapter<Traits, boost::property_tree::basic_ptree<K, V> >
    : public abstract_adapter<Traits> {

    typedef K                                              key_type;
    typedef boost::property_tree::basic_ptree<key_type, V> ptree_type;
    AJG_SYNTH_ADAPTER(ptree_type)
    adapted_type adapted_;

  public:

    boolean_type test() const {
        if (adapted_.empty()) {
            return boolean_type(value_type(adapted_.data()));
        }
        else {
            return true;
        }
    }

    void output(ostream_type& out) const {
        if (adapted_.empty()) {
            out << value_type(adapted_.data());
        }
        else {
            this->list(out);
        }
    }

    iterator begin() { return iterator(adapted_.begin()); }
    iterator end()   { return iterator(adapted_.end()); }

    const_iterator begin() const { return const_iterator(adapted_.begin()); }
    const_iterator end()   const { return const_iterator(adapted_.end()); }

    optional<value_type> index(value_type const& what) const {
        key_type const key = traits_type::template convert<value_type, key_type>(what);
        typename ptree_type::const_iterator const it = adapted_.find(key);
        if (it == adapted_.end()) {
            return boost::none;
        }
        return value_type(it->second);
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_PTREE_HPP_INCLUDED


