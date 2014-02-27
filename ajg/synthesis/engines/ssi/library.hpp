//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_SSI_LIBRARY_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_SSI_LIBRARY_HPP_INCLUDED

#include <boost/mpl/pair.hpp>

#include <boost/fusion/include/vector.hpp>

#include <ajg/synth/engines/ssi/directives.hpp>

namespace ajg {
namespace synth {
namespace ssi {

//
// default_library
////////////////////////////////////////////////////////////////////////////////

struct default_library : fusion::vector9
    < ssi::config_directive
    , ssi::echo_directive
    , ssi::exec_directive
    , ssi::fsize_directive
    , ssi::flastmod_directive
    , ssi::if_directive<>
    , ssi::include_directive
    , ssi::printenv_directive
    , ssi::set_directive
    > {};

}}} // namespace ajg::synth::ssi

#endif // AJG_SYNTH_ENGINES_SSI_LIBRARY_HPP_INCLUDED
