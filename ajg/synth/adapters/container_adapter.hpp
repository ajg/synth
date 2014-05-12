//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_CONTAINER_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_CONTAINER_ADAPTER_HPP_INCLUDED

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {

template <class Behavior, class Adapted>
struct container_adapter : concrete_adapter<Behavior, Adapted> {
  protected:

    AJG_SYNTH_ADAPTER_TYPEDEFS(Adapted);
    container_adapter(adapted_type const& adapted) : concrete_adapter<Behavior, Adapted>(adapted) {}

  public:

    boolean_type to_boolean() const { return !this->adapted_.empty(); }
    void output(ostream_type& out) const { behavior_type::enumerate(*this, out); }

    // TODO[c++11]: Use std::begin & std::end.

    iterator begin() { return iterator(this->adapted_.begin()); }
    iterator end()   { return iterator(this->adapted_.end()); }

    const_iterator begin() const { return const_iterator(this->adapted_.begin()); }
    const_iterator end()   const { return const_iterator(this->adapted_.end()); }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_CONTAINER_ADAPTER_HPP_INCLUDED

