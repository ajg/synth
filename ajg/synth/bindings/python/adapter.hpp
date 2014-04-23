//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include <ajg/synth/adapters/adapter.hpp>
#include <ajg/synth/bindings/python/detail.hpp>

namespace ajg {
namespace synth {
namespace {

namespace py = boost::python;
namespace d  = ajg::synth::bindings::python::detail;

} // namespace

//
// specialization for boost::python::object
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior>
struct adapter<Behavior, py::object>
    : public base_adapter<Behavior> {

    typedef py::object object_type;
    AJG_SYNTH_ADAPTER(object_type)
    object_type adapted_;

  private:

    typedef typename py::stl_input_iterator<object_type> stl_iterator_type;

  public:

    virtual boolean_type to_boolean() const { return boolean_type(adapted_); }
    virtual datetime_type to_datetime() const { return d::to_datetime<traits_type>(adapted_); }

 // virtual void input (istream_type& in)        { in >> adapted_; }
 // virtual void output(ostream_type& out) const { out << adapted_; }
    virtual void output(ostream_type& out) const { out << d::to_string<string_type>(adapted_); }

    virtual iterator begin() { return begin<iterator>(adapted_); }
    virtual iterator end()   { return end<iterator>(adapted_); }

    virtual const_iterator begin() const { return begin<const_iterator>(adapted_); }
    virtual const_iterator end()   const { return end<const_iterator>(adapted_); }

    virtual boolean_type is_boolean() const { return PyBool_Check(adapted_.ptr()); }
    virtual boolean_type is_string()  const { return PyString_Check(adapted_.ptr()); }
    virtual boolean_type is_numeric() const { return PyNumber_Check(adapted_.ptr()); }

    optional<value_type> index(value_type const& what) const {
        // Per https://docs.djangoproject.com/en/dev/topics/templates/#variables
        // TODO: Move this to django::engine.
        // TODO: Support arbitrary values as keys for non-django general case.

        PyObject   *const o = adapted_.ptr();
        std::string const k = traits_type::narrow(what.to_string());

        // 1. Dictionary lookup
        if (PyMapping_Check(o)) {
            // TODO: If value is a py::object, use PyMapping_HasKey(o, <value>.ptr())
            if (PyMapping_HasKeyString(o, const_cast<char*>(k.c_str()))) {
                return value_type(py::object(adapted_[py::str(k)]));
            }
        }

        // 2. Attribute lookup
        // TODO: If value is a py::object, use PyObject_HasAttr(o, <value>.ptr()) and attr(...)
        if (PyObject_HasAttrString(o, k.c_str())) {
            py::object obj = adapted_.attr(py::str(k));

            // 3. Method call
            if (PyCallable_Check(obj.ptr())) {
                obj = obj();
            }

            return value_type(obj);
        }

        // 4. List-index lookup
        if (PySequence_Check(o)) {
            Py_ssize_t n = static_cast<Py_ssize_t>(what.to_number());

            if (n < PySequence_Size(o)) {
                return value_type(py::object(adapted_[py::long_(n)]));
            }
        }

        return boost::none;
    }

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
            std::string const& type = traits_type::narrow(class_name(obj));
            throw_exception(std::runtime_error(type + " object is not iterable"));
        }
    }

    template <class I>
    inline static I end(py::object const& obj) {
        return I(stl_iterator_type());
    }

    inline static string_type class_name(py::object const& obj) {
        return d::to_string<string_type>(obj.attr("__class__").attr("__name__"));
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
