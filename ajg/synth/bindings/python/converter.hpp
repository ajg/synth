//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_CONVERTER_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_CONVERTER_HPP_INCLUDED

#if AJG_SYNTH_OBSOLETE

// ---- adapter.hpp

    typedef typename py::stl_input_iterator<value_type> stl_iterator;

// ---- module.cpp

template <class T>
static void* convertible(PyObject* obj) {
    return obj;
}

template <class T>
static void construct(PyObject* obj, py::converter::rvalue_from_python_stage1_data* data) {
    void* storage = ((py::converter::rvalue_from_python_storage<T>*) data)->storage.bytes;

    // Use borrowed to construct the object so that a reference count will be properly handled.
    py::handle<> hndl(py::borrowed(obj));
    new (storage) T(py::object(hndl));

    data->convertible = storage;
}

// ---- BOOST_PYTHON_MODULE

    // TODO: ssi values, tmpl values
    typedef binding_type::django_template_type::value_type value_type;
    py::converter::registry::push_back(&convertible<value_type>, &construct<value_type>, py::type_id<value_type>());

#endif // AJG_SYNTH_OBSOLETE

#endif // AJG_SYNTH_BINDINGS_PYTHON_CONVERTER_HPP_INCLUDED
