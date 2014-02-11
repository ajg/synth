//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/python.hpp>

#include <ajg/synthesis/engines/django.hpp>
#include <ajg/synthesis/bindings/python/binding.hpp>

BOOST_PYTHON_MODULE(synthesis)
{
    using namespace boost::python;
    typedef char                                          Char;
    typedef ajg::synthesis::django::engine<>              Engine;
    typedef ajg::synthesis::python::binding<Char, Engine> Template;
    typedef Template::string_type                         String;

    def("version", ajg::synthesis::python::version);

    class_<Template>("Template", init<String, String>())
        .def("render_to_string", &Template::render_to_string)
    ;
}
