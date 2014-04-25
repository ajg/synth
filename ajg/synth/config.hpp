//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_CONFIG_HPP_INCLUDED
#define AJG_SYNTH_CONFIG_HPP_INCLUDED

#include <ajg/synth/version.hpp>

//
// AJG_SYNTH_DEFAULT_CHAR_TYPE
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_DEFAULT_CHAR_TYPE
#define AJG_SYNTH_DEFAULT_CHAR_TYPE char
#endif

//
// AJG_SYNTH_NO_WCHAR_T
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(BOOST_NO_INTRINSIC_WCHAR_T) || defined(DISABLE_WIDE_CHAR_SUPPORT)
#    ifndef AJG_SYNTH_NO_WCHAR_T
#    define AJG_SYNTH_NO_WCHAR_T
#    endif
#endif

//
// AJG_SYNTH_NO_LONG_LONG
////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(BOOST_HAS_LONG_LONG)
#    ifndef AJG_SYNTH_NO_LONG_LONG
#    define AJG_SYNTH_NO_LONG_LONG
#    endif
#endif

//
// AJG_SYNTH_COMPILER_*
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_COMPILER_CLANG 0
#define AJG_SYNTH_COMPILER_GCC   0
#define AJG_SYNTH_COMPILER_MSVC  0

#if defined(__clang__)
#    undef  AJG_SYNTH_COMPILER_CLANG
#    define AJG_SYNTH_COMPILER_CLANG 1
#    define AJG_SYNTH_COMPILER_CLANG_VERSION // TODO
#elif defined(__GNUC__)
#    undef  AJG_SYNTH_COMPILER_GCC
#    define AJG_SYNTH_COMPILER_GCC 1
#    define AJG_SYNTH_COMPILER_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__) // e.g. 406 == 4.6
#elif defined(_MSC_VER)
#    undef  AJG_SYNTH_COMPILER_MSVC
#    define AJG_SYNTH_COMPILER_MSVC 1
#    define AJG_SYNTH_COMPILER_MSVC_VERSION // TODO
#endif

//
// AJG_SYNTH_PLATFORM_*
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AJG_SYNTH_PLATFORM_WINDOWS 0

#if defined(_WIN32) || defined(_WIN64)
#    undef  AJG_SYNTH_PLATFORM_WINDOWS
#    define AJG_SYNTH_PLATFORM_WINDOWS 1
#endif

//
// AJG_SYNTH_ASSERT:
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_ASSERT
#define AJG_SYNTH_ASSERT(e) BOOST_ASSERT(e)
#endif

//
// AJG_SYNTH_UNREACHABLE:
//     NOTE: __assume prevents MSVC's warning C4715.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_UNREACHABLE
#    if AJG_SYNTH_COMPILER_MSVC
#        define AJG_SYNTH_UNREACHABLE (AJG_SYNTH_ASSERT(0), (__assume(0)))
#    elif AJG_SYNTH_COMPILER_CLANG
#        define AJG_SYNTH_UNREACHABLE (AJG_SYNTH_ASSERT(0), (__builtin_unreachable()))
#    elif AJG_SYNTH_COMPILER_GCC && (AJG_SYNTH_COMPILER_GCC_VERSION >= 405)
#        define AJG_SYNTH_UNREACHABLE (AJG_SYNTH_ASSERT(0), (__builtin_unreachable()))
#    else
#        define AJG_SYNTH_UNREACHABLE (AJG_SYNTH_ASSERT(0), (::std::terminate()))
#    endif
#endif

//
// AJG_SYNTH_IF_WINDOWS:
//     Picks the first version for Windows platforms, otherwise the second.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_PLATFORM_WINDOWS
#    define AJG_SYNTH_IF_WINDOWS(a, b) a // No parens.
#else
#    define AJG_SYNTH_IF_WINDOWS(a, b) b // No parens.
#endif

//
// AJG_SYNTH_IF_MSVC:
//     Picks the first version for Microsoft compilers, otherwise the second.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_COMPILER_MSVC
#    define AJG_SYNTH_IF_MSVC(a, b) a // No parens.
#else
#    define AJG_SYNTH_IF_MSVC(a, b) b // No parens.
#endif

//
// AJG_SYNTH_HAS_CXXABI_H, AJG_SYNTH_HAS_EXECINFO_H:
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_COMPILER_CLANG || AJG_SYNTH_COMPILER_GCC
#    ifndef AJG_SYNTH_HAS_CXXABI_H
#    define AJG_SYNTH_HAS_CXXABI_H 1
#    endif
#    ifndef AJG_SYNTH_HAS_EXECINFO_H
#    define AJG_SYNTH_HAS_EXECINFO_H 1
#    endif
#else
#    ifndef AJG_SYNTH_HAS_CXXABI_H
#    define AJG_SYNTH_HAS_CXXABI_H 0
#    endif
#    ifndef AJG_SYNTH_HAS_EXECINFO_H
#    define AJG_SYNTH_HAS_EXECINFO_H 0
#    endif
#endif

//
// AJG_SYNTH_COMPILER_MSVC
//     TODO: Make this more accurate; not all non-MSVC compilers have sigaction.h.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_COMPILER_MSVC
#    ifndef AJG_SYNTH_HAS_SIGACTION_H
#    define AJG_SYNTH_HAS_SIGACTION_H 0
#    endif
#else
#    ifndef AJG_SYNTH_HAS_SIGACTION_H
#    define AJG_SYNTH_HAS_SIGACTION_H 1
#    endif
#endif

//
// AJG_SYNTH_THROW_EXCEPTION:
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AJG_SYNTH_THROW_EXCEPTION
#define AJG_SYNTH_THROW_EXCEPTION(e) (::boost::throw_exception(e))
#endif

//
// AJG_SYNTH_THROW:
//     Indirection layer needed because in some cases (e.g. virtual methods with non-void return
//     types) MSVC won't get it through its head that throw_exception doesn't return, even with
//     __declspec(noreturn) which in turns triggers warning C4715 or error C4716.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_COMPILER_MSVC
#    define AJG_SYNTH_THROW(e) (AJG_SYNTH_THROW_EXCEPTION(e), AJG_SYNTH_UNREACHABLE)
#else
#    define AJG_SYNTH_THROW(e) (AJG_SYNTH_THROW_EXCEPTION(e))
#endif

//
// AJG_SYNTH_DEBUG
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NDEBUG) && defined(_DEBUG)
#    error NDEBUG is not compatible with _DEBUG
#elif defined(NDEBUG)
#    ifndef AJG_SYNTH_DEBUG
#    define AJG_SYNTH_DEBUG 0
#    endif
#else
#    ifndef AJG_SYNTH_DEBUG
#    define AJG_SYNTH_DEBUG 1
#    endif
#endif

//
// Debugging-specific stuff
////////////////////////////////////////////////////////////////////////////////////////////////////

#if AJG_SYNTH_DEBUG

#ifndef AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT
#define AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT 1024
#endif

// #define BOOST_THROW_EXCEPTION_CURRENT_FUNCTION

#include <ajg/synth/debug.hpp>

#endif // AJG_SYNTH_DEBUG

#endif // AJG_SYNTH_CONFIG_HPP_INCLUDED
