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
    typedef py::init<py::object, py::object, py::object>                        constructor_type;

    typedef typename value_type::arguments_type                                 arguments_type;

    typedef typename context_type::data_type                                    data_type;
    typedef typename context_type::metadata_type                                metadata_type;
    typedef typename context_type::format_type                                  format_type;
    typedef typename context_type::formats_type                                 formats_type;

  private:

    typedef conversions<value_type>                                             c;

  public:

    // TODO: Support passing either a string or a file-like object.
    // TODO: Override e.g. pprint_filter with Python's own pprint.pprint?
    binding(py::object const& src, py::object const& eng, py::object const& opts)
        : boost::base_from_member<PyObject*>(py::incref(src.ptr())) // Keep the object alive.
        , base_type( c::make_buffer(boost::base_from_member<PyObject*>::member)
                   , c::make_string(eng)
                   , make_options(opts, base_type::default_options())
                   ) {}

    ~binding() throw() { py::decref(boost::base_from_member<PyObject*>::member); }

  public: // private:

    inline static options_type make_options(py::object const& opts, options_type options) {
        if (opts) {
            // TODO: boolean_type  metadata.caseless
            // TODO: boolean_type  metadata.safe
            // TODO: string_type   metadata.application
            // TODO: timezone_type metadata.timezone
            // TODO: language_type metadata.language
            // TODO: boolean_type  metadata.localized
            if (PyMapping_HasKeyString(opts.ptr(), const_cast<char*>("formats")))     options.metadata.formats = c::make_formats(py::dict(opts["formats"]));
            if (PyMapping_HasKeyString(opts.ptr(), const_cast<char*>("debug")))       options.debug            = boolean_type(opts["debug"]);
            if (PyMapping_HasKeyString(opts.ptr(), const_cast<char*>("directories"))) options.directories      = c::make_paths(py::list(opts["directories"]));
            if (PyMapping_HasKeyString(opts.ptr(), const_cast<char*>("libraries")))   options.libraries        = make_libraries(py::dict(opts["libraries"]));
            if (PyMapping_HasKeyString(opts.ptr(), const_cast<char*>("loaders")))     options.loaders          = make_loaders(py::list(opts["loaders"]));
            if (PyMapping_HasKeyString(opts.ptr(), const_cast<char*>("resolvers")))   options.resolvers        = make_resolvers(py::list(opts["resolvers"]));
        }
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

    void render_to_path(py::object const& path, py::object& data) const {
        return base_type::render_to_path(c::make_string(path), data);
    }

    string_type render_to_string(py::object& data) const {
        return base_type::render_to_string(data);
    }

    static void set_default_options(py::dict const& opts) {
        options_type const options = make_options(opts, base_type::default_options());
        base_type::default_options(&options);
    }

  private:

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
