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
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(BOOST_NO_INTRINSIC_WCHAR_T) || defined(DISABLE_WIDE_CHAR_SUPPORT)
#    ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T
#    define AJG_SYNTH_CONFIG_NO_WCHAR_T
#    endif
#endif

//
// AJG_SYNTH_CONFIG_NO_LONG_LONG
////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(BOOST_HAS_LONG_LONG)
#    ifndef AJG_SYNTH_CONFIG_NO_LONG_LONG
#    define AJG_SYNTH_CONFIG_NO_LONG_LONG
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
#endif

//
// AJG_SYNTH_CONFIG_HANDLE_ASSERT:
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_CONFIG_HANDLE_ASSERT
#define AJG_SYNTH_CONFIG_HANDLE_ASSERT(e) (BOOST_ASSERT(e))
#endif

//
// NOTE: Nothing from here down is user-configurable.
////////////////////////////////////////////////////////////////////////////////////////////////////

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

#define AJG_SYNTH_IS_COMPILER_CLANG   0
#define AJG_SYNTH_IS_COMPILER_GCC     0
#define AJG_SYNTH_IS_COMPILER_MSVC    0
#define AJG_SYNTH_IS_COMPILER_UNKNOWN 0

#if defined(__clang__)
#    undef  AJG_SYNTH_IS_COMPILER_CLANG
#    define AJG_SYNTH_IS_COMPILER_CLANG 1
#    define AJG_SYNTH_COMPILER_VERSION  // TODO
#elif defined(__GNUC__)
#    undef  AJG_SYNTH_IS_COMPILER_GCC
#    define AJG_SYNTH_IS_COMPILER_GCC  1
#    define AJG_SYNTH_COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__) // e.g. 406 == 4.6
#elif defined(_MSC_VER)
#    undef  AJG_SYNTH_IS_COMPILER_MSVC
#    define AJG_SYNTH_IS_COMPILER_MSVC 1
#    define AJG_SYNTH_COMPILER_VERSION // TODO
#else
#    undef  AJG_SYNTH_IS_COMPILER_UNKNOWN
#    define AJG_SYNTH_IS_COMPILER_UNKNOWN 1
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

////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NDEBUG) && defined(_DEBUG)
#    error NDEBUG is not compatible with _DEBUG
#endif

#if AJG_SYNTH_IS_DEBUG
#    include <ajg/synth/debug.hpp>
#endif

#endif // AJG_SYNTH_CONFIG_HPP_INCLUDED
