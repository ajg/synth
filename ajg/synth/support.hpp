//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_SUPPORT_HPP_INCLUDED
#define AJG_SYNTH_SUPPORT_HPP_INCLUDED

#include <ajg/synth/config.hpp>

// Enable nanosecond (96-bit) precision, instead of microsecond (64-bit).
// #define BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG

//
// AJG_SYNTH_IS_DEBUG
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_CONFIG_NO_DEBUG
#    define AJG_SYNTH_IS_DEBUG 1
#else
#    define AJG_SYNTH_IS_DEBUG 0
#endif

//
// AJG_SYNTH_IS_COMPILER_*, AJG_SYNTH_COMPILER_VERSION
//     TODO: AJG_SYNTH_IS(category, value), AJG_SYNTH_HAS(feature), AJG_SYNTH_GET(info)...
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_IS_COMPILER_INTEL   0
#define AJG_SYNTH_IS_COMPILER_CLANG   0
#define AJG_SYNTH_IS_COMPILER_GCC     0
#define AJG_SYNTH_IS_COMPILER_MSVC    0
#define AJG_SYNTH_IS_COMPILER_UNKNOWN 0

#if defined(__INTEL_COMPILER)
#    undef  AJG_SYNTH_IS_COMPILER_INTEL
#    define AJG_SYNTH_IS_COMPILER_INTEL 1
#    define AJG_SYNTH_COMPILER_VERSION  __INTEL_COMPILER // e.g. 900 == 9.0
#elif defined(__clang__)
#    undef  AJG_SYNTH_IS_COMPILER_CLANG
#    define AJG_SYNTH_IS_COMPILER_CLANG 1
#    define AJG_SYNTH_COMPILER_VERSION  (__clang_major__ * 100 + __clang_minor__) // e.g. 301 == 3.1
#elif defined(__GNUC__)
#    undef  AJG_SYNTH_IS_COMPILER_GCC
#    define AJG_SYNTH_IS_COMPILER_GCC  1
#    define AJG_SYNTH_COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__) // e.g. 406 == 4.6
#elif defined(_MSC_VER)
#    undef  AJG_SYNTH_IS_COMPILER_MSVC
#    define AJG_SYNTH_IS_COMPILER_MSVC 1
#    define AJG_SYNTH_COMPILER_VERSION (_MSC_VER - 600) // e.g. 1200 == 12.0 (2013)
#else
#    undef  AJG_SYNTH_IS_COMPILER_UNKNOWN
#    define AJG_SYNTH_IS_COMPILER_UNKNOWN 1
#    define AJG_SYNTH_COMPILER_VERSION    0
#endif

//
// AJG_SYNTH_PLATFORM_*
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_IS_PLATFORM_WINDOWS 0

#if defined(_WIN32) || defined(_WIN64)
#    undef  AJG_SYNTH_IS_PLATFORM_WINDOWS
#    define AJG_SYNTH_IS_PLATFORM_WINDOWS 1
#endif

//
// AJG_SYNTH_UNREACHABLE:
//     NOTE: __assume prevents MSVC's warning C4715.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_IS_COMPILER_MSVC
#    define AJG_SYNTH_UNREACHABLE (AJG_SYNTH_ASSERT(0), (__assume(0)))
#elif AJG_SYNTH_IS_COMPILER_CLANG
#    define AJG_SYNTH_UNREACHABLE (AJG_SYNTH_ASSERT(0), (__builtin_unreachable()))
#elif AJG_SYNTH_IS_COMPILER_GCC && (AJG_SYNTH_COMPILER_VERSION >= 405)
#    define AJG_SYNTH_UNREACHABLE (AJG_SYNTH_ASSERT(0), (__builtin_unreachable()))
#else
#    define AJG_SYNTH_UNREACHABLE (AJG_SYNTH_ASSERT(0), (::std::terminate()))
#endif

//
// AJG_SYNTH_IF_WINDOWS:
//     Picks the first version for Windows platforms, otherwise the second.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_IS_PLATFORM_WINDOWS
#    define AJG_SYNTH_IF_WINDOWS(a, b) a // No parens.
#else
#    define AJG_SYNTH_IF_WINDOWS(a, b) b // No parens.
#endif

//
// AJG_SYNTH_IF_MSVC:
//     Picks the first version for Microsoft compilers, otherwise the second.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_IS_COMPILER_MSVC
#    define AJG_SYNTH_IF_MSVC(a, b) a // No parens.
#else
#    define AJG_SYNTH_IF_MSVC(a, b) b // No parens.
#endif

///
/// AJG_SYNTH_ONLY_DEBUG
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_IS_DEBUG
#    define AJG_SYNTH_ONLY_DEBUG(e) e // No parens.
#else
#    define AJG_SYNTH_ONLY_DEBUG(e)
#endif

///
/// AJG_SYNTH_CONFIG_NO_WINDOWS_H
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef AJG_SYNTH_CONFIG_NO_WINDOWS_H
#    ifndef BOOST_SPIRIT_FILEITERATOR_STD
#    define BOOST_SPIRIT_FILEITERATOR_STD // Prevents the inclusion of <windows.h>
#    endif
#endif

//
// AJG_SYNTH_HAS_CXXABI_H, AJG_SYNTH_HAS_EXECINFO_H:
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_IS_COMPILER_CLANG || AJG_SYNTH_IS_COMPILER_GCC
#    define AJG_SYNTH_HAS_CXXABI_H   1
#    define AJG_SYNTH_HAS_EXECINFO_H 1
#else
#    define AJG_SYNTH_HAS_CXXABI_H   0
#    define AJG_SYNTH_HAS_EXECINFO_H 0
#endif

//
// AJG_SYNTH_HAS_SIGACTION_H
//     TODO: Make this more accurate; not all non-MSVC compilers have sigaction.h.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_IS_COMPILER_MSVC
#    define AJG_SYNTH_HAS_SIGACTION_H 0
#else
#    define AJG_SYNTH_HAS_SIGACTION_H 1
#endif

//
// AJG_SYNTH_EMPTY
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_EMPTY // Nothing.

//
// AJG_SYNTH_TEMPLATE
//     Needed because MSVC isn't terribly standards-conforming.
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_TEMPLATE AJG_SYNTH_IF_MSVC(AJG_SYNTH_EMPTY, template)

//
// AJG_SYNTH_ASSERT:
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_ASSERT(e) (AJG_SYNTH_CONFIG_HANDLE_ASSERT(e))

//
// AJG_SYNTH_THROW:
//     Indirection layer needed because in some cases (e.g. virtual methods with non-void return
//     types) MSVC won't get it through its head that throw_exception doesn't return, even with
//     __declspec(noreturn) which in turns triggers warning C4715 or error C4716.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_IS_COMPILER_MSVC
#    define AJG_SYNTH_THROW(e) (AJG_SYNTH_CONFIG_HANDLE_EXCEPTION(e), AJG_SYNTH_UNREACHABLE)
#else
#    define AJG_SYNTH_THROW(e) (AJG_SYNTH_CONFIG_HANDLE_EXCEPTION(e))
#endif

//
// AJG_SYNTH_HAS_CXX11:
////////////////////////////////////////////////////////////////////////////////////////////////////

#if __cplusplus >= 201103L
#    define AJG_SYNTH_HAS_CXX11 1
#else
#    define AJG_SYNTH_HAS_CXX11 0
#endif

//
// AJG_SYNTH_THREAD_LOCAL:
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_HAS_CXX11
#    define AJG_SYNTH_THREAD_LOCAL thread_local
#elif AJG_SYNTH_IS_COMPILER_MSVC || (AJG_SYNTH_IS_COMPILER_INTEL && AJG_SYNTH_IS_PLATFORM_WINDOWS)
#    define AJG_SYNTH_THREAD_LOCAL __declspec(thread)
#elif AJG_SYNTH_IS_COMPILER_CLANG || AJG_SYNTH_IS_COMPILER_GCC || AJG_SYNTH_IS_COMPILER_INTEL
#    define AJG_SYNTH_THREAD_LOCAL __thread
#else
#    define AJG_SYNTH_THREAD_LOCAL // Cross your fingers and pray.
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NDEBUG) && defined(_DEBUG)
#    error NDEBUG is not compatible with _DEBUG
#endif

#if AJG_SYNTH_IS_DEBUG
#    include <ajg/synth/debug.hpp>
#endif

#endif // AJG_SYNTH_SUPPORT_HPP_INCLUDED
