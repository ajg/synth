//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_LIST_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_LIST_HPP_INCLUDED

#include <list>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for std::list
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, std::list<T> >
    : public base_adapter<Behavior> {

    AJG_SYNTH_ADAPTER(std::list<T>)
    adapted_type adapted_;

  public:

    boolean_type to_boolean() const { return !adapted_.empty(); }
    void output(ostream_type& out) const { behavior_type::enumerate(*this, out); }

    iterator begin() { return iterator(adapted_.begin()); }
    iterator end()   { return iterator(adapted_.end()); }

    const_iterator begin() const { return const_iterator(adapted_.begin()); }
    const_iterator end()   const { return const_iterator(adapted_.end()); }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_LIST_HPP_INCLUDED

