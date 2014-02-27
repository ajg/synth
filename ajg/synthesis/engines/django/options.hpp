//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED
#define AJG_SYNTHESIS_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED

#include <map>

#include <ajg/synthesis/engines/detail.hpp>

namespace ajg {
namespace synthesis {
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

    value_type                          default_value;
    boolean_type                        autoescape;
    std::map<string_type, string_type>* blocks;
    std::map<iterator_type, size_type>  cycles;
    std::map<iterator_type, value_type> registry;

    options( value_type const& default_value = detail::text("")
           , boolean_type const autoescape   = true
           )
        : default_value(default_value)
        , autoescape(autoescape)
        , blocks(0) {}

    template <class I>
    options(options<I, value_type> const& that)
        : default_value(that.default_value)
        , autoescape(that.autoescape)
        , blocks(that.blocks) {}
        // cycles?, registry?
};

}}} // namespace ajg::synthesis::django

#endif // AJG_SYNTHESIS_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED
