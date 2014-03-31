//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED

#include <utility>

#include <boost/python.hpp>
#include <boost/foreach.hpp>

namespace ajg {
namespace synth {
namespace python {
namespace detail {

namespace py = boost::python;

template <class Numeric>
inline Numeric to_numeric(py::object const& obj) {
    return py::extract<Numeric>(py::long_(obj));
}

template <class String>
inline String to_string(py::object const& obj) {
    return py::extract<String>(py::str(obj));
}

template <class Traits>
inline typename Traits::datetime_type to_datetime(py::object const& obj) {
    return typename Traits::datetime_type
        ( typename Traits::date_type
              ( to_numeric<unsigned short>(obj.attr("year"))
              , to_numeric<unsigned short>(obj.attr("month"))
              , to_numeric<unsigned short>(obj.attr("day"))
              )
        , typename Traits::duration_type
              ( to_numeric<long>(obj.attr("hour"))
              , to_numeric<long>(obj.attr("minute"))
              , to_numeric<long>(obj.attr("second"))
              , to_numeric<long>(obj.attr("microsecond")) * 1000
           // , TODO: obj.attr("tzinfo")
              )
        );
}

template <class Value>
inline py::object from_value(Value const& value) {
    // TODO: Implement.
    AJG_SYNTH_THROW(not_implemented("from_value"));
}

template <class Arguments>
inline std::pair<py::list, py::dict> from_arguments(Arguments const& arguments) {
    std::pair<py::list, py::dict> args;

    BOOST_FOREACH(typename Arguments::first_type::value_type const& value, arguments.first) {
        args.first.append(from_value(value));
    }

    BOOST_FOREACH(typename Arguments::second_type::value_type const& pair, arguments.second) {
        args.second[pair.first] = from_value(pair.second);
    }

    return args;
}

template <class String>
inline static String class_name(py::object const& obj) {
    return to_string<String>(obj.attr("__class__").attr("__name__"));
}

}}}} // namespace ajg::synth::python::detail

#endif // AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED
