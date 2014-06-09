//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED

namespace boost {

// Forward declaration.
template <class T, std::size_t N> class array;

}

#include <ajg/synth/adapters/concrete_adapter.hpp>
#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

//
// specialization for native arrays of statically known size
////////////////////////////////////////////////////////////////////////////////////////////////////

// NOTE: A reference is stored and not a copy because initializing an array within a constructor's
//       initializer list ranges from difficult to impossible in C++03.
// TODO[c++11]: Store array by value.


template <class Value, class T, std::size_t N>
struct adapter<Value, T[N]>        : range_adapter<Value, T const (&)[N], adapter<Value, T[N]>, T const*, sequential> {
    adapter(T const (&adapted)[N]) : range_adapter<Value, T const (&)[N], adapter<Value, T[N]>, T const*, sequential>(adapted) {}

    virtual T const* begin() const { return static_cast<T const*>(this->adapted()); }
    virtual T const* end()   const { return static_cast<T const*>(this->adapted()) + N; }
};

//
// specialization for native arrays of statically unknown size
//     TODO[c++14]: Remove support for this nastiness and defer to std::dynarray.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class T>
struct adapter<Value, T[]>                              : range_adapter<Value, T* const, adapter<Value, T[]>, T const*, sequential> {
    adapter(T* const adapted, std::size_t const length) : range_adapter<Value, T* const, adapter<Value, T[]>, T const*, sequential>(adapted), length_(length) {}

    virtual T const* begin() const { return static_cast<T const*>(this->adapted()); }
    virtual T const* end()   const { return static_cast<T const*>(this->adapted()) + this->length_; }

  private:

    std::size_t length_;
};

//
// specialization for boost::array
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value, class T, std::size_t N>
struct adapter<Value, boost::array<T, N> >     : container_adapter<Value, boost::array<T, N>, sequential> {
    adapter(boost::array<T, N> const& adapted) : container_adapter<Value, boost::array<T, N>, sequential>(adapted) {}
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED

