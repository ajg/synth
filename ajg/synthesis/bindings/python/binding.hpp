//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_BINDINGS_PYTHON_BINDING_HPP_INCLUDED
#define AJG_SYNTHESIS_BINDINGS_PYTHON_BINDING_HPP_INCLUDED

#include <memory>
#include <stdexcept>

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

// #include <ajg/synthesis/bindings/python/adapter.hpp>

namespace ajg {
namespace synthesis {
namespace python {

namespace py = boost::python;

inline char const* version()
{
   return BOOST_PP_STRINGIZE(AJG_SYNTHESIS_VERSION);
}

template <class Char, class Django, class SSI, class TMPL>
struct binding {

  public:

    typedef Char   char_type;
    typedef Django django_type;
    typedef SSI    ssi_type;
    typedef TMPL   tmpl_type;

    typedef string_template<char_type, django_type>     django_template_type;
    typedef string_template<char_type, ssi_type>        ssi_template_type;
    typedef string_template<char_type, tmpl_type>       tmpl_template_type;
    typedef std::basic_string<Char>                     string_type;
    // typedef typename engine_type::context_type          context_type;

  private:

    // TODO: Use unique_ptr?
    boost::shared_ptr<django_template_type> django_template;
    boost::shared_ptr<ssi_template_type>    ssi_template;
    boost::shared_ptr<tmpl_template_type>   tmpl_template;

  public:

    binding(string_type source, string_type engine)
        : django_template(engine == "django" ? new django_template_type(source) : 0)
        , ssi_template   (engine == "ssi"    ? new ssi_template_type(source)    : 0)
        , tmpl_template  (engine == "tmpl"   ? new tmpl_template_type(source)   : 0) {

        if (!django_template && !ssi_template && !tmpl_template) {
            throw std::invalid_argument("engine");
        }
    }

    string_type render_to_string(py::dict d) const {
        if (django_template) {
            return django_template->render_to_string(
                get_context<typename django_template_type::context_type>(d));
        }
        else if (ssi_template) {
            return ssi_template->render_to_string(
                get_context<typename ssi_template_type::context_type>(d));
        }
        else if (tmpl_template) {
            return tmpl_template->render_to_string(
                get_context<typename tmpl_template_type::context_type>(d));
        }
        AJG_UNREACHABLE;
    }

  private:

    template <class Context>
    inline static Context get_context(py::dict d) {
        using py::extract;
        Context context;
        py::list const items = d.items();
        AJG_DUMP(len(items));

        for (std::size_t i = 0, n = len(items); i < n; ++i) {
            py::tuple const item = extract<py::tuple>(items[i]);
            // AJG_DUMP(item);
            extract<string_type> key(item[0]);
            extract<string_type> value(item[1]);
            // AJG_DUMP(key);
            // AJG_DUMP(value);
            AJG_DUMP(key.check());
            AJG_DUMP(value.check());

            if (key.check() && value.check()) {
                context[string_type(key)] = string_type(value);
            }
        }

        return context;
    }
};

}}} // namespace ajg::synthesis::python

#endif // AJG_SYNTHESIS_BINDINGS_PYTHON_BINDING_HPP_INCLUDED
