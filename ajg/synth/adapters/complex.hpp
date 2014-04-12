//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_COMPLEX_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_COMPLEX_HPP_INCLUDED

#include <complex>
#include <functional>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// Specialization for std::complex
//     TODO: Have is_numeric return true.
//     TODO: Use numeric_adapter as base?
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, std::complex<T> > : public base_adapter<Behavior> {

    AJG_SYNTH_ADAPTER(std::complex<T>)
    adapted_type adapted_;

  public:

    number_type  to_number()  const { return number_type(adapted_.real()); }
    boolean_type to_boolean() const { return adapted_ != T(0); }
    void input (istream_type& in)        { in >> adapted_; }
    void output(ostream_type& out) const { out << adapted_; }
};

}} // namespace ajg::synth

namespace std {

template <class T>
struct less<complex<T> > {
    bool operator()(complex<T> const& a, complex<T> const& b) const {
        return a.real() < b.real() && a.imag() < b.imag();
    }
};

} // namespace std

#endif // AJG_SYNTH_ADAPTERS_COMPLEX_HPP_INCLUDED

