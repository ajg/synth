//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_CONVERTER_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_CONVERTER_HPP_INCLUDED

#if AJG_SYNTH_OBSOLETE

// ---- adapter.hpp

    typedef typename boost::python::stl_input_iterator<value_type> stl_iterator;

// ---- module.cpp

template <class T>
static void* convertible(PyObject* obj) {
    return obj;
}

template <class T>
static void construct(PyObject* obj, boost::python::converter::rvalue_from_python_stage1_data* data) {
    using namespace boost::python;

    void* storage = ((converter::rvalue_from_python_storage<T>*) data)->storage.bytes;

    // Use borrowed to construct the object so that a reference count will be properly handled.
    handle<> hndl(borrowed(obj));
    new (storage) T(object(hndl));

    data->convertible = storage;
}

// ---- BOOST_PYTHON_MODULE

    // TODO: ssi values, tmpl values
    typedef Template::django_template_type::value_type Value;
    converter::registry::push_back(&convertible<Value>, &construct<Value>, type_id<Value>());

#endif // AJG_SYNTH_OBSOLETE

#endif // AJG_SYNTH_BINDINGS_PYTHON_CONVERTER_HPP_INCLUDED
