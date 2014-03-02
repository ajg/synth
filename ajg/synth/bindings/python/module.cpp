//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/python.hpp>

#include <ajg/synth/engines.hpp>
#include <ajg/synth/bindings/python/binding.hpp>
#include <ajg/synth/templates/multi_template.hpp>
#include <ajg/synth/templates/string_template.hpp>

BOOST_PYTHON_MODULE(synth)
{
    using namespace boost::python;
    namespace synth = ajg::synth;
    typedef synth::python::binding<synth::detail::multi_template
        < char
        , synth::string_template_identity
        , synth::django::engine<>
        , synth::ssi::engine<>
        , synth::tmpl::engine<>
        >
    > Template;

    def("version", synth::python::version);

    class_<Template>("Template", Template::constructor_type())
        // .def("render", &Template::render)
        .def("render_to_string", &Template::render_to_string)
        .def("render_to_file", &Template::render_to_file)
    ;
}
