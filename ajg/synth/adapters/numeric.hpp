//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_NUMERIC_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_NUMERIC_HPP_INCLUDED

#include <cmath>
#include <vector>
#include <iomanip>

#include <boost/cstdint.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>

#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/numeric_adapter.hpp>

namespace ajg {
namespace synth {

#define AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(type)                          \
    template <class Traits>                                                 \
    struct adapter<Traits, type> : public numeric_adapter<Traits, type> {   \
        adapter(type const value) : numeric_adapter<Traits, type>(value) {} \
    }                                                                       \

//
// Integral specializations
////////////////////////////////////////////////////////////////////////////////////////////////////

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(char);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(char signed);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(char unsigned);

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(short);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(short unsigned);

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(int);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(int unsigned);

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long unsigned);

#ifndef BOOST_NO_INTRINSIC_WCHAR_T
#ifndef DISABLE_WIDE_CHAR_SUPPORT
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(wchar_t);
#endif // !DISABLE_WIDE_CHAR_SUPPORT
#endif // !BOOST_NO_INTRINSIC_WCHAR_T

#ifdef BOOST_HAS_LONG_LONG
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long long);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long long unsigned);
#elif defined(BOOST_HAS_MS_INT64)
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(__int64);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(__int64 unsigned);
#endif

//
// Floating-point specializations
////////////////////////////////////////////////////////////////////////////////////////////////////

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(float);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(double);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long double);


}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_NUMERIC_HPP_INCLUDED

