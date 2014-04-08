//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

// TODO? #include <boost/python/detail/wrap_python.hpp> instead
#include <Python.h>             // This prevents a strange macro issue in pyport.h.
#include <ajg/synth/config.hpp> // Must come ahead of everything else, but after Python.h.

#include <boost/python.hpp>

#include <ajg/synth/engines.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/templates.hpp>
#include <ajg/synth/bindings/python/binding.hpp>

BOOST_PYTHON_MODULE(synth)
{
    using namespace boost::python;
    namespace synth = ajg::synth;

    typedef synth::python::binding<synth::detail::multi_template
        < char
        , synth::string_template_identity
        , synth::django::engine
        , synth::ssi::engine<>
        , synth::tmpl::engine<>
        >
    > Template;

    def("version", synth::python::version);

    class_<Template>("Template", Template::constructor_type())
     // .def("render",           &Template::render)
        .def("render_to_string", &Template::render_to_string)
        .def("render_to_file",   &Template::render_to_file)
    ;
}
