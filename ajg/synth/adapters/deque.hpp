//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_DEQUE_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_DEQUE_HPP_INCLUDED

#include <deque>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for std::deque
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, std::deque<T> >
    : public abstract_adapter<Traits> {

    AJG_SYNTH_ADAPTER(std::deque<T>)
    adapted_type adapted_;

  public:

    boolean_type test() const { return !adapted_.empty(); }
    void output(ostream_type& out) const { this->list(out); }

    iterator begin() { return iterator(adapted_.begin()); }
    iterator end()   { return iterator(adapted_.end()); }

    const_iterator begin() const { return const_iterator(adapted_.begin()); }
    const_iterator end()   const { return const_iterator(adapted_.end()); }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_DEQUE_HPP_INCLUDED

