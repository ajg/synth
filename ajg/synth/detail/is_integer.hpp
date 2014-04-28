//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_IS_INTEGER_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_IS_INTEGER_HPP_INCLUDED

namespace ajg {
namespace synth {
namespace detail {

//
// is_integer:
//     Determines whether a floating-point number is an integer.
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class FloatingPoint>
inline bool is_integer(FloatingPoint const& fp) {
    FloatingPoint integer_part;
    return (std::modf)(fp, &integer_part) == FloatingPoint(0.0);
}

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_IS_INTEGER_HPP_INCLUDED
