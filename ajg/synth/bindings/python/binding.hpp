//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_BINDING_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_BINDING_HPP_INCLUDED

#include <memory>
#include <vector>
#include <stdexcept>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <ajg/synth/bindings/python/adapter.hpp>

namespace ajg {
namespace synth {
namespace python {

namespace py = boost::python;

inline char const* version()
{
   return AJG_SYNTH_VERSION_STRING;
}

template <class MultiTemplate>
struct binding : MultiTemplate {

  public:

    typedef MultiTemplate                        base_type;
    typedef typename base_type::boolean_type     boolean_type;
    typedef typename base_type::string_type      string_type;
    typedef typename base_type::directories_type directories_type;
    typedef py::dict                             context_type;

    typedef py::init< string_type
                    , string_type
                    , py::optional
                        < boolean_type
                        , string_type
                        , py::list
                        >
                    > constructor_type;

  public:

    // TODO: Support passing either a string or a file-like object.
    binding( string_type  const& source
           , string_type  const& engine_name
           , boolean_type const  autoescape    = true
           , string_type  const& default_value = detail::text("")
           , py::list     const& directories   = py::list()
           )
        : base_type(source, engine_name, autoescape, default_value, get_directories(directories)) {}

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

    inline static directories_type get_directories(py::list list) {
        py::stl_input_iterator<string_type> begin(list), end;
        return directories_type(begin, end);
    }

  public: // TODO: Replace (in c++11) with `friend MultiTemplate;`

    template <class Context>
    inline static Context adapt_context(context_type const& dictionary) {
        Context context;
        py::list const items = dictionary.items();

        // TODO: Replace with stl_input_iterator version.
        for (std::size_t i = 0, n = len(items); i < n; ++i) {
            py::tuple const item = py::extract<py::tuple>(items[i]);
            // py::extract<string_type> key((py::str(item[0])));
            py::extract<string_type> key(item[0]);
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
