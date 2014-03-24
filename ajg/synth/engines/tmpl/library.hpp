//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_TMPL_LIBRARY_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_TMPL_LIBRARY_HPP_INCLUDED

#include <boost/mpl/pair.hpp>

#include <boost/fusion/include/vector.hpp>

#include <ajg/synth/engines/tmpl/tags.hpp>

namespace ajg {
namespace synth {
namespace tmpl {

//
// default_library
////////////////////////////////////////////////////////////////////////////////////////////////////

struct default_library : fusion::vector
    < tmpl::comment_tag // NOTE: Part of ctpp, not tmpl.
    , tmpl::if_tag
    , tmpl::include_tag
    , tmpl::loop_tag<>
    , tmpl::unless_tag
    , tmpl::variable_tag
    > {};

}}} // namespace ajg::synth::tmpl

#endif // AJG_SYNTH_ENGINES_TMPL_LIBRARY_HPP_INCLUDED
