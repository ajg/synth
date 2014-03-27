//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_CONFIG_HPP_INCLUDED
#define AJG_SYNTH_CONFIG_HPP_INCLUDED

#define AJG_SYNTH_VERSION_MAJOR 0
#define AJG_SYNTH_VERSION_MINOR 9
#define AJG_SYNTH_VERSION_PATCH 0

#define AJG_SYNTH_VERSION        AJG_SYNTH_VERSION_MAJOR.AJG_SYNTH_VERSION_MINOR.AJG_SYNTH_VERSION_PATCH
#define AJG_SYNTH_VERSION_TUPLE  (AJG_SYNTH_VERSION_MAJOR, AJG_SYNTH_VERSION_MINOR, AJG_SYNTH_VERSION_PATCH)
#define AJG_SYNTH_VERSION_STRING BOOST_PP_STRINGIZE(AJG_SYNTH_VERSION)

#ifndef AJG_SYNTH_SEQUENCE_LIMIT
#define AJG_SYNTH_SEQUENCE_LIMIT 60
#endif

#ifndef AJG_SYNTH_CONSTRUCT_LIMIT
#define AJG_SYNTH_CONSTRUCT_LIMIT 10
#endif

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_FUSION_DONT_USE_PREPROCESSED_FILES

// #define BOOST_MPL_LIMIT_VECTOR_SIZE AJG_SYNTH_SEQUENCE_LIMIT
#define FUSION_MAX_MAP_SIZE            AJG_SYNTH_SEQUENCE_LIMIT
#define FUSION_MAX_VECTOR_SIZE         AJG_SYNTH_SEQUENCE_LIMIT

#ifndef NDEBUG

#ifndef AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT
#define AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT 1024
#endif

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

#include <ajg/synth/debug.hpp>

#endif // NDEBUG

#include <boost/preprocessor/stringize.hpp>

#endif // AJG_SYNTH_CONFIG_HPP_INCLUDED
