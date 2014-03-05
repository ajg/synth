//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED

#include <map>
#include <vector>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace django {

template <class Options>
struct abstract_library {
    typedef abstract_library                   this_type;
    typedef Options                            options_type;
    typedef typename options_type::string_type string_type;
    typedef typename options_type::value_type  value_type;
    typedef std::map<string_type, value_type>  context_type; // TODO: Use the engine's.
    typedef std::vector<string_type>           names_type;   // TODO: Use the engine's.
    typedef std::vector<value_type>            array_type;
    typedef shared_ptr<this_type>              library_type; // TODO[c++11]: Use unique_ptr.

    typedef value_type (tag_fn_type)(options_type&, context_type*, array_type&);
    typedef value_type (filter_fn_type)(options_type&, context_type*, value_type&, array_type&);

    typedef boost::function<tag_fn_type>      tag_type;
    typedef boost::function<filter_fn_type>   filter_type;
    typedef std::map<string_type, tag_type>      tags_type;
    typedef std::map<string_type, filter_type>   filters_type;
    typedef std::map<string_type, library_type>  libraries_type;

    virtual names_type  list_tags() const                    = 0;
    virtual names_type  list_filters() const                 = 0;
    virtual tag_type    load_tag(string_type const& name)    = 0;
    virtual filter_type load_filter(string_type const& name) = 0;
    virtual ~abstract_library() {}
};


//
// options
////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct options {
    typedef options                             self_type;
    typedef Value                               value_type;
    typedef typename value_type::char_type      char_type;
    typedef typename value_type::string_type    string_type;
    typedef typename value_type::boolean_type   boolean_type;
    typedef typename value_type::size_type      size_type;
    typedef std::vector<string_type>            directories_type;
    typedef abstract_library<self_type>         abstract_library_type;

    typedef typename abstract_library_type::tag_type       tag_type;
    typedef typename abstract_library_type::tags_type      tags_type;
    typedef typename abstract_library_type::filter_type    filter_type;
    typedef typename abstract_library_type::filters_type   filters_type;
    typedef typename abstract_library_type::library_type   library_type;
    typedef typename abstract_library_type::libraries_type libraries_type;

    boolean_type      autoescape;
    value_type        default_value;
    directories_type  directories;
    tags_type         loaded_tags;
    filters_type      loaded_filters;
    libraries_type    libraries;

    options( boolean_type     const  autoescape    = true
           , value_type       const& default_value = detail::text("")
           , directories_type const& directories   = directories_type()
           , libraries_type   const& libraries     = libraries_type()
           )
        : autoescape(autoescape)
        , default_value(default_value)
        , directories(directories)
        , libraries(libraries)
        , loaded_tags()
        , loaded_filters()
        , blocks(0)
        , cycles()
        , registry() {}

    // TODO: Make the below private and friend specific tags.

    typedef size_type marker_type; // TODO: pair<filename, size_type>

    std::map<string_type, string_type>*  blocks;
    std::map<marker_type, size_type>     cycles;
    std::map<marker_type, value_type>    registry;
};

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED
