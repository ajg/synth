//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_LIBRARY_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_LIBRARY_HPP_INCLUDED

#include <ajg/synth/config.hpp>
#include <ajg/synth/vector.hpp>

#include <ajg/synth/engines/django/tags.hpp>
#include <ajg/synth/engines/django/filters.hpp>

namespace ajg {
namespace synth {
namespace django {

//
// default_library
////////////////////////////////////////////////////////////////////////////////////////////////////

struct default_library : fusion::vector
    < django::autoescape_tag
    , django::block_tag
    , django::comment_tag
    , django::csrf_token_tag
    , django::cycle_tag
    , django::debug_tag
    , django::extends_tag
    , django::filter_tag
    , django::firstof_tag
    , django::for_tag
    , django::for_empty_tag
    , django::if_tag
    , django::ifchanged_tag
    , django::ifequal_tag
    , django::ifnotequal_tag
    , django::include_tag
    , django::load_tag
    , django::load_from_tag
    , django::now_tag
    , django::regroup_tag
    , django::spaceless_tag
    , django::ssi_tag
    , django::templatetag_tag
    , django::url_tag
    , django::url_as_tag
    , django::variable_tag
    , django::verbatim_tag
    , django::widthratio_tag
    , django::with_tag
    , django::library_tag
    > {};

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_LIBRARY_HPP_INCLUDED
