//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <vector>

#include <boost/python.hpp>

#include <ajg/synthesis/engines/django.hpp>
#include <ajg/synthesis/engines/ssi.hpp>
#include <ajg/synthesis/engines/tmpl.hpp>
#include <ajg/synthesis/bindings/python/binding.hpp>

BOOST_PYTHON_MODULE(synthesis)
{
    using namespace boost::python;
    typedef ajg::synthesis::python::binding
      < char
      , ajg::synthesis::django::engine<>
      , ajg::synthesis::ssi::engine<>
      , ajg::synthesis::tmpl::engine<>
      > Template;

    def("version", ajg::synthesis::python::version);

    class_<Template>("Template", Template::constructor_type())
        .def("render_to_string", &Template::render_to_string)
    ;
}
