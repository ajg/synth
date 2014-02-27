//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_ADAPTERS_MAP_HPP_INCLUDED
#define AJG_SYNTHESIS_ADAPTERS_MAP_HPP_INCLUDED

#include <map>

#include <ajg/synthesis/adapters/adapter.hpp>

namespace ajg {
namespace synthesis {

//
// specialization for std::map
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class K, class V>
struct adapter<Traits, std::map<K, V> >
    : public abstract_adapter<Traits> {

    typedef std::map<K, V> map_type;
    AJG_SYNTHESIS_ADAPTER(map_type)
    adapted_type adapted_;

  public:

    boolean_type test() const { return !adapted_.empty(); }
    void output(ostream_type& out) const { this->list(out); }

    iterator begin() { return iterator(adapted_.begin()); }
    iterator end()   { return iterator(adapted_.end()); }

    const_iterator begin() const { return const_iterator(adapted_.begin()); }
    const_iterator end()   const { return const_iterator(adapted_.end()); }
};

//
// specialization for std::multimap
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class K, class V>
struct adapter<Traits, std::multimap<K, V> >
    : public abstract_adapter<Traits> {

    typedef std::multimap<K, V> map_type;
    AJG_SYNTHESIS_ADAPTER(map_type)
    adapted_type adapted_;

  public:

    boolean_type test() const { return !adapted_.empty(); }
    void output(ostream_type& out) const { this->list(out); }

    iterator begin() { return iterator(adapted_.begin()); }
    iterator end()   { return iterator(adapted_.end()); }

    const_iterator begin() const { return const_iterator(adapted_.begin()); }
    const_iterator end()   const { return const_iterator(adapted_.end()); }
};

}} // namespace ajg::synthesis

#endif // AJG_SYNTHESIS_ADAPTERS_MAP_HPP_INCLUDED


