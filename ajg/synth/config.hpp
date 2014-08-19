//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_CONFIG_HPP_INCLUDED
#define AJG_SYNTH_CONFIG_HPP_INCLUDED

#include <ajg/synth/version.hpp>

//
// AJG_SYNTH_CONFIG_DEFAULT_CHAR_TYPE
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_CONFIG_DEFAULT_CHAR_TYPE
#define AJG_SYNTH_CONFIG_DEFAULT_CHAR_TYPE char
#endif

//
// AJG_SYNTH_CONFIG_MAX_TEMPLATE_DEPTH:
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_CONFIG_MAX_TEMPLATE_DEPTH
#define AJG_SYNTH_CONFIG_MAX_TEMPLATE_DEPTH 1024
#endif

//
// AJG_SYNTH_CONFIG_MAX_FRAMES:
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_CONFIG_MAX_FRAMES
#define AJG_SYNTH_CONFIG_MAX_FRAMES 1024
#endif

//
// AJG_SYNTH_CONFIG_NO_WCHAR_T
//     FIXME: Needs boost/config.hpp, which is not yet available here.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(BOOST_NO_INTRINSIC_WCHAR_T) \
 || defined(BOOST_NO_STD_WSTREAMBUF) \
 || defined(BOOST_NO_STD_WSTRING) \
 || defined(DISABLE_WIDE_CHAR_SUPPORT)
#    ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T
#    define AJG_SYNTH_CONFIG_NO_WCHAR_T
#    endif
#endif

//
// AJG_SYNTH_CONFIG_NO_DEBUG
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NDEBUG)
#    ifndef AJG_SYNTH_CONFIG_NO_DEBUG
#    define AJG_SYNTH_CONFIG_NO_DEBUG
#    endif
#endif

//
// AJG_SYNTH_CONFIG_HANDLE_EXCEPTION
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_CONFIG_HANDLE_EXCEPTION
#define AJG_SYNTH_CONFIG_HANDLE_EXCEPTION(e) (::boost::throw_exception(e))
#include <boost/throw_exception.hpp>
#endif

//
// AJG_SYNTH_CONFIG_HANDLE_ASSERT:
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_CONFIG_HANDLE_ASSERT
#define AJG_SYNTH_CONFIG_HANDLE_ASSERT(e) (BOOST_ASSERT(e))
#include <boost/assert.hpp>
#endif

#endif // AJG_SYNTH_CONFIG_HPP_INCLUDED
