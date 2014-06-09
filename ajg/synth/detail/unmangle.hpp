//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_DETAIL_UNMANGLE_HPP_INCLUDED
#define AJG_SYNTH_DETAIL_UNMANGLE_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <string>
#include <cstdlib>

#include <boost/shared_ptr.hpp>

#if AJG_SYNTH_HAS_CXXABI_H
#include <cxxabi.h>
#endif

namespace ajg {
namespace synth {
namespace detail {

//
// unmangle
////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::string unmangle(std::string const& mangled) {
#if !AJG_SYNTH_HAS_CXXABI_H

    // TODO[c++11]: unique_ptr.
    int status = 0;
    boost::shared_ptr<char> unmangled(abi::__cxa_demangle(mangled.c_str(), 0, 0, &status), std::free);
    return unmangled && status == 0 ? std::string(unmangled.get()) : mangled;

#else

    return mangled;

#endif
}

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_DETAIL_UNMANGLE_HPP_INCLUDED
