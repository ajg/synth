//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_BASE_OPTIONS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_BASE_OPTIONS_HPP_INCLUDED

#include <map>
#include <stack>
#include <vector>
#include <utility>

#include <ajg/synth/detail/find.hpp>
#include <ajg/synth/detail/text.hpp>

namespace ajg {
namespace synth {
namespace engines {

//
// base_options
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Context>
struct base_options {
  public:

    typedef Context                                                             context_type;
    typedef base_options                                                        options_type;

    typedef typename context_type::value_type                                   value_type;

    typedef typename value_type::range_type                                     range_type;
    typedef typename value_type::sequence_type                                  sequence_type;
    typedef typename value_type::association_type                               association_type;
    typedef typename value_type::arguments_type                                 arguments_type;
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
    typedef typename traits_type::url_type                                      url_type;
    typedef typename traits_type::symbols_type                                  symbols_type;
    typedef typename traits_type::paths_type                                    paths_type;
    typedef typename traits_type::names_type                                    names_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef std::map<string_type, string_type>                                  formats_type;

    struct abstract_library;
    struct abstract_loader;
    struct abstract_resolver;

    // TODO[c++11]: Use unique_ptr (scoped_ptr won't work because it isn't container-friendly.)
    typedef boost::shared_ptr<abstract_library>                                 library_type;
    typedef boost::shared_ptr<abstract_loader>                                  loader_type;
    typedef boost::shared_ptr<abstract_resolver>                                resolver_type;

    // TODO: Rename builtin_tags::tag_type/builtin_filters::filter_type to make less ambiguous.
    typedef boost::function<void(arguments_type const&, ostream_type&, context_type&)>           renderer_type;
    typedef std::pair<std::vector<string_type>, renderer_type>                                   segment_type;
    typedef std::vector<segment_type>                                                            segments_type;
    typedef boost::function<value_type(value_type const&, arguments_type const&, context_type&)> filter_type;
    typedef struct {
        boost::function<renderer_type(segments_type const&)> function;
        symbols_type                                         middle_names;
        symbols_type                                         last_names;

        inline operator boolean_type() const { return this->function; }
    }                                                                           tag_type;

    typedef std::map<string_type, tag_type>                                     tags_type;
    typedef std::map<string_type, filter_type>                                  filters_type;
    typedef std::map<size_type, renderer_type>                                  renderers_type;

    typedef std::map<string_type, library_type>                                 libraries_type;
    typedef std::vector<loader_type>                                            loaders_type;
    typedef std::vector<resolver_type>                                          resolvers_type;

    typedef struct {
        size_type     position;
        tag_type      tag;
        segments_type segments;
     // boolean_type  proceed;

    }                                                                           entry_type;
    typedef std::stack<entry_type>                                              entries_type;

  private:

    typedef detail::text<string_type>                                           text;

  public:

    inline string_type format(string_type const& name) const {
        // TODO: Throw unknown_format/missing_format instead?
        return detail::find(name, this->formats).get_value_or(text::literal("[missing format]"));
    }

    inline void format(string_type const& name, string_type const& value) {
        this->formats[name] = value;
    }

  public:

    value_type        default_value;
    value_type        error_value;
    formats_type      formats;
    boolean_type      debug;
    paths_type        directories;
    libraries_type    libraries;
    loaders_type      loaders;
    resolvers_type    resolvers;
};


template <class Value>
struct base_options<Value>::abstract_library {
  public:

    virtual boolean_type has_tag(string_type const& name) const    = 0;
    virtual boolean_type has_filter(string_type const& name) const = 0;
    virtual names_type   list_tags() const                         = 0;
    virtual names_type   list_filters() const                      = 0;
    virtual tag_type     get_tag(string_type const& name)          = 0;
    virtual filter_type  get_filter(string_type const& name)       = 0;

    virtual ~abstract_library() {}
};

template <class Value>
struct base_options<Value>::abstract_loader {
  public:

    virtual library_type load_library(string_type const& name) = 0;

    virtual ~abstract_loader() {}
};

template <class Value>
struct base_options<Value>::abstract_resolver {
  public:

    virtual url_type resolve( string_type  const& path
                            , context_type const& context
                            , options_type const& options
                            ) = 0;
    virtual url_type reverse( string_type    const& name
                            , arguments_type const& arguments
                            , context_type   const& context
                            , options_type   const& options
                            ) = 0;

    virtual ~abstract_resolver() {}
};


}}} // namespace ajg::synth::engines

#endif // AJG_SYNTH_ENGINES_BASE_OPTIONS_HPP_INCLUDED

