//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_BINDING_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_BINDING_HPP_INCLUDED

#include <memory>
#include <vector>
#include <utility>
#include <stdexcept>

#include <boost/python.hpp>
#include <boost/optional.hpp>
#include <boost/utility/base_from_member.hpp>

#include <ajg/synth/engines/exceptions.hpp>
#include <ajg/synth/engines/django/options.hpp>
#include <ajg/synth/bindings/python/detail.hpp>
#include <ajg/synth/bindings/python/adapter.hpp>
#include <ajg/synth/bindings/python/loader.hpp>
#include <ajg/synth/bindings/python/library.hpp>
#include <ajg/synth/bindings/python/resolver.hpp>

namespace ajg {
namespace synth {
namespace python {

namespace py = boost::python;
namespace d = detail;

inline char const* version() {
    return AJG_SYNTH_VERSION_STRING;
}

template <class MultiTemplate>
struct binding : private boost::base_from_member<py::object>, MultiTemplate {
  public:

    typedef binding                              binding_type;
    typedef MultiTemplate                        base_type;

    typedef typename binding_type::traits_type   traits_type;

    typedef typename traits_type::boolean_type   boolean_type;
    typedef typename traits_type::size_type      size_type;
    typedef typename traits_type::string_type    string_type;
    typedef typename traits_type::paths_type     paths_type;

    typedef typename base_type::arguments_type   arguments_type;
    typedef typename base_type::formats_type     formats_type;
    typedef typename base_type::options_type     options_type;
    typedef typename base_type::library_type     library_type;
    typedef typename base_type::libraries_type   libraries_type;
    typedef typename base_type::loader_type      loader_type;
    typedef typename base_type::loaders_type     loaders_type;
    typedef typename base_type::resolver_type    resolver_type;
    typedef typename base_type::resolvers_type   resolvers_type;
    typedef py::dict                             context_type;
    typedef py::init< py::object
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
    binding( py::object   const& src
           , string_type  const& engine_name
           , boolean_type const  autoescape    = true
           , string_type  const& default_value = string_type()
           // TODO: Rename abbreviated parameters and expose them as kwargs.
           , py::dict     const& fmts          = py::dict()
           , boolean_type const  debug         = false
           , py::list     const& dirs          = py::list()
           , py::dict     const& libs          = py::dict()
           , py::list     const& ldrs          = py::list()
           , py::list     const& rslvrs        = py::list()
           )
        : boost::base_from_member<py::object>(src) // Keep the object alive.
        , base_type( get_source(boost::base_from_member<py::object>::member) // (src)
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

    void render_to_file(py::object const& file, py::dict const& dictionary) const {
        file.attr("write")(base_type::template render_to_string<binding>(dictionary));
        // XXX: Automatically call flush()?

        /* TODO: Be more intelligent and use something like:

        if (descriptor = file.attr("fileno")) {
            return base_type::template render_to_descriptor<binding>(descriptor, dictionary);
        }
        else {
            streambuf stream(file, buffer_size=0); // (e.g. from python_streambuf.h)
            base_type::template render_to_stream<binding>(stream, dictionary);
        }
        */
    }

    void render_to_path(py::str const& filepath, py::dict const& dictionary) const {
        string_type const s = py::extract<string_type>(filepath);
        return base_type::template render_to_path<binding>(s, dictionary);
    }

    string_type render_to_string(py::dict const& dictionary) const {
        return base_type::template render_to_string<binding>(dictionary);
    }

  private:

    // inline static std::pair<char_type const*, size_type> get_source(py::object const& src) {
    //     if (PyString_Check(src.ptr())) {}
    //     else if (PyUnicode_Check(src.ptr())) {}
    // }

    inline static std::pair<char const*, size_type> get_source(py::object const& src) {
        char*      data;
        Py_ssize_t size;

        if (PyString_AsStringAndSize(src.ptr(), &data, &size) == -1) {
            AJG_SYNTH_THROW(std::invalid_argument("source"));
        }

        return std::pair<char const*, size_type>(data, size);
    }

    // TODO: Rename these to_*

    inline static formats_type get_formats(py::dict const& fmts) {
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

    inline static paths_type get_directories(py::list const& dirs) {
        py::stl_input_iterator<string_type> begin(dirs), end;
        return paths_type(begin, end);
    }

    inline static libraries_type get_libraries(py::dict const& libs) {
        py::stl_input_iterator<py::tuple> begin(libs.items()), end;
        libraries_type libraries;

        BOOST_FOREACH(py::tuple const& item, std::make_pair(begin, end)) {
            string_type const& key = py::extract<string_type>(py::str(item[0]));
            py::object  const& lib = item[1];
            typedef typename libraries_type::value_type pair_type;
            libraries.insert(pair_type(key, library_type(new library<options_type>(lib))));
        }

        return libraries;
    }

    inline static loaders_type get_loaders(py::list const& ldrs) {
        py::stl_input_iterator<py::object> begin(ldrs), end;
        loaders_type loaders;

        BOOST_FOREACH(py::object const& ldr, std::make_pair(begin, end)) {
            loaders.push_back(loader_type(new loader<options_type>(ldr)));
        }

        return loaders;
    }

    inline static resolvers_type get_resolvers(py::list const& rslvrs) {
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
