
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ADAPTERS_BOOL_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ADAPTERS_BOOL_HPP_INCLUDED

#include <chemical/synthesis/adapters/adapter.hpp>

namespace chemical {
namespace synthesis {

//
// specialization for bool
////////////////////////////////////////////////////////////////////////////////

template <class Traits>
struct adapter<Traits, bool>
    : public abstract_adapter<Traits> {

    CHEMICAL_SYNTHESIS_ADAPTER(bool)
    adapted_type const adapted_;

  public:

    boolean_type equal(abstract_type const& that) const { return compare(*this, that); }
    number_type  count() const { return adapted_ ? 1 : 0; }
    boolean_type test()  const { return adapted_; }
    void input (ostream_type& in)        { adapted_ >> in; }
    void output(ostream_type& out) const { out << (adapted_ ? "True" : "False"); }
};

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_ADAPTERS_BOOL_HPP_INCLUDED

