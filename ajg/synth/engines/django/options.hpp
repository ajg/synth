//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED

#include <map>
#include <vector>

#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace django {

using boost::optional;

template <class Options>
struct abstract_loader {
    typedef abstract_loader                     this_type;
    typedef Options                             options_type;
    typedef typename options_type::string_type  string_type;
    typedef typename options_type::library_type library_type;

    virtual library_type load_library(string_type const& name) = 0;

    virtual ~abstract_loader() {}
};

template <class Options>
struct abstract_resolver {
    typedef abstract_resolver                     this_type;
    typedef Options                               options_type;
    typedef typename options_type::string_type    string_type;
    typedef typename options_type::context_type   context_type;
    typedef typename options_type::arguments_type arguments_type;

    virtual optional<string_type> resolve( string_type  const& path
                                         , context_type const& context
                                         , options_type const& options
                                         ) = 0;
    virtual optional<string_type> reverse( string_type    const& name
                                         , arguments_type const& arguments
                                         , context_type   const& context
                                         , options_type   const& options
                                         ) = 0;

    virtual ~abstract_resolver() {}
};

template <class Options>
struct abstract_library {
    typedef abstract_library                      this_type;
    typedef Options                               options_type;
    typedef typename options_type::boolean_type   boolean_type;
    typedef typename options_type::string_type    string_type;
    typedef typename options_type::value_type     value_type;
    typedef typename options_type::context_type   context_type;
    typedef typename options_type::names_type     names_type;
    typedef typename options_type::array_type     array_type;
    typedef typename options_type::tag_type       tag_type;
    typedef typename options_type::tags_type      tags_type;
    typedef typename options_type::filter_type    filter_type;
    typedef typename options_type::filters_type   filters_type;
    typedef typename options_type::library_type   library_type;
    typedef typename options_type::libraries_type libraries_type;

    virtual boolean_type has_tag(string_type const& name) const    = 0;
    virtual boolean_type has_filter(string_type const& name) const = 0;
    virtual names_type   list_tags() const                         = 0;
    virtual names_type   list_filters() const                      = 0;
    virtual tag_type     get_tag(string_type const& name)          = 0;
    virtual filter_type  get_filter(string_type const& name)       = 0;

    virtual ~abstract_library() {}
};


//
// options
////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct options {

  public:

    typedef options                                options_type;
    typedef Value                                  value_type;
    typedef typename value_type::char_type         char_type;
    typedef typename value_type::string_type       string_type;
    typedef typename value_type::boolean_type      boolean_type;
    typedef typename value_type::size_type         size_type;
    typedef std::map<string_type, value_type>      context_type; // TODO: value_type keys.
    typedef std::vector<string_type>               names_type;
    typedef std::vector<value_type>                array_type;

    typedef std::vector<value_type>                sequence_type;
    typedef std::map<value_type, value_type>       mapping_type;
    typedef std::pair<sequence_type, mapping_type> arguments_type;

    typedef std::map<string_type, string_type>     formats_type;
    typedef std::vector<string_type>               directories_type;
    typedef abstract_library<options_type>         abstract_library_type;
    typedef abstract_loader<options_type>          abstract_loader_type;
    typedef abstract_resolver<options_type>        abstract_resolver_type;

    typedef shared_ptr<abstract_library_type>      library_type;  // TODO[c++11]: Use unique_ptr?
    typedef shared_ptr<abstract_loader_type>       loader_type;   // TODO[c++11]: Use unique_ptr?
    typedef shared_ptr<abstract_resolver_type>     resolver_type; // TODO[c++11]: Use unique_ptr?

    typedef value_type (tag_fn_type)(options_type&, context_type*, array_type&);
    typedef value_type (filter_fn_type)(options_type const&, context_type const*, value_type const&, array_type const&);

    typedef boost::function<tag_fn_type>           tag_type;
    typedef boost::function<filter_fn_type>        filter_type;

    typedef std::map<string_type, tag_type>        tags_type;
    typedef std::map<string_type, filter_type>     filters_type;

    typedef std::map<string_type, library_type>    libraries_type;
    typedef std::vector<loader_type>               loaders_type;
    typedef std::vector<resolver_type>             resolvers_type;

  public:

    boolean_type      autoescape;
    value_type        default_value;
    formats_type      formats;
    boolean_type      debug;
    directories_type  directories;
    tags_type         loaded_tags;
    filters_type      loaded_filters;
    libraries_type    libraries;
    loaders_type      loaders;
    resolvers_type    resolvers;

  public:

    options( boolean_type     const  autoescape    = true
           , value_type       const& default_value = detail::text("")
           , formats_type     const& formats       = formats_type()
           , boolean_type     const  debug         = false
           , directories_type const& directories   = directories_type()
           , libraries_type   const& libraries     = libraries_type()
           , loaders_type     const& loaders       = loaders_type()
           , resolvers_type   const& resolvers     = resolvers_type()
           )
        : autoescape(autoescape)
        , default_value(default_value)
        , formats(default_formats(formats))
        , debug(debug)
        , directories(directories)
        , libraries(libraries)
        , loaders(loaders)
        , resolvers(resolvers)
        , loaded_tags()
        , loaded_filters()
        , blocks(0)
        , cycles()
        , registry() {}

  public:

    inline static formats_type default_formats(formats_type formats) {
        if (!formats.empty()) {
            return formats;
        }

        formats[detail::text("DATE_FORMAT")]           = detail::text("%N %j, %Y");
        formats[detail::text("DATETIME_FORMAT")]       = detail::text("%N %j, %Y, %P");
        formats[detail::text("MONTH_DAY_FORMAT")]      = detail::text("%F %j");
        formats[detail::text("SHORT_DATE_FORMAT")]     = detail::text("%m/%d/%Y");
        formats[detail::text("SHORT_DATETIME_FORMAT")] = detail::text("%m/%d/%Y %P");
        formats[detail::text("TIME_FORMAT")]           = detail::text("%P");
        formats[detail::text("YEAR_MONTH_FORMAT")]     = detail::text("%F %Y");
        return formats;
    }

  // private: // TODO: Make the below private and friend specific tags.

    typedef size_type marker_type; // FIXME: pair<filename, size_type>

    std::map<string_type, string_type>*  blocks;
    std::map<marker_type, size_type>     cycles;
    std::map<marker_type, value_type>    registry;
};

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED
