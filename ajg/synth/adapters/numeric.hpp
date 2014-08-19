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

#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/adapters/numeric_adapter.hpp>

namespace ajg {
namespace synth {
namespace adapters {

#define AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER_(type, flags) \
    template <class Value> \
    struct adapter<Value, type>   : numeric_adapter<Value, type, type_flags(flags)> { \
        adapter(type const value) : numeric_adapter<Value, type, type_flags(flags)>(value) {} \
    } \

#define AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(type) \
        AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER_(type, unspecified)

//
// Integral specializations
////////////////////////////////////////////////////////////////////////////////////////////////////

// XXX: These should probably be adapted as character/textual rather than numerically.
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER_(char,          textual | character);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER_(char signed,   textual | character);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER_(char unsigned, textual | character);

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER_(wchar_t, textual | character);
#endif

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(short);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(short unsigned);

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(int);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(int unsigned);

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long unsigned);

#ifdef BOOST_HAS_LONG_LONG
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long long);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long long unsigned);
#elif AJG_SYNTH_IS_COMPILER_MSVC
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(__int64);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(__int64 unsigned);
#endif

//
// Floating-point specializations
////////////////////////////////////////////////////////////////////////////////////////////////////

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(float);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(double);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long double);


}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_ADAPTERS_NUMERIC_HPP_INCLUDED

