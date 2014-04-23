//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED

#include <map>
#include <vector>

#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assign/list_of.hpp>

#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace django {
namespace {
using boost::optional;
}

template <class Options>
struct abstract_loader {
  public:

    typedef abstract_loader                                                     abstract_loader_type;
    typedef Options                                                             options_type;

    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::library_type                                 library_type;

    typedef typename value_type::traits_type                                    traits_type;
    typedef typename traits_type::string_type                                   string_type;

  public:

    virtual library_type load_library(string_type const& name) = 0;

    virtual ~abstract_loader() {}
};

template <class Options>
struct abstract_resolver {
  public:

    typedef abstract_resolver                                                   abstract_resolver_type;
    typedef Options                                                             options_type;

    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::arguments_type                               arguments_type;

    typedef typename value_type::traits_type                                    traits_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::string_type                                   string_type;

  public:

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
  public:

    typedef abstract_library                                                    abstract_library_type;
    typedef Options                                                             options_type;

    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::names_type                                   names_type;
    typedef typename options_type::arguments_type                               arguments_type;

    typedef typename options_type::tag_type                                     tag_type;
    typedef typename options_type::tags_type                                    tags_type;
    typedef typename options_type::filter_type                                  filter_type;
    typedef typename options_type::filters_type                                 filters_type;
    typedef typename options_type::library_type                                 library_type;
    typedef typename options_type::libraries_type                               libraries_type;

    typedef typename value_type::traits_type                                    traits_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::string_type                                   string_type;

  public:

    virtual boolean_type has_tag(string_type const& name) const    = 0;
    virtual boolean_type has_filter(string_type const& name) const = 0;
    virtual names_type   list_tags() const                         = 0;
    virtual names_type   list_filters() const                      = 0;
    virtual tag_type     get_tag(string_type const& name)          = 0;
    virtual filter_type  get_filter(string_type const& name)       = 0;

    virtual ~abstract_library() {}
};

template <class T> struct engine;
template <class K> struct builtin_tags;
template <class K> struct builtin_filters;
template <class E> struct loader;

//
// options
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct options {
  public:

    typedef options                                                             options_type;
    typedef Value                                                               value_type;

    typedef typename value_type::behavior_type                                  behavior_type;
    typedef typename value_type::range_type                                     range_type;
    typedef typename value_type::sequence_type                                  sequence_type;
    typedef typename value_type::mapping_type                                   mapping_type;
    typedef typename value_type::traits_type                                    traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::date_type                                     date_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::paths_type                                    paths_type;
    typedef typename traits_type::names_type                                    names_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef std::map<string_type, value_type>                                   context_type;     // TODO: value_type keys.

    typedef std::pair<sequence_type, mapping_type>                              arguments_type;
    typedef std::map<string_type, string_type>                                  formats_type;

    typedef abstract_library<options_type>                                      abstract_library_type;
    typedef abstract_loader<options_type>                                       abstract_loader_type;
    typedef abstract_resolver<options_type>                                     abstract_resolver_type;

    typedef boost::shared_ptr<abstract_library_type>                            library_type;  // TODO[c++11]: Use unique_ptr?
    typedef boost::shared_ptr<abstract_loader_type>                             loader_type;   // TODO[c++11]: Use unique_ptr?
    typedef boost::shared_ptr<abstract_resolver_type>                           resolver_type; // TODO[c++11]: Use unique_ptr?

    typedef value_type (tag_fn_type)(options_type&, context_type&, arguments_type&);
    typedef value_type (filter_fn_type)(options_type const&, context_type const&, value_type const&, arguments_type const&);

    typedef boost::function<tag_fn_type>                                        tag_type;
    typedef boost::function<filter_fn_type>                                     filter_type;

    typedef std::map<string_type, tag_type>                                     tags_type;
    typedef std::map<string_type, filter_type>                                  filters_type;

    typedef std::map<string_type, library_type>                                 libraries_type;
    typedef std::vector<loader_type>                                            loaders_type;
    typedef std::vector<resolver_type>                                          resolvers_type;

  private:

    typedef size_type                                                           marker_type; // FIXME: pair<filename, size_type>
    typedef std::map<string_type, string_type>                                  blocks_type;
    typedef std::map<marker_type, size_type>                                    cycles_type;
    typedef std::map<marker_type, value_type>                                   changes_type;

  private:

    template <class T> friend struct django::engine;
    template <class K> friend struct django::builtin_tags;
    template <class K> friend struct django::builtin_filters;
    template <class E> friend struct django::loader;

  public:

    options( boolean_type     const  autoescape    = true
           , value_type       const& default_value = string_type()
           , formats_type     const& formats       = formats_type()
           , boolean_type     const  debug         = boolean_type()
           , paths_type       const& directories   = paths_type()
           , libraries_type   const& libraries     = libraries_type()
           , loaders_type     const& loaders       = loaders_type()
           , resolvers_type   const& resolvers     = resolvers_type()
           )
        : autoescape(autoescape)
        , nonbreaking_space(traits_type::literal("&nbsp;"))
        , default_value(default_value)
        , formats(merge_default_formats(formats))
        , debug(debug)
        , directories(directories)
        , libraries(libraries)
        , loaders(loaders)
        , resolvers(resolvers)
        , loaded_tags()
        , loaded_filters()
        , blocks_(0)
        , cycles_()
        , changes_() {}

  public:

    inline static formats_type merge_default_formats(formats_type formats) {
        typedef typename formats_type::value_type format_type;
        static formats_type const defaults = boost::assign::list_of<format_type>
            (traits_type::literal("DATE_FORMAT"),           traits_type::literal("N j, Y"))
            (traits_type::literal("DATETIME_FORMAT"),       traits_type::literal("N j, Y, P"))
            (traits_type::literal("MONTH_DAY_FORMAT"),      traits_type::literal("F j"))
            (traits_type::literal("SHORT_DATE_FORMAT"),     traits_type::literal("m/d/Y"))
            (traits_type::literal("SHORT_DATETIME_FORMAT"), traits_type::literal("m/d/Y P"))
            (traits_type::literal("TIME_FORMAT"),           traits_type::literal("P"))
            (traits_type::literal("YEAR_MONTH_FORMAT"),     traits_type::literal("F Y"))
            ;

        BOOST_FOREACH(format_type const& format, defaults) {
            if (formats.find(format.first) == formats.end()) {
                formats.insert(format);
            }
        }

        return formats;
    }

  public:

    boolean_type      autoescape;
    string_type       nonbreaking_space;
    value_type        default_value;
    formats_type      formats;
    boolean_type      debug;
    paths_type        directories;
    libraries_type    libraries;
    loaders_type      loaders;
    resolvers_type    resolvers;

    inline boolean_type top_level() const {
        return this->blocks_ == 0;
    }

    inline optional<string_type> get_block(string_type const& name) const {
        if (this->top_level()) {
            AJG_SYNTH_THROW(std::invalid_argument("not in a derived template"));
        }
        return detail::find(name, *this->blocks_);
    }

    inline string_type get_base_block() const {
        if (this->base_block_.empty()) {
            AJG_SYNTH_THROW(std::invalid_argument("not in a derived block"));
        }
        else if (optional<string_type> const& block = this->get_block(this->base_block_)) {
            return *block;
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("invalid block"));
        }
    }

    // TODO: Move these to either context or a `state` type:

  protected:

    tags_type     loaded_tags;
    filters_type  loaded_filters;

  private:

    blocks_type*  blocks_;
    cycles_type   cycles_;
    changes_type  changes_;
    string_type   base_block_;
};

}}}} // namespace ajg::synth::engines::django

#endif // AJG_SYNTH_ENGINES_DJANGO_OPTIONS_HPP_INCLUDED
