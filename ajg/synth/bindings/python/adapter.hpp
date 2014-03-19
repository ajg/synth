//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED


#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for boost::python::object
////////////////////////////////////////////////////////////////////////////////

template <class Traits>
struct adapter<Traits, boost::python::object>
    : public abstract_adapter<Traits> {

    typedef boost::python::object object_type;
    AJG_SYNTH_ADAPTER(object_type)
    object_type adapted_;

  private:

    typedef typename boost::python::stl_input_iterator<object_type> stl_iterator_type;

  public:

    inline static string_type as_string(boost::python::object const& obj) {
        return boost::python::extract<string_type>(boost::python::str(obj));
    }

    template <class T>
    inline static T as_numeric(boost::python::object const& obj) {
        return boost::python::extract<T>(boost::python::long_(obj));
    }

    boolean_type test() const { return boolean_type(adapted_); }
    datetime_type to_datetime() const {
        using namespace boost::gregorian;
        using namespace boost::posix_time;
        return datetime_type
            ( date( as_numeric<unsigned short>(adapted_.attr("year"))
                  , as_numeric<unsigned short>(adapted_.attr("month"))
                  , as_numeric<unsigned short>(adapted_.attr("day"))
                  )
            , time_duration( as_numeric<long>(adapted_.attr("hour"))
                           , as_numeric<long>(adapted_.attr("minute"))
                           , as_numeric<long>(adapted_.attr("second"))
                           , as_numeric<long>(adapted_.attr("microsecond")) * 1000
                        // , TODO: adapted_.attr("tzinfo")
                           )
            );
    }

 // void input (istream_type& in)        { in >> adapted_; }
 // void output(ostream_type& out) const { out << adapted_; }
    void output(ostream_type& out) const { out << as_string(adapted_); }

    iterator begin() { return iterator(stl_iterator_type(adapted_)); }
    iterator end()   { return iterator(stl_iterator_type()); }

    const_iterator begin() const { return const_iterator(stl_iterator_type(adapted_)); }
    const_iterator end()   const { return const_iterator(stl_iterator_type()); }

    optional<value_type> index(value_type const& what) const {
        namespace py = ::boost::python;
        // AJG_DUMP(as_string(adapted_));
        // AJG_DUMP(what.to_string());

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
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_BINDINGS_PYTHON_ADAPTER_HPP_INCLUDED
