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

#include <boost/shared_ptr.hpp>

#include <ajg/synth/bindings/python/adapter.hpp>

namespace ajg {
namespace synth {
namespace python {

namespace py = boost::python;

inline char const* version()
{
   return BOOST_PP_STRINGIZE(AJG_SYNTH_VERSION);
}

template <class Char, class Django, class SSI, class TMPL>
struct binding {

  public:

    typedef Char   char_type;
    typedef Django django_engine_type;
    typedef SSI    ssi_engine_type;
    typedef TMPL   tmpl_engine_type;

    typedef string_template<char_type, django_engine_type> django_template_type;
    typedef string_template<char_type, ssi_engine_type>    ssi_template_type;
    typedef string_template<char_type, tmpl_engine_type>   tmpl_template_type;

    typedef bool                      boolean_type;
    typedef std::basic_string<Char>   string_type;
    typedef std::vector<string_type>  directories_type;

    // typedef typename engine_type::context_type context_type;

    typedef py::init< string_type
                    , string_type
                    , py::optional
                        < boolean_type
                        , string_type
                        , py::list
                        >
                    > constructor_type;

  public:

    binding( string_type  const& source
           , string_type  const& engine_name
           , boolean_type const  autoescape     = true
           , string_type  const& default_value  = detail::text("")
           , py::list     const& directories    = py::list()
           )
        : django_template_(engine_name == "django" ? new django_template_type(source) : 0)
        , ssi_template_   (engine_name == "ssi"    ? new ssi_template_type   (source) : 0)
        , tmpl_template_  (engine_name == "tmpl"   ? new tmpl_template_type  (source) : 0)
        , django_options_(autoescape, default_value, get_directories(directories)) {

        if (!django_template_ && !ssi_template_ && !tmpl_template_) {
            throw std::invalid_argument("engine_name");
        }
    }

    string_type render_to_string(py::dict dictionary) const {
        if (django_template_) {
            return django_template_->render_to_string(
                get_context<typename django_template_type::context_type>(dictionary), django_options_);
        }
        else if (ssi_template_) {
            return ssi_template_->render_to_string(
                get_context<typename ssi_template_type::context_type>(dictionary)); // TODO: ssi_options_.
        }
        else if (tmpl_template_) {
            return tmpl_template_->render_to_string(
                get_context<typename tmpl_template_type::context_type>(dictionary)); // TODO: tmpl_options_.
        }
        AJG_UNREACHABLE;
    }

  private:

    template <class Context>
    inline static Context get_context(py::dict dictionary) {
        Context context;
        py::list const items = dictionary.items();

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

    inline static directories_type get_directories(py::list list) {
        py::stl_input_iterator<string_type> begin(list), end;
        return directories_type(begin, end);
    }

  private:

    // TODO: Use unique_ptr?
    boost::shared_ptr<django_template_type>     django_template_;
    boost::shared_ptr<ssi_template_type>        ssi_template_;
    boost::shared_ptr<tmpl_template_type>       tmpl_template_;
    typename django_template_type::options_type django_options_;
};

}}} // namespace ajg::synth::python

#endif // AJG_SYNTH_BINDINGS_PYTHON_BINDING_HPP_INCLUDED
