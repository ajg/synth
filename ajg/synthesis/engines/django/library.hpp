//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_ENGINES_DJANGO_LIBRARY_HPP_INCLUDED
#define AJG_SYNTHESIS_ENGINES_DJANGO_LIBRARY_HPP_INCLUDED

#include <boost/mpl/pair.hpp>
#include <boost/mpl/string.hpp>

#include <boost/fusion/include/vector.hpp>

#include <ajg/synthesis/engines/django/tags.hpp>
#include <ajg/synthesis/engines/django/filters.hpp>

namespace ajg {
namespace synthesis {
namespace django {

//
// default_tags
////////////////////////////////////////////////////////////////////////////////

typedef fusion::vector24
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
 // , django::load_tag
    , django::now_tag
 // , django::regroup_tag
    , django::spaceless_tag
    , django::ssi_tag
    , django::templatetag_tag
 // , django::url_tag
    , django::variable_tag
    , django::verbatim_tag
    , django::widthratio_tag
    , django::with_tag
    >
default_tags;

//
// default_filters
////////////////////////////////////////////////////////////////////////////////

typedef fusion::vector48
    < django::add_filter
    , django::addslashes_filter
    , django::capfirst_filter
    , django::center_filter
    , django::cut_filter
 // , django::date_filter
    , django::default_filter
    , django::default_if_none_filter
 // , django::dictsort_filter
 // , django::dictsortreversed_filter
    , django::divisibleby_filter
    , django::escape_filter
    , django::escapejs_filter
    , django::filesizeformat_filter
    , django::first_filter
    , django::fix_ampersands_filter
    , django::floatformat_filter
    , django::force_escape_filter
    , django::get_digit_filter
    , django::iriencode_filter
    , django::join_filter
    , django::last_filter
    , django::length_filter
    , django::length_is_filter
    , django::linebreaks_filter
    , django::linebreaksbr_filter
    , django::linenumbers_filter
    , django::ljust_filter
    , django::lower_filter
    , django::make_list_filter
    , django::phone2numeric_filter
    , django::pluralize_filter
 // , django::pprint_filter
    , django::random_filter
    , django::removetags_filter
    , django::rjust_filter
    , django::safe_filter
    , django::safeseq_filter
    , django::slice_filter
    , django::slugify_filter
    , django::stringformat_filter
    , django::striptags_filter
 // , django::time_filter
 // , django::timesince_filter
 // , django::timeuntil_filter
    , django::title_filter
 // , django::truncatechars_filter
 // , django::truncatechars_html_filter
    , django::truncatewords_filter
 // , django::truncatewords_html_filter
    , django::unordered_list_filter
    , django::upper_filter
    , django::urlencode_filter
    , django::urlize_filter
    , django::urlizetrunc_filter
    , django::wordcount_filter
    , django::wordwrap_filter
    , django::yesno_filter
    >
default_filters;

struct default_library
    : mpl::string<'d','e','f','a','u','l','t'>
    , mpl::pair
        < default_tags
        , default_filters
        >
{};

/*
struct libraries : mpl::list
    < default_library
    >
{};
*/

}}} // namespace ajg::synthesis::django

#endif // AJG_SYNTHESIS_ENGINES_DJANGO_LIBRARY_HPP_INCLUDED
