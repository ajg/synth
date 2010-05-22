
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ADAPTERS_SET_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ADAPTERS_SET_HPP_INCLUDED

#include <set>

#include <chemical/synthesis/adapters/adapter.hpp>

namespace chemical {
namespace synthesis {

//
// specialization for std::set
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, std::set<T> >
    : public abstract_adapter<Traits> {

    typedef std::set<T> set_type;
    CHEMICAL_SYNTHESIS_ADAPTER(set_type)
    adapted_type const adapted_;

  public:

    boolean_type test() const { return !adapted_.empty(); }
    void output(ostream_type& out) const { this->list(out); }

    iterator begin() { return iterator(adapted_.begin()); }
    iterator end()   { return iterator(adapted_.end()); }

    const_iterator begin() const { return const_iterator(adapted_.begin()); }
    const_iterator end()   const { return const_iterator(adapted_.end()); }
};

//
// specialization for std::multiset
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, std::multiset<T> >
    : public abstract_adapter<Traits> {

    typedef std::multiset<T> set_type;
    CHEMICAL_SYNTHESIS_ADAPTER(set_type)
    adapted_type const adapted_;

  public:

    boolean_type test() const { return !adapted_.empty(); }
    void output(ostream_type& out) const { this->list(out); }

    iterator begin() { return iterator(adapted_.begin()); }
    iterator end()   { return iterator(adapted_.end()); }

    const_iterator begin() const { return const_iterator(adapted_.begin()); }
    const_iterator end()   const { return const_iterator(adapted_.end()); }
};

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_ADAPTERS_SET_HPP_INCLUDED
