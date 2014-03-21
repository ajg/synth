//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_LOADER_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_LOADER_HPP_INCLUDED

#include <vector>

namespace ajg {
namespace synth {
namespace django {

//
// default_loader
////////////////////////////////////////////////////////////////////////////////

struct default_loader {
    template <class Engine>
    static void load( typename Engine::context_type&      context
                    , typename Engine::options_type&      options
                    , typename Engine::string_type const& library_name
                    , typename Engine::names_type const*  names
                    ) {

        typedef typename Engine::options_type       options_type;
        typedef typename options_type::string_type  string_type;
        typedef typename options_type::value_type   value_type;
        typedef typename options_type::tag_type     tag_type;
        typedef typename options_type::filter_type  filter_type;;
        typedef typename options_type::library_type library_type;
        typedef typename options_type::loader_type  loader_type;

        library_type library = options.libraries[library_name];

        if (!library) {
            BOOST_FOREACH(loader_type const& loader, options.loaders) {
                if ((library = loader->load_library(library_name))) {
                    options.libraries[library_name] = library;
                    break;
                }
            }
        }

        if (!library) {
            throw_exception(missing_library(library_name));
        }
        else if (names) {
            BOOST_FOREACH(string_type const& name, *names) {
                tag_type    const& tag    = library->get_tag(name);
                filter_type const& filter = library->get_filter(name);

                if (!tag && !filter) {
                    throw_exception(missing_variable(name));
                }
                if (tag) {
                    options.loaded_tags[name] = tag;
                }
                if (filter) {
                    options.loaded_filters[name] = filter;
                }
            }
        }
        else {
            BOOST_FOREACH(string_type const& name, library->list_tags()) {
                if (tag_type const& tag = library->get_tag(name)) {
                    options.loaded_tags[name] = tag;
                }
                else {
                    throw_exception(missing_variable(name));
                }
            }
            BOOST_FOREACH(string_type const& name, library->list_filters()) {
                if (filter_type const& filter = library->get_filter(name)) {
                    options.loaded_filters[name] = filter;
                }
                else {
                    throw_exception(missing_variable(name));
                }
            }
        }
    }
};

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_LOADER_HPP_INCLUDED
