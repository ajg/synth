
//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ENGINES_SSI_LIBRARY_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ENGINES_SSI_LIBRARY_HPP_INCLUDED

#include <boost/mpl/pair.hpp>

#include <boost/fusion/include/vector.hpp>

#include <chemical/synthesis/engines/ssi/directives.hpp>

namespace chemical {
namespace synthesis {
namespace ssi {

//
// default_library
////////////////////////////////////////////////////////////////////////////////

typedef fusion::vector9
    < ssi::config_directive
    , ssi::echo_directive
    , ssi::exec_directive
    , ssi::fsize_directive
    , ssi::flastmod_directive
    , ssi::if_directive<>
    , ssi::include_directive
    , ssi::printenv_directive
    , ssi::set_directive
    >
default_library;

}}} // namespace chemical::synthesis::ssi

#endif // CHEMICAL_SYNTHESIS_ENGINES_SSI_LIBRARY_HPP_INCLUDED
