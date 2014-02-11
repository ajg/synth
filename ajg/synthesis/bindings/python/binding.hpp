//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_BINDINGS_PYTHON_BINDING_HPP_INCLUDED
#define AJG_SYNTHESIS_BINDINGS_PYTHON_BINDING_HPP_INCLUDED

#include <memory>

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

namespace ajg {
namespace synthesis {
namespace python {

namespace py = boost::python;

inline char const* version()
{
   return BOOST_PP_STRINGIZE(AJG_SYNTHESIS_VERSION);
}

template <class Char, class Engine>
struct binding {

  public:

    typedef Char                                    char_type;
    typedef Engine                                  engine_type;
    typedef string_template<char_type, engine_type> string_template;
    typedef typename string_template::string_type   string_type;
    typedef typename string_template::context_type  context_type;

  private:

    boost::shared_ptr<string_template> t; // TODO: unique_ptr

  public:

    binding(string_type source, string_type engine)
        : t(new string_template(source)) {}

    string_type render_to_string(py::dict d) const {
        using py::extract;
        context_type context;
        py::list const items = d.items();

        for (std::size_t i = 0, n = len(items); i < n; ++i) {
            // py::tuple const item = extract<py::tuple>(items[i]);
            py::object const item = items[i];
            extract<string_type> key(item[0]);
            extract<string_type> value(item[1]);

            if (key.check() && value.check()) {
                context[string_type(key)] = string_type(value);
            }
        }

        return t->render_to_string(context);
    }
};

}}} // namespace ajg::synthesis::python

#endif // AJG_SYNTHESIS_BINDINGS_PYTHON_BINDING_HPP_INCLUDED
