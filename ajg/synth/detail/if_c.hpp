//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_IF_C_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_IF_C_HPP_INCLUDED

namespace ajg {
namespace synth {
namespace detail {

//
// if_c
////////////////////////////////////////////////////////////////////////////////////////////////////

template <bool C, class X, class Y> struct if_c { typedef X type; };
template <class X, class Y>         struct if_c<false, X, Y> { typedef Y type; };

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_IF_C_HPP_INCLUDED
