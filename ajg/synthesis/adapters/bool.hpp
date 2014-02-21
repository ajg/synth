//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_ADAPTERS_BOOL_HPP_INCLUDED
#define AJG_SYNTHESIS_ADAPTERS_BOOL_HPP_INCLUDED

#include <ajg/synthesis/adapters/adapter.hpp>

namespace ajg {
namespace synthesis {

//
// specialization for bool
////////////////////////////////////////////////////////////////////////////////

template <class Traits>
struct adapter<Traits, bool>
    : public abstract_adapter<Traits> {

    AJG_SYNTHESIS_ADAPTER(bool)
    adapted_type const adapted_;

  public:

    boolean_type equal(abstract_type const& that) const { return this->compare(*this, that); }
    number_type  count() const { return adapted_ ? 1 : 0; }
    boolean_type test()  const { return adapted_; }
    void input (istream_type& in)        { in >> adapted_; }
    void output(ostream_type& out) const { out << (adapted_ ? "True" : "False"); }
};

}} // namespace ajg::synthesis

#endif // AJG_SYNTHESIS_ADAPTERS_BOOL_HPP_INCLUDED

