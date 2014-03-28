//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

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
namespace d = synth::python::detail;

} // namespace

//
// specialization for boost::python::object
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits>
struct adapter<Traits, py::object>
    : public abstract_adapter<Traits> {

    typedef py::object object_type;
    AJG_SYNTH_ADAPTER(object_type)
    object_type adapted_;

  private:

    typedef typename py::stl_input_iterator<object_type> stl_iterator_type;

  public:

    boolean_type test() const { return boolean_type(adapted_); }
    datetime_type to_datetime() const { return d::to_datetime<traits_type>(adapted_); }

 // void input (istream_type& in)        { in >> adapted_; }
 // void output(ostream_type& out) const { out << adapted_; }
    void output(ostream_type& out) const { out << d::to_string<string_type>(adapted_); }

    iterator begin() { return begin<iterator>(adapted_); }
    iterator end()   { return iterator(stl_iterator_type()); }

    const_iterator begin() const { return begin<const_iterator>(adapted_); }
    const_iterator end()   const { return const_iterator(stl_iterator_type()); }

    optional<value_type> index(value_type const& what) const {
        // Per https://docs.djangoproject.com/en/dev/topics/templates/#variables
        // TODO: Move this to django::engine.
        // TODO: Support arbitrary values as keys for non-django general case.

        PyObject *const o = adapted_.ptr();
        string_type const k = what.to_string();

        // 1. Dictionary lookup
        if (PyMapping_Check(o)) {
            if (PyMapping_HasKeyString(o, const_cast<char*>(k.c_str()))) {
                return value_type(py::object(adapted_[py::str(k)]));
            }
        }

        // 2. Attribute lookup
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
            Py_ssize_t n = what.count();

            if (n < PySequence_Size(o)) {
                return value_type(py::object(adapted_[py::long_(n)]));
            }
        }

        return none;
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
            string_type const& type = d::class_name<std::string>(obj);
            throw_exception(std::runtime_error(type + " object is not iterable"));
        }
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
