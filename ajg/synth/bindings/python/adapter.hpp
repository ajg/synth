//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include <ajg/synth/detail/text.hpp>
#include <ajg/synth/adapters/concrete_adapter.hpp>
#include <ajg/synth/bindings/python/conversions.hpp>

namespace ajg {
namespace synth {
namespace adapters {
namespace {
namespace py = ::boost::python;

using bindings::python::get_string;
using bindings::python::get_datetime;
} // namespace

//
// specialization for boost::python::object
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior>
struct adapter<Behavior, py::object> : concrete_adapter<Behavior, py::object> {
    adapter(py::object const& adapted) : concrete_adapter<Behavior, py::object>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);

 // virtual void input (istream_type& in)        { in >> this->adapted(); }
 // virtual void output(ostream_type& out) const { out << this->adapted(); }
    virtual void output(ostream_type& out) const { out << get_string<traits_type>(this->adapted()); }

    virtual boolean_type  to_boolean()  const { return boolean_type(this->adapted()); }
    virtual datetime_type to_datetime() const { return get_datetime<traits_type>(this->adapted()); }
    virtual range_type    to_range()    const {
        return range_type( begin<const_iterator>(this->adapted())
                         , end<const_iterator>(this->adapted())
                         );
    }

    virtual boolean_type is_boolean() const { return PyBool_Check(this->adapted().ptr()); }
    virtual boolean_type is_string()  const { return PyString_Check(this->adapted().ptr()); }
    virtual boolean_type is_numeric() const { return PyNumber_Check(this->adapted().ptr()); }

    boost::optional<value_type> index(value_type const& what) const {
        // Per https://docs.djangoproject.com/en/dev/topics/templates/#variables
        // TODO: Move this to django::engine.
        // TODO: Support arbitrary values as keys for non-django general case.

        PyObject   *const o = this->adapted().ptr();
        std::string const k = text::narrow(what.to_string());

        // 1. Dictionary lookup
        if (PyMapping_Check(o)) {
            // TODO: If value is a py::object, use PyMapping_HasKey(o, <value>.ptr())
            if (PyMapping_HasKeyString(o, const_cast<char*>(k.c_str()))) {
                return value_type(py::object(this->adapted()[py::str(k)]));
            }
        }

        // 2. Attribute lookup
        // TODO: If value is a py::object, use PyObject_HasAttr(o, <value>.ptr()) and attr(...)
        if (PyObject_HasAttrString(o, k.c_str())) {
            py::object obj = this->adapted().attr(py::str(k));

            // 3. Method call
            if (PyCallable_Check(obj.ptr())) {
                obj = obj();
            }

            return value_type(obj);
        }

        // 4. List-index lookup
        if (PySequence_Check(o)) {
            Py_ssize_t n = static_cast<Py_ssize_t>(what.to_floating());

            if (n < PySequence_Size(o)) {
                return value_type(py::object(this->adapted()[py::long_(n)]));
            }
        }

        return boost::none;
    }

  private:

    typedef typename py::stl_input_iterator<py::object>                         stl_iterator_type;
    typedef detail::text<string_type>                                           text;

  private:

    template <class I>
    inline static I begin(py::object const& obj) {
        if (PyObject_HasAttrString(obj.ptr(), "__iter__")) {
            return I(stl_iterator_type(obj));
        }
        else if (PyObject_HasAttrString(obj.ptr(), "__getitem__")) {
            // TODO: Don't instantiate a list; use a lazy iterator or generator.
            return I(stl_iterator_type(py::list(obj)));
        }
        else {
            std::string const& type = text::narrow(class_name(obj));
            AJG_SYNTH_THROW(std::runtime_error(type + " object is not iterable"));
        }
    }

    template <class I>
    inline static I end(py::object const& obj) {
        return I(stl_iterator_type());
    }

    inline static string_type class_name(py::object const& obj) {
        return get_string<traits_type>(obj.attr("__class__").attr("__name__"));
    }
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
