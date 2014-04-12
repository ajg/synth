//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_CONFIG_HPP_INCLUDED
#define AJG_SYNTH_CONFIG_HPP_INCLUDED

#include <ajg/synth/version.hpp>

#ifndef AJG_SYNTH_DEFAULT_CHAR_TYPE
#define AJG_SYNTH_DEFAULT_CHAR_TYPE char
#endif

#if defined(BOOST_NO_INTRINSIC_WCHAR_T) || defined(DISABLE_WIDE_CHAR_SUPPORT)
#    ifndef AJG_SYNTH_NO_WCHAR_T
#    define AJG_SYNTH_NO_WCHAR_T
#    endif
#endif

#ifndef AJG_SYNTH_SEQUENCE_LIMIT
#define AJG_SYNTH_SEQUENCE_LIMIT 40
#endif

#ifndef AJG_SYNTH_CONSTRUCT_LIMIT
#define AJG_SYNTH_CONSTRUCT_LIMIT 10
#endif

#if AJG_SYNTH_SEQUENCE_LIMIT > 50
#    define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#    define BOOST_FUSION_DONT_USE_PREPROCESSED_FILES
#endif

// #define BOOST_MPL_LIMIT_VECTOR_SIZE AJG_SYNTH_SEQUENCE_LIMIT
#define FUSION_MAX_MAP_SIZE            AJG_SYNTH_SEQUENCE_LIMIT
#define FUSION_MAX_VECTOR_SIZE         AJG_SYNTH_SEQUENCE_LIMIT

#if defined(NDEBUG) && defined(_DEBUG)
#    error NDEBUG is not compatible with _DEBUG
#endif

#ifndef NDEBUG

#ifndef AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT
#define AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT 1024
#endif

// TODO: 1. Rename AJG_SYNTH_*

#if defined(__clang__) || defined(__GNUC__)
#    ifndef HAS_CXXABI_H
#    define HAS_CXXABI_H 1
#    endif
#    ifndef HAS_EXECINFO_H
#    define HAS_EXECINFO_H 1
#    endif
#else
#    ifndef HAS_CXXABI_H
#    define HAS_CXXABI_H 0
#    endif
#    ifndef HAS_EXECINFO_H
#    define HAS_EXECINFO_H 0
#    endif
#endif

// XXX: This is not especially accurate:
#if defined(_MSC_VER)
#    ifndef HAS_SIGACTION
#    define HAS_SIGACTION 0
#    endif
#else
#    ifndef HAS_SIGACTION
#    define HAS_SIGACTION 1
#    endif
#endif

// #define BOOST_THROW_EXCEPTION_CURRENT_FUNCTION

#include <ajg/synth/debug.hpp>

#endif // NDEBUG

#endif // AJG_SYNTH_CONFIG_HPP_INCLUDED
