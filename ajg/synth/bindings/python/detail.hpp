//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

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

template <class Integral>
inline Integral to_integral(py::object const& obj) {
    return py::extract<Integral>(py::long_(obj));
}

template <class String>
inline String to_string(py::object const& obj) {
    return py::extract<String>(py::str(obj));
}

template <class Traits>
inline typename Traits::datetime_type to_datetime(py::object const& obj) {
    return typename Traits::datetime_type
        ( typename Traits::date_type
              ( to_integral<unsigned short>(obj.attr("year"))
              , to_integral<unsigned short>(obj.attr("month"))
              , to_integral<unsigned short>(obj.attr("day"))
              )
        , typename Traits::duration_type
              ( to_integral<long>(obj.attr("hour"))
              , to_integral<long>(obj.attr("minute"))
              , to_integral<long>(obj.attr("second"))
              , to_integral<long>(obj.attr("microsecond")) * 1000
           // , TODO: obj.attr("tzinfo")
              )
        );
}

template <class Value>
inline py::object from_value(Value const& value) {
    return value.template to_<py::object>();
}

template <class Arguments>
inline std::pair<py::tuple, py::dict> from_arguments(Arguments const& arguments) {
    py::list list;

    BOOST_FOREACH(typename Arguments::first_type::value_type const& value, arguments.first) {
        list.append(from_value(value));
    }

    std::pair<py::tuple, py::dict> args((py::tuple(list)), py::dict());

    BOOST_FOREACH(typename Arguments::second_type::value_type const& pair, arguments.second) {
        args.second[pair.first] = from_value(pair.second);
    }

    return args;
}

template <class Value, class Arguments>
inline std::pair<py::tuple, py::dict> from_arguments(Value const& p0, Arguments arguments) {
    arguments.first.insert(arguments.first.begin(), 1, p0);
    return from_arguments(arguments);
}

}}}} // namespace ajg::synth::python::detail

#endif // AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED
