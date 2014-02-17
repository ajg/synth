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

#include <ajg/synthesis/bindings/python/adapter.hpp>

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
    boost::shared_ptr<django_template_type> django_template_;
    boost::shared_ptr<ssi_template_type>    ssi_template_;
    boost::shared_ptr<tmpl_template_type>   tmpl_template_;

  public:

    binding(string_type source, string_type engine)
        : django_template_(engine == "django" ? new django_template_type(source) : 0)
        , ssi_template_   (engine == "ssi"    ? new ssi_template_type(source)    : 0)
        , tmpl_template_  (engine == "tmpl"   ? new tmpl_template_type(source)   : 0) {

        if (!django_template_ && !ssi_template_ && !tmpl_template_) {
            throw std::invalid_argument("engine");
        }
    }

    string_type render_to_string(py::dict d) const {
        if (django_template_) {
            return django_template_->render_to_string(
                get_context<typename django_template_type::context_type>(d));
        }
        else if (ssi_template_) {
            return ssi_template_->render_to_string(
                get_context<typename ssi_template_type::context_type>(d));
        }
        else if (tmpl_template_) {
            return tmpl_template_->render_to_string(
                get_context<typename tmpl_template_type::context_type>(d));
        }
        AJG_UNREACHABLE;
    }

  private:

    template <class Context>
    inline static Context get_context(py::dict d) {
        Context context;
        py::list const items = d.items();

        for (std::size_t i = 0, n = len(items); i < n; ++i) {
            py::tuple const item = py::extract<py::tuple>(items[i]);
            py::extract<string_type> key((py::str(item[0])));
            py::object value(item[1]);

            if (key.check()) {
                context[string_type(key)] = value;
            }
        }

        return context;
    }
};

}}} // namespace ajg::synthesis::python

#endif // AJG_SYNTHESIS_BINDINGS_PYTHON_BINDING_HPP_INCLUDED
