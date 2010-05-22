
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ENGINES_TMPL_LIBRARY_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ENGINES_TMPL_LIBRARY_HPP_INCLUDED

#include <boost/mpl/pair.hpp>

#include <boost/fusion/include/vector.hpp>

#include <chemical/synthesis/engines/tmpl/tags.hpp>

namespace chemical {
namespace synthesis {
namespace tmpl {

//
// default_library
////////////////////////////////////////////////////////////////////////////////

typedef fusion::vector6
    < tmpl::comment_tag // NB: Part of ctpp, not tmpl.
    , tmpl::if_tag
    , tmpl::include_tag
    , tmpl::loop_tag<>
    , tmpl::unless_tag
    , tmpl::variable_tag
    >
default_library;

}}} // namespace chemical::synthesis::tmpl

#endif // CHEMICAL_SYNTHESIS_ENGINES_TMPL_LIBRARY_HPP_INCLUDED
