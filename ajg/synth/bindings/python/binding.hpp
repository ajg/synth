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

#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/bindings/base_binding.hpp>
#include <ajg/synth/bindings/python/adapter.hpp>
#include <ajg/synth/bindings/python/loader.hpp>
#include <ajg/synth/bindings/python/library.hpp>
#include <ajg/synth/bindings/python/resolver.hpp>
#include <ajg/synth/bindings/python/conversions.hpp>
#include <ajg/synth/templates/char_template.hpp>

namespace ajg {
namespace synth {
namespace bindings {
namespace python {

namespace py = ::boost::python;

inline char const* version() {
    return AJG_SYNTH_VERSION_STRING;
}

template <class Traits>
struct binding : private boost::base_from_member<PyObject*>
               , bindings::base_binding<Traits, templates::char_template> {
  public:

    typedef binding                                                             binding_type;
    typedef Traits                                                              traits_type;
    typedef typename binding::base_binding_type                                 base_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::paths_type                                    paths_type;

    typedef typename base_type::options_type                                    options_type;

    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::formats_type                                 formats_type;
    typedef typename options_type::library_type                                 library_type;
    typedef typename options_type::libraries_type                               libraries_type;
    typedef typename options_type::loader_type                                  loader_type;
    typedef typename options_type::loaders_type                                 loaders_type;
    typedef typename options_type::resolver_type                                resolver_type;
    typedef typename options_type::resolvers_type                               resolvers_type;
    typedef py::init< py::object
                    , py::str
                    , py::optional
                        < py::object
                        , py::dict
                        , boolean_type
                        , py::list
                        , py::dict
                        , py::list
                        , py::list
                        >
                    >                                                           constructor_type;

    typedef typename value_type::arguments_type                                 arguments_type;

  private:

    typedef conversions<value_type>                                             c;

  public:

    // TODO: Support passing either a string or a file-like object.
    // TODO: Override filters like pprint with Python's own pprint.pprint,
    //       perhaps using a passed-in "overrides" library.
    binding( py::object   const& src
           , py::str      const& engine
           // TODO: Rename abbreviated parameters and expose them as kwargs.
           , py::object   const& replacement = py::str()
           , py::dict     const& fmts        = py::dict()
           , boolean_type const  debug       = false
           , py::list     const& dirs        = py::list()
           , py::dict     const& libs        = py::dict()
           , py::list     const& ldrs        = py::list()
           , py::list     const& rslvrs      = py::list()
           )
        : boost::base_from_member<PyObject*>(py::incref(src.ptr())) // Keep the object alive.
        , base_type( get_source(boost::base_from_member<PyObject*>::member)
                   , c::make_string(engine)
                   , get_options(replacement, fmts, debug, dirs, libs, ldrs, rslvrs)
                   ) {}

    ~binding() throw() { py::decref(boost::base_from_member<PyObject*>::member); }

  private:

    inline static options_type get_options( py::object   const& replacement
                                          , py::dict     const& fmts
                                          , boolean_type const  debug
                                          , py::list     const& dirs
                                          , py::dict     const& libs
                                          , py::list     const& ldrs
                                          , py::list     const& rslvrs
                                          ) {
        options_type options;
        options.default_value = replacement;
        options.formats       = get_formats(fmts);
        options.debug         = debug;
        options.directories   = get_directories(dirs);
        options.libraries     = get_libraries(libs);
        options.loaders       = get_loaders(ldrs);
        options.resolvers     = get_resolvers(rslvrs);
        return options;
    }

  public:

    void render_to_file(py::object const& file, py::object const& object) const {
        context_type context((object));
        file.attr("write")(base_type::render_to_string(context));
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

    void render_to_path(py::str const& path, py::object const& object) const {
        context_type context((object));
        return base_type::render_to_path(c::make_string(path), context);
    }

    string_type render_to_string(py::object const& object) const {
        context_type context((object));
        return base_type::render_to_string(context);
    }

  private:

    // TODO: Investigate using something like: (or creating a utf<{8,16,32}>_iterator)
    // inline static std::pair<char_type const*, size_type> get_source(PyObject* const o) {
    //     if (PyString_Check(o)) { use char template }
    //     else if (PyUnicode_Check(o)) { use Py_UNICODE template }
    // }

    inline static std::pair<char const*, size_type> get_source(PyObject* const o) {
        char*      data;
        Py_ssize_t size;

        if (PyString_AsStringAndSize(o, &data, &size) == -1) {
            AJG_SYNTH_THROW(std::invalid_argument("source"));
        }

        return std::pair<char const*, size_type>(data, size);
    }

    // TODO: Rename these to_*

    inline static formats_type get_formats(py::dict const& fmts) {
        py::stl_input_iterator<py::tuple> begin(fmts.items()), end;
        formats_type formats;

        BOOST_FOREACH(py::tuple const& item, std::make_pair(begin, end)) {
            string_type const& key = c::make_string(item[0]);
            string_type const& fmt = c::make_string(item[1]);
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
            string_type const& key = c::make_string(item[0]);
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
};

}}}} // namespace ajg::synth::bindings::python

#endif // AJG_SYNTH_BINDINGS_PYTHON_BINDING_HPP_INCLUDED
