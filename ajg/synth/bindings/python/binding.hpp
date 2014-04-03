//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_BINDING_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_BINDING_HPP_INCLUDED

#include <memory>
#include <vector>
#include <utility>
#include <stdexcept>

#include <boost/optional.hpp>

#include <boost/python.hpp>
// #include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <ajg/synth/engines/exceptions.hpp>
#include <ajg/synth/engines/django/options.hpp> // (abstract_library)
#include <ajg/synth/bindings/python/detail.hpp>
#include <ajg/synth/bindings/python/adapter.hpp>

namespace ajg {
namespace synth {
namespace python {

namespace py = boost::python;
namespace d = detail;

inline char const* version() {
    return AJG_SYNTH_VERSION_STRING;
}

//
// library
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Options>
struct library : Options::abstract_library_type {
    typedef Options                                      options_type;
    typedef typename options_type::boolean_type          boolean_type;
    typedef typename options_type::string_type           string_type;
    typedef typename options_type::value_type            value_type;
    typedef typename options_type::arguments_type        arguments_type;
    typedef typename options_type::names_type            names_type;
    typedef typename options_type::tag_type              tag_type;
    typedef typename options_type::filter_type           filter_type;
    typedef typename options_type::context_type          context_type;
    typedef typename options_type::tags_type             tags_type;
    typedef typename options_type::filters_type          filters_type;

    explicit library(py::object const& lib) {
        if (py::dict tags = py::extract<py::dict>(lib.attr("tags"))) {
            py::stl_input_iterator<string_type> begin(tags.keys()), end;
            tag_names_ = names_type(begin, end);

            BOOST_FOREACH(string_type const& name, tag_names_) {
                tags_[name] = tag_type(boost::bind(call_tag, tags[name], _1, _2, _3));
            }
        }

        if (py::dict filters = py::extract<py::dict>(lib.attr("filters"))) {
            py::stl_input_iterator<string_type> begin(filters.keys()), end;
            filter_names_ = names_type(begin, end);

            BOOST_FOREACH(string_type const& name, filter_names_) {
                filters_[name] = filter_type(boost::bind(call_filter, filters[name], _1, _2, _3, _4));
            }
        }
    }
    virtual ~library() {}

    virtual boolean_type has_tag(string_type const& name) const { return tags_.find(name) != tags_.end(); }
    virtual boolean_type has_filter(string_type const& name) const { return filters_.find(name) != filters_.end(); }
    virtual names_type   list_tags() const { return tag_names_; }
    virtual names_type   list_filters() const { return filter_names_; }
    virtual tag_type     get_tag(string_type const& name) { return tags_[name]; }
    virtual filter_type  get_filter(string_type const& name) { return filters_[name]; }

    static value_type call_tag(py::object tag, options_type&, context_type*, arguments_type& arguments) {
        std::pair<py::list, py::dict> const& args = d::from_arguments(arguments);
        return tag(*args.first, **args.second);
    }

    static value_type call_filter( py::object            filter
                                 , options_type   const&
                                 , context_type   const*
                                 , value_type     const& value
                                 , arguments_type const& arguments) {
        std::pair<py::list, py::dict> const& args = d::from_arguments(arguments);
        return filter(d::from_value(value), *args.first, **args.second);
    }

  private:

    names_type /*const*/ tag_names_;
    names_type /*const*/ filter_names_;

    tags_type    /*const*/ tags_;
    filters_type /*const*/ filters_;
};

//
// loader
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Options>
struct loader : Options::abstract_loader_type {
    typedef Options                                      options_type;
    typedef typename options_type::string_type           string_type;
    typedef typename options_type::library_type          library_type;

    explicit loader(py::object const& object) : object_(object) {}
    virtual ~loader() {}

    virtual library_type load_library(string_type const& name) {
        return library_type(new library<options_type>(object_(name)));
    }


  private:

    py::object /*const*/ object_;
};

//
// resolver
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Options>
struct resolver : Options::abstract_resolver_type {
    typedef Options                               options_type;
    typedef typename options_type::string_type    string_type;
    typedef typename options_type::context_type   context_type;
    typedef typename options_type::arguments_type arguments_type;

    virtual optional<string_type> resolve( string_type  const& path
                                         , context_type const& context
                                         , options_type const& options
                                         ) {
        try {
            py::object const& result = object_.attr("resolve")(path);
            return d::to_string<string_type>(result);
        }
        catch (...) { // TODO: Catch only Resolver404?
            return none;
        }
    }


    virtual optional<string_type> reverse( string_type    const& name
                                         , arguments_type const& arguments
                                         , context_type   const& context
                                         , options_type   const& options
                                         ) {
        try {
            std::pair<py::list, py::dict> const& args = d::from_arguments(arguments);
            py::object const& result = object_.attr("reverse")(name, *args.first, **args.second); // TODO: current_app
            return d::to_string<string_type>(result);
        }
        catch (...) { // TODO: Catch only NoReverseMatch?
            return none;
        }
    }

    explicit resolver(py::object const& object) : object_(object) {}
    virtual ~resolver() {}

  private:

    py::object /*const*/ object_;
};

template <class MultiTemplate>
struct binding : MultiTemplate /*, boost::noncopyable*/ {

  public:

    typedef binding                              binding_type;
    typedef MultiTemplate                        base_type;
    typedef typename base_type::boolean_type     boolean_type;
    typedef typename base_type::string_type      string_type;
    typedef typename base_type::arguments_type   arguments_type;
    typedef typename base_type::formats_type     formats_type;
    typedef typename base_type::directories_type directories_type;
    typedef typename base_type::options_type     options_type;
    typedef typename base_type::library_type     library_type;
    typedef typename base_type::libraries_type   libraries_type;
    typedef typename base_type::loader_type      loader_type;
    typedef typename base_type::loaders_type     loaders_type;
    typedef typename base_type::resolver_type    resolver_type;
    typedef typename base_type::resolvers_type   resolvers_type;
    typedef py::dict                             context_type;
    typedef std::pair<py::list, py::dict>        args_type;
    typedef py::init< string_type
                    , string_type
                    , py::optional
                        < boolean_type
                        , string_type
                        , py::dict
                        , boolean_type
                        , py::list
                        , py::dict
                        , py::list
                        , py::list
                        >
                    >                           constructor_type;

  public:

    // TODO: Support passing either a string or a file-like object.
    // TODO: Override filters like pprint with Python's own pprint.pprint,
    //       perhaps using a passed-in "overrides" library.
    binding( string_type  const& source
           , string_type  const& engine_name
           , boolean_type const  autoescape    = true
           , string_type  const& default_value = synth::detail::text("")
           // TODO: Rename abbreviated parameters and expose them as kwargs.
           , py::dict     const& fmts          = py::dict()
           , boolean_type const  debug         = false
           , py::list     const& dirs          = py::list()
           , py::dict     const& libs          = py::dict()
           , py::list     const& ldrs          = py::list()
           , py::list     const& rslvrs        = py::list()
           )
        : base_type( source
                   , engine_name
                   , autoescape
                   , default_value
                   , get_formats(fmts)
                   , debug
                   , get_directories(dirs)
                   , get_libraries(libs)
                   , get_loaders(ldrs)
                   , get_resolvers(rslvrs)
                   ) {}

    void render(py::object file, py::dict dictionary) const {
        throw_exception(not_implemented("render"));
        // TODO:
        // streambuf stream(file, buffer_size=0); // (from python_streambuf.h)
        // return base_type::template render<binding>(stream, dictionary);
    }

    string_type render_to_string(py::dict dictionary) const {
        return base_type::template render_to_string<binding>(dictionary);
    }

    void render_to_file(py::str filepath, py::dict dictionary) const {
        string_type const s = py::extract<string_type>(filepath);
        return base_type::template render_to_file<binding>(s, dictionary);
    }

  private:

    inline static formats_type get_formats(py::dict fmts) {
        py::stl_input_iterator<py::tuple> begin(fmts.items()), end;
        formats_type formats;

        BOOST_FOREACH(py::tuple const& item, std::make_pair(begin, end)) {
            string_type const& key = py::extract<string_type>(py::str(item[0]));
            string_type const& fmt = py::extract<string_type>(py::str(item[1]));
            typedef typename formats_type::value_type pair_type;
            formats.insert(pair_type(key, fmt));
        }

        return formats;
    }

    // TODO: Rename these to_* or as_*

    inline static directories_type get_directories(py::list dirs) {
        py::stl_input_iterator<string_type> begin(dirs), end;
        return directories_type(begin, end);
    }

    inline static libraries_type get_libraries(py::dict libs) {
        py::stl_input_iterator<py::tuple> begin(libs.items()), end;
        libraries_type libraries;

        BOOST_FOREACH(py::tuple const& item, std::make_pair(begin, end)) {
            string_type const& key = py::extract<string_type>(py::str(item[0]));
            py::object const& lib = item[1];
            typedef typename libraries_type::value_type pair_type;
            libraries.insert(pair_type(key, library_type(new library<options_type>(lib))));
        }

        return libraries;
    }

    inline static loaders_type get_loaders(py::list ldrs) {
        py::stl_input_iterator<py::object> begin(ldrs), end;
        loaders_type loaders;

        BOOST_FOREACH(py::object const& ldr, std::make_pair(begin, end)) {
            loaders.push_back(loader_type(new loader<options_type>(ldr)));
        }

        return loaders;
    }

    inline static resolvers_type get_resolvers(py::list rslvrs) {
        py::stl_input_iterator<py::object> begin(rslvrs), end;
        resolvers_type resolvers;

        BOOST_FOREACH(py::object const& rslvr, std::make_pair(begin, end)) {
            resolvers.push_back(resolver_type(new resolver<options_type>(rslvr)));
        }

        return resolvers;
    }

  public: // TODO[c++11]: Replace with `friend MultiTemplate;`

    template <class Context>
    inline static Context adapt_context(context_type const& dictionary) {
        Context context;
        py::list const items = dictionary.items();

        // TODO: Replace with stl_input_iterator version.
        for (std::size_t i = 0, n = len(items); i < n; ++i) {
            py::tuple const item = py::extract<py::tuple>(items[i]);
            // py::extract<string_type> key((py::str(item[0])));
            py::extract<string_type> key(item[0]); // TODO: Support non-string keys.
            py::object value(item[1]);

            if (key.check()) {
                context[string_type(key)] = value;
            }
        }

        return context;
    }
};

}}} // namespace ajg::synth::python

#endif // AJG_SYNTH_BINDINGS_PYTHON_BINDING_HPP_INCLUDED
