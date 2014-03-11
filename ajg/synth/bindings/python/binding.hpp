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

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <ajg/synth/engines/exceptions.hpp>
#include <ajg/synth/engines/django/options.hpp> // (abstract_library)
#include <ajg/synth/bindings/python/adapter.hpp>

namespace ajg {
namespace synth {
namespace python {

namespace py = boost::python;

inline char const* version()
{
   return AJG_SYNTH_VERSION_STRING;
}

template <class Options>
struct library : django::abstract_library<Options> {
    typedef django::abstract_library<Options>   base_type;
    typedef typename base_type::string_type     string_type;
    typedef typename base_type::value_type      value_type;
    typedef typename base_type::array_type      array_type;
    typedef typename base_type::names_type      names_type;
    typedef typename base_type::tag_type        tag_type;
    typedef typename base_type::filter_type     filter_type;
    typedef typename base_type::context_type    context_type;
    typedef typename base_type::options_type    options_type;
    typedef typename base_type::tags_type       tags_type;
    typedef typename base_type::filters_type    filters_type;

    explicit library(boost::python::object lib) {
        namespace py = boost::python;

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

    virtual boolean_type has_tag(string_type const& name) const { return tags_.find(name) != tags_.end(); }
    virtual boolean_type has_filter(string_type const& name) const { return filters_.find(name) != filters_.end(); }
    virtual names_type   list_tags() const { return tag_names_; }
    virtual names_type   list_filters() const { return filter_names_; }
    virtual tag_type     load_tag(string_type const& name) { return tags_[name]; }
    virtual filter_type  load_filter(string_type const& name) { return filters_[name]; }
    virtual ~library() {}

    static value_type call_tag(py::object tag, options_type&, context_type*, array_type&) {
        throw_exception(not_implemented("call_tag"));
    }

    static value_type call_filter(py::object filter, options_type&, context_type*, value_type&, array_type&) {
        throw_exception(not_implemented("call_filter"));
    }

  private:

    names_type /*const*/ tag_names_;
    names_type /*const*/ filter_names_;

    tags_type    /*const*/ tags_;
    filters_type /*const*/ filters_;
};


template <class MultiTemplate>
struct binding : MultiTemplate /*, boost::noncopyable*/ {

  public:

    typedef MultiTemplate                        base_type;
    typedef typename base_type::boolean_type     boolean_type;
    typedef typename base_type::string_type      string_type;
    typedef typename base_type::directories_type directories_type;
    typedef typename base_type::options_type     options_type;
    typedef typename base_type::library_type     library_type;
    typedef typename base_type::libraries_type   libraries_type;
    typedef py::dict                             context_type;

    typedef py::init< string_type
                    , string_type
                    , py::optional
                        < boolean_type
                        , string_type
                        , py::list
                        , py::dict
                        >
                    > constructor_type;

  public:

    // TODO: Support passing either a string or a file-like object.
    binding( string_type  const& source
           , string_type  const& engine_name
           , boolean_type const  autoescape    = true
           , string_type  const& default_value = detail::text("")
           , py::list     const& dirs          = py::list()
           , py::dict     const& libs          = py::dict()
           )
        : base_type( source
                   , engine_name
                   , autoescape
                   , default_value
                   , get_directories(dirs)
                   , get_libraries(libs)
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
        string_type const s = boost::python::extract<string_type>(filepath);
        return base_type::template render_to_file<binding>(s, dictionary);
    }

  private:

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

    /*
    inline static libraries_type get_libraries(py::dict libs) {
        libraries_type libraries;
        py::list const items = libs.items();

        // TODO: Replace with stl_input_iterator version.
        for (std::size_t i = 0, n = len(items); i < n; ++i) {
            py::tuple const item = py::extract<py::tuple>(items[i]);
            py::extract<string_type> key(item[0]);
            py::object value(item[1]);

            if (key.check()) {
                libraries[string_type(key)] = new library<options_type>(lib);
            }
        }
    }
    */

    /*
    inline static libraries_type get_libraries(py::list libs) {
        py::stl_input_iterator<py::object> begin(libs), end;
        libraries_type libraries;
        libraries.reserve(py::len(libs));

        BOOST_FOREACH(py::object const& lib, std::make_pair(begin, end)) {
            libraries.push_back(new library<options_type>(lib));
        }

        return libraries;
    }
    */

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
