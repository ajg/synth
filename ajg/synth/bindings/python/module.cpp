//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

// TODO? #include <boost/python/detail/wrap_python.hpp>
#include <Python.h>             // This prevents a strange macro issue in pyport.h.
#include <ajg/synth/config.hpp> // Must come ahead of everything except Python.h.

#include <boost/python.hpp>

#include <ajg/synth/engines.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/templates.hpp>
#include <ajg/synth/templates/multi_template.hpp>
#include <ajg/synth/bindings/python/binding.hpp>

BOOST_PYTHON_MODULE(synth)
{
    using namespace boost::python;
    namespace synth = ajg::synth;

    typedef synth::default_traits<AJG_SYNTH_DEFAULT_CHAR_TYPE> traits_type;
    typedef synth::python::binding<synth::detail::multi_template
        < traits_type
        , synth::char_template
        , synth::django::engine<traits_type>
        , synth::ssi::engine<traits_type>
        , synth::tmpl::engine<traits_type>
        >
    > Template;

    def("version", synth::python::version);

    class_<Template>("Template", Template::constructor_type())
        .def("render_to_file",   &Template::render_to_file)
        .def("render_to_path",   &Template::render_to_path)
        .def("render_to_string", &Template::render_to_string)
    ;
}
