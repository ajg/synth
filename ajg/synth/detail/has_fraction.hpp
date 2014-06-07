//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_IS_INTEGER_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_IS_INTEGER_HPP_INCLUDED

#include <cmath>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>

namespace ajg {
namespace synth {
namespace detail {

//
// has_fraction:
//     Determines whether a floating-point number has a fractional part ("decimals").
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
inline typename boost::enable_if<boost::is_integral<T>, bool>::type has_fraction(T const) { return false; }

template <class T>
inline typename boost::disable_if<boost::is_integral<T>, bool>::type has_fraction(T const t) {
    T integral_part;
    return (std::modf)(t, &integral_part) != static_cast<T>(0.0);
}

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_IS_INTEGER_HPP_INCLUDED
