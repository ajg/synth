//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_COMPLEX_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_COMPLEX_HPP_INCLUDED

#include <complex>

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {

//
// Specialization for std::complex
//     TODO: Have is_numeric return true.
//     TODO: Use numeric_adapter as base?
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, std::complex<T> >  : concrete_adapter<Behavior, std::complex<T> > {
    adapter(std::complex<T> const& adapted) : concrete_adapter<Behavior, std::complex<T> >(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);

    floating_type to_floating() const { return static_cast<floating_type>(this->adapted().real()); }
    boolean_type to_boolean() const { return this->adapted() != T(0); }
    void input (istream_type& in)        { in >> this->adapted(); }
    void output(ostream_type& out) const { out << this->adapted(); }
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

