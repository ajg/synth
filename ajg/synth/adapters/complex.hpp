//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_COMPLEX_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_COMPLEX_HPP_INCLUDED

#include <complex>

#include <ajg/synth/adapters/concrete_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// Specialization for std::complex
//     TODO: Have is_numeric return true.
//     TODO: Use numeric_adapter as base?
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class T> // TODO: Set floating, integral etc. flags based on T.
struct adapter<Value, std::complex<T> >     : concrete_adapter<Value, std::complex<T>, numeric> {
    adapter(std::complex<T> const& adapted) : concrete_adapter<Value, std::complex<T>, numeric>(adapted) {}

    virtual optional<typename Value::number_type> get_number() const { return static_cast<typename Value::number_type>(this->adapted().real()); }
};

}}} // namespace ajg::synth::adapters

namespace std {

template <class T>
struct less<complex<T> > {
    bool operator()(complex<T> const& a, complex<T> const& b) const {
        return a.real() < b.real() && a.imag() < b.imag();
    }
};

} // namespace std

#endif // AJG_SYNTH_ADAPTERS_COMPLEX_HPP_INCLUDED

