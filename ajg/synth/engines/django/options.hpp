//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED

#include <map>
#include <vector>

#include <boost/function.hpp>

#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace django {

//
// options
// TODO: We could get rid of the need for Iterator here by using an offset
//       between iterators (a size_type) instead of the iterator as a key.
////////////////////////////////////////////////////////////////////////////////

template <class Iterator, class Value>
struct options {
    typedef Iterator                           iterator_type;
    typedef Value                              value_type;
    typedef typename value_type::char_type     char_type;
    typedef typename value_type::string_type   string_type;
    typedef typename value_type::boolean_type  boolean_type;
    typedef typename value_type::size_type     size_type;
    typedef std::vector<string_type>           directories_type;
    typedef std::vector<value_type>            array_type;

    typedef void (tag_fn_type)(); // TODO: Figure out what the signature should be.
    typedef value_type (filter_fn_type)(value_type, array_type);
    typedef boost::function<tag_fn_type>       tag_type;
    typedef boost::function<filter_fn_type>    filter_type;

    boolean_type                        autoescape;
    value_type                          default_value;
    directories_type                    directories;
    std::map<string_type, tag_type>     tags;
    std::map<string_type, filter_type>  filters;
    std::map<string_type, string_type>* blocks;
    std::map<iterator_type, size_type>  cycles;
    std::map<iterator_type, value_type> registry;

    options( boolean_type     const  autoescape    = true
           , value_type       const& default_value = detail::text("")
           , directories_type const& directories   = directories_type()
           )
        : autoescape(autoescape)
        , default_value(default_value)
        , directories(directories)
        , tags()
        , filters()
        , blocks(0) {}

    template <class I>
    options(options<I, value_type> const& that)
        : autoescape(that.autoescape)
        , default_value(that.default_value)
        , directories(that.directories)
        , tags(that.tags)
        , filters(that.filters)
        , blocks(that.blocks) {}
        // cycles?, registry?
};

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED
