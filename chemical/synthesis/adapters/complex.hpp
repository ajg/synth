
//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ADAPTERS_COMPLEX_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ADAPTERS_COMPLEX_HPP_INCLUDED

#include <complex>

#include <chemical/synthesis/adapters/adapter.hpp>

namespace chemical {
namespace synthesis {

//
// specialization for std::complex
////////////////////////////////////////////////////////////////////////////////

template <class Traits, class T>
struct adapter<Traits, std::complex<T> >
    : public abstract_adapter<Traits> {

    CHEMICAL_SYNTHESIS_ADAPTER(std::complex<T>)
    adapted_type const adapted_;

  public:

    number_type  count() const { return number_type(adapted_.real()); }
    boolean_type test()  const { return adapted_ != T(0); }
    void input (ostream_type& in)  const { adapted_ >> in; }
    void output(ostream_type& out) const { out << adapted_; }
};

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_ADAPTERS_COMPLEX_HPP_INCLUDED

