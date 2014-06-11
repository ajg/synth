//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_BASE_STATE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_BASE_STATE_HPP_INCLUDED

#include <ajg/synth/detail/text.hpp>

#include <ajg/synth/engines/base_context.hpp>

namespace ajg {
namespace synth {
namespace engines {

//
// base_state
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Match, class Range, class Options>
struct base_state {
  public:

    typedef Match                                                               match_type;
    typedef Range                                                               range_type;
    typedef Options                                                             options_type;
    typedef base_state                                                          base_state_type;

    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::filter_type                                  filter_type;
    typedef typename options_type::filters_type                                 filters_type;
    typedef typename options_type::tag_type                                     tag_type;
    typedef typename options_type::tags_type                                    tags_type;
    typedef typename options_type::renderer_type                                renderer_type;
    typedef typename options_type::renderers_type                               renderers_type;
    typedef typename options_type::loader_type                                  loader_type;
    typedef typename options_type::loaders_type                                 loaders_type;
    typedef typename options_type::resolver_type                                resolver_type;
    typedef typename options_type::resolvers_type                               resolvers_type;
    typedef typename options_type::library_type                                 library_type;
    typedef typename options_type::libraries_type                               libraries_type;
    typedef typename options_type::entry_type                                   entry_type;
    typedef typename options_type::entries_type                                 entries_type;

    typedef typename value_type::sequence_type                                  sequence_type;
    typedef typename value_type::association_type                               association_type;
    typedef typename value_type::traits_type                                    traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::integer_type                                  integer_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::date_type                                     date_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::paths_type                                    paths_type;
    typedef typename traits_type::names_type                                    names_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

  private:

    typedef detail::text<string_type>                                           text;

  public:

    inline explicit base_state() {}
    inline explicit base_state(range_type const& range, options_type const& options)
        : range(range), options(options), loaders_(options.loaders), loaded_libraries_(options.libraries) {}

  public:

    /*
    inline filter_type get_filter(string_type const& name) const {
        if (boost::optional<filter_type> const& filter = detail::find(name, this->loaded_filters_)) {
            return *filter;
        }
        AJG_SYNTH_THROW(missing_filter(text::narrow(name)));
    }

    inline tag_type get_tag(string_type const& name) const {
        if (boost::optional<tag_type> const& tag = detail::find(name, this->loaded_tags_)) {
            return *tag;
        }
        AJG_SYNTH_THROW(missing_tag(text::narrow(name)));
    }
    */

    inline boost::optional<filter_type> get_filter(string_type const& name) const {
        return detail::find(name, this->loaded_filters_);
    }

    inline boost::optional<tag_type> get_tag(string_type const& name) const {
        return detail::find(name, this->loaded_tags_);
    }

    inline boost::optional<renderer_type> get_renderer(size_type const position) const {
        return detail::find(position, this->parsed_renderers_);
    }

    inline void set_renderer(size_type const position, renderer_type const& renderer) {
        this->parsed_renderers_[position] = renderer;
    }

    void load_library(string_type const& library_name, names_type const& names = names_type()) {
        library_type library = this->loaded_libraries_[library_name];

        if (!library) {
            BOOST_FOREACH(loader_type const& loader, this->loaders_) {
                if ((library = loader->load_library(library_name))) {
                    this->loaded_libraries_[library_name] = library;
                    break;
                }
            }
        }

        if (!library) {
            AJG_SYNTH_THROW(missing_library(text::narrow(library_name)));
        }
        else if (!names.empty()) {
            BOOST_FOREACH(string_type const& name, names) {
                if (name.empty()) {
                    AJG_SYNTH_THROW(std::invalid_argument("empty component name"));
                }
                tag_type    const& tag    = library->get_tag(name);
                filter_type const& filter = library->get_filter(name);

                if (!tag && !filter) {
                    AJG_SYNTH_THROW(missing_key(text::narrow(name)));
                }
                if (tag) {
                    this->loaded_tags_[name] = tag;
                }
                if (filter) {
                    this->loaded_filters_[name] = filter;
                }
            }
        }
        else {
            BOOST_FOREACH(string_type const& name, library->list_tags()) {
                if (tag_type const& tag = library->get_tag(name)) {
                    this->loaded_tags_[name] = tag;
                }
                else {
                    AJG_SYNTH_THROW(missing_tag(text::narrow(name)));
                }
            }
            BOOST_FOREACH(string_type const& name, library->list_filters()) {
                if (filter_type const& filter = library->get_filter(name)) {
                    this->loaded_filters_[name] = filter;
                }
                else {
                    AJG_SYNTH_THROW(missing_filter(text::narrow(name)));
                }
            }
        }
    }

  public:

    match_type   match;
    range_type   range;
    options_type options;

  public: // TODO: private:

    loaders_type             loaders_;
    tags_type                loaded_tags_;
    filters_type             loaded_filters_;
    libraries_type           loaded_libraries_;
    renderers_type           parsed_renderers_;

    std::vector<string_type> library_tag_args_;
    entries_type             library_tag_entries_;
    boolean_type             library_tag_continue_;
};

}}} // namespace ajg::synth::engines

#endif // AJG_SYNTH_ENGINES_BASE_STATE_HPP_INCLUDED

