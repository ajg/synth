//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
#define AJG_SYNTHESIS_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED


#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include <ajg/synthesis/adapters/adapter.hpp>

namespace ajg {
namespace synthesis {

//
// specialization for boost::python::object
////////////////////////////////////////////////////////////////////////////////

template <class Traits>
struct adapter<Traits, boost::python::object>
    : public abstract_adapter<Traits> {

    typedef boost::python::object object_type;
    AJG_SYNTHESIS_ADAPTER(object_type)
    object_type const adapted_;

  private:

    typedef typename boost::python::stl_input_iterator<object_type> stl_iterator;

  public:

    inline static string_type as_string(boost::python::object const& obj) {
        boost::python::extract<string_type> const s((boost::python::str(obj)));
        if (!s.check()) {
            throw_exception(bad_method("str"));
        }
        return string_type(s);
    }

    boolean_type test() const { return boolean_type(adapted_); }

 // void input (istream_type& in)        { in >> adapted_; }
 // void output(ostream_type& out) const { out << adapted_; }
    void output(ostream_type& out) const { out << as_string(adapted_); }

    iterator begin() { return iterator(stl_iterator(adapted_)); }
    iterator end()   { return iterator(stl_iterator()); }

    const_iterator begin() const { return const_iterator(stl_iterator(adapted_)); }
    const_iterator end()   const { return const_iterator(stl_iterator()); }

    optional<value_type> index(value_type const& key) const {
        namespace py = ::boost::python;
        AJG_DUMP(as_string(adapted_));
        AJG_DUMP(key.to_string());

        // Per https://docs.djangoproject.com/en/dev/topics/templates/#variables
        // TODO: Move this to django::engine.
        // TODO: Support arbitrary values as keys for non-django general case.

        PyObject *const o = adapted_.ptr();
        string_type const k = key.to_string();

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
            Py_ssize_t n = key.count();

            if (n < PySequence_Size(o)) {
                return value_type(py::object(adapted_[py::long_(n)]));
            }
        }

        return none;
    }
};

}} // namespace ajg::synthesis

#endif // AJG_SYNTHESIS_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
