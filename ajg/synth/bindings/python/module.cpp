//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/python.hpp>

#include <ajg/synth/engines.hpp>
#include <ajg/synth/bindings/python/binding.hpp>
#include <ajg/synth/templates/multi_template.hpp>

BOOST_PYTHON_MODULE(synth)
{
    using namespace boost::python;
    typedef ajg::synth::python::binding<ajg::synth::detail::multi_template
        < char
        , ajg::synth::django::engine<>
        , ajg::synth::ssi::engine<>
        , ajg::synth::tmpl::engine<>
        >
    > Template;

    def("version", ajg::synth::python::version);

    class_<Template>("Template", Template::constructor_type())
        // .def("render", &Template::render)
        .def("render_to_string", &Template::render_to_string)
        .def("render_to_file", &Template::render_to_file)
    ;
}
