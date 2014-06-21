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
} // namespace

//
// specialization for boost::python::object
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct adapter<Value, py::object>      : concrete_adapter_without_operators<Value, py::object> {
    adapter(py::object const& adapted) : concrete_adapter_without_operators<Value, py::object>(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Value);

    typedef ajg::synth::bindings::python::conversions<value_type>               c;

/* TODO: Pickle/unpickle or repr.
    virtual boolean_type input (istream_type& istream) const { return false; }
    virtual boolean_type output(ostream_type& ostream) const { return false; }
*/

    virtual boolean_type equal_to(value_type const& that) const { return this->adapted() == that.template as<py::object>(); }
    virtual boolean_type less    (value_type const& that) const { return this->adapted() < that.template as<py::object>(); }

    virtual type_flags flags() const {
        PyObject* const o = this->adapted().ptr();
        type_flags flags  = unspecified;
        if (!o) return flags;

        if (Py_None == o)              flags = type_flags(flags | unit);
        if (PyBool_Check(o))           flags = type_flags(flags | boolean);
        // FIXME: This PyNumber_Check seems to return true for all instances of "classic" Python classes.
        if (PyNumber_Check(o)) {
            if (PyInt_Check(o) ||
                PyLong_Check(o))       flags = type_flags(flags | numeric | integral);
            else if (PyFloat_Check(o)) flags = type_flags(flags | numeric | floating);
            else                       flags = type_flags(flags | numeric);
        }
        if (PyString_Check(o)) {
            if (PyString_Size(o) == 1) flags = type_flags(flags | textual | character);
            else                       flags = type_flags(flags | textual);
        }
        if (PyDate_Check(o) ||
            PyTime_Check(o) ||
            PyDateTime_Check(o))       flags = type_flags(flags | chronologic);
        // TODO: PyTZInfo_Check
        if (PySequence_Check(o))       flags = type_flags(flags | container | sequential);
        if (PyMapping_Check(o))        flags = type_flags(flags | container | associative);
        return flags;
    }

    virtual optional<boolean_type>  get_boolean()    const { return c::make_boolean(this->adapted()); }
    virtual optional<number_type>   get_number()     const { return c::make_number(this->adapted()); }
    virtual optional<datetime_type> get_datetime()   const { return c::make_datetime(this->adapted()); }
    virtual optional<string_type>   get_string()     const { return c::make_string(this->adapted()); }
    virtual optional<range_type>    get_range()      const { // TODO: c::make_range
        return range_type( begin<const_iterator>(this->adapted())
                         , end<const_iterator>(this->adapted())
                         );
    }

    virtual attributes_type attributes() const {
        attributes_type attributes;
        py::list const keys = py::dict(this->adapted()).keys(); // FIXME: Not all mapping types can be converted to a dict.

        // TODO: Replace with stl_input_iterator version.
        for (std::size_t i = 0, n = len(keys); i < n; ++i) {
            py::object const key = keys[i];
            attributes.insert(value_type(key));
        }

        return attributes;
    }

    virtual attribute_type attribute(value_type const& key) const {
        // Per https://docs.djangoproject.com/en/dev/topics/templates/#variables
        // TODO: Move this to django::engine.
        // TODO: Support arbitrary values as keys for non-django general case.

        PyObject   *const o = this->adapted().ptr();
        std::string const k = text::narrow(key.to_string());

        // 1. Dictionary lookup
        if (PyMapping_Check(o)) {
            // TODO: If value is a py::object, use PyMapping_HasKey(o, <value>.ptr())
            if (PyMapping_HasKeyString(o, const_cast<char*>(k.c_str()))) {
                return value_type(py::object(this->adapted()[py::str(k)]));
            }
        }

        // 2. Attribute lookup
        // TODO: If key is a py::object, use PyObject_HasAttr(o, <value>.ptr()) and attr(...)
        if (PyObject_HasAttrString(o, k.c_str())) {
            py::object obj = this->adapted().attr(py::str(k));

            // 3. Method call
            if (PyCallable_Check(obj.ptr())) {
                obj = obj();
            }

            return value_type(obj);
        }

        // 4. List-index lookup
        if (PySequence_Check(o) && key.is_integral()) {
            Py_ssize_t const n = static_cast<Py_ssize_t>(key.to_integer());

            if (n < PySequence_Size(o)) {
                return value_type(py::object(this->adapted()[py::long_(n)]));
            }
        }

        return attribute_type();
    }

    virtual void attribute(value_type const& key, attribute_type const& attribute) const {
        // TODO: Support arbitrary values as keys for non-django general case.

        PyObject   *const o = this->adapted().ptr();
        std::string const k = text::narrow(key.to_string());

        if (PyMapping_Check(o)) {
            // TODO: If key is a py::object, use PyMapping_HasKey, etc.
            if (attribute) {
                PyMapping_SetItemString(o, const_cast<char*>(k.c_str()), c::make_object(*attribute).ptr());
            }
            else {
                if (PyMapping_HasKeyString(o, const_cast<char*>(k.c_str()))) {
                    PyMapping_DelItemString(o, const_cast<char*>(k.c_str()));
                }
            }
        }

        /* TODO:
        if (PySequence_Check(o) && key.is_integral()) {
            Py_ssize_t n = static_cast<Py_ssize_t>(key.to_integer());

            if (n < PySequence_Size(o)) {
                this->adapted()[py::long_(n)] = ...;
            }
        }

        // TODO: If value is a py::object, use PyObject_HasAttr(o, <value>.ptr()) and attr(...)
        if (PyObject_HasAttrString(o, k.c_str())) {
            this->adapted().attr(py::str(k), ...);
        }
        */
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
        return c::make_string(obj.attr("__class__").attr("__name__"));
    }
};

//
// specializations for boost::python::{tuple, list, dict, str}
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct adapter<Value, py::tuple>      : adapter<Value, py::object> {
    adapter(py::tuple const& adapted) : adapter<Value, py::object>(adapted) {}
};

template <class Value>
struct adapter<Value, py::list>      : adapter<Value, py::object> {
    adapter(py::list const& adapted) : adapter<Value, py::object>(adapted) {}
};

template <class Value>
struct adapter<Value, py::dict>      : adapter<Value, py::object> {
    adapter(py::dict const& adapted) : adapter<Value, py::object>(adapted) {}
};

template <class Value>
struct adapter<Value, py::str>      : adapter<Value, py::object> {
    adapter(py::str const& adapted) : adapter<Value, py::object>(adapted) {}
};

}}} // namespace ajg::synth::adapters

#endif // AJG_SYNTH_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
