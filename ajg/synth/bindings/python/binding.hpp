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

#include <ajg/synth/engines.hpp>
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
               , bindings::base_binding< Traits
                                       , py::object
                                       , templates::char_template
                                       , engines::django::engine
                                       , engines::ssi::engine
                                       , engines::tmpl::engine
                                       > {
  public:

    typedef binding                                                             binding_type;
    typedef Traits                                                              traits_type;
    typedef typename binding::base_binding_type                                 base_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::paths_type                                    paths_type;

    typedef typename base_type::source_type                                     source_type;
    typedef typename base_type::options_type                                    options_type;

    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::library_type                                 library_type;
    typedef typename options_type::libraries_type                               libraries_type;
    typedef typename options_type::loader_type                                  loader_type;
    typedef typename options_type::loaders_type                                 loaders_type;
    typedef typename options_type::resolver_type                                resolver_type;
    typedef typename options_type::resolvers_type                               resolvers_type;
    typedef py::init< py::object
                    , py::str
                    , py::optional
                        < py::dict
                        , boolean_type
                        , py::list
                        , py::dict
                        , py::list
                        , py::list
                        >
                    >                                                           constructor_type;

    typedef typename value_type::arguments_type                                 arguments_type;

    typedef typename context_type::data_type                                    data_type;
    typedef typename context_type::metadata_type                                metadata_type;
    typedef typename context_type::format_type                                  format_type;
    typedef typename context_type::formats_type                                 formats_type;

  private:

    typedef conversions<value_type>                                             c;

  public:

    // TODO: Support passing either a string or a file-like object.
    // TODO: Override filters like pprint with Python's own pprint.pprint,
    //       perhaps using a passed-in "overrides" library.
    binding( py::object   const& src
           , py::str      const& engine
           // TODO: Rename abbreviated parameters and expose them as kwargs.
           // TODO: Change the non-boolean defaults to None (py::object()) to reduce allocations.
           , py::dict     const& fmts        = py::dict()
           , boolean_type const  debug       = false
           , py::list     const& dirs        = py::list()
           , py::dict     const& libs        = py::dict()
           , py::list     const& ldrs        = py::list()
           , py::list     const& rslvrs      = py::list()
           )
        : boost::base_from_member<PyObject*>(py::incref(src.ptr())) // Keep the object alive.
        , base_type( make_source(boost::base_from_member<PyObject*>::member)
                   , c::make_string(engine)
                   , make_options(fmts, debug, dirs, libs, ldrs, rslvrs)
                   ) {}

    ~binding() throw() { py::decref(boost::base_from_member<PyObject*>::member); }

  private:

    inline static options_type make_options( py::dict     const& fmts
                                           , boolean_type const  debug
                                           , py::list     const& dirs
                                           , py::dict     const& libs
                                           , py::list     const& ldrs
                                           , py::list     const& rslvrs
                                           ) {
        options_type options;
        options.defaults    = make_defaults(fmts);
        options.debug       = debug;
        options.directories = c::make_paths(dirs);
        options.libraries   = make_libraries(libs);
        options.loaders     = make_loaders(ldrs);
        options.resolvers   = make_resolvers(rslvrs);
        return options;
    }

  public:

    void render_to_file(py::object const& file, py::object& data) const {
        file.attr("write")(base_type::render_to_string(data));
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

    void render_to_path(py::str const& path, py::object& data) const {
        return base_type::render_to_path(c::make_string(path), data);
    }

    using base_type::render_to_string;
    /*
    string_type render_to_string(py::object& data) const {
        return base_type::render_to_string(data);
    }
    */

  private:

    // TODO: Investigate using something like: (or creating a utf<{8,16,32}>_iterator)
    // inline static std::pair<char_type const*, size_type> make_source(PyObject* const o) {
    //     if (PyString_Check(o)) { use char template }
    //     else if (PyUnicode_Check(o)) { use Py_UNICODE template }
    // }

    inline static source_type make_source(PyObject* const o) {
        char*      data;
        Py_ssize_t size;

        if (PyString_AsStringAndSize(o, &data, &size) == -1) {
            AJG_SYNTH_THROW(std::invalid_argument("source"));
        }

        return std::pair<char const*, size_type>(data, size);
    }

    inline static metadata_type make_defaults(py::dict const& fmts) {
        metadata_type defaults;
        defaults.formats = c::make_formats(fmts);
        return defaults;
    }

    inline static libraries_type make_libraries(py::dict const& libs) {
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

    inline static loaders_type make_loaders(py::list const& ldrs) {
        py::stl_input_iterator<py::object> begin(ldrs), end;
        loaders_type loaders;

        BOOST_FOREACH(py::object const& ldr, std::make_pair(begin, end)) {
            loaders.push_back(loader_type(new loader<options_type>(ldr)));
        }

        return loaders;
    }

    inline static resolvers_type make_resolvers(py::list const& rslvrs) {
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
