
//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ADAPTERS_DEQUE_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ADAPTERS_DEQUE_HPP_INCLUDED

#include <deque>

#include <chemical/synthesis/adapters/adapter.hpp>

namespace chemical {
namespace synthesis {

//
// specialization for std::deque
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, std::deque<T> >
    : public abstract_adapter<Traits> {

    CHEMICAL_SYNTHESIS_ADAPTER(std::deque<T>)
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

#endif // CHEMICAL_SYNTHESIS_ADAPTERS_DEQUE_HPP_INCLUDED

