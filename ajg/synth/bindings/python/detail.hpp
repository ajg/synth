//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED

#include <utility>

#include <boost/python.hpp>
#include <boost/foreach.hpp>

// TODO: Refactor this into a utility struct or fold into the binding or such.

namespace ajg {
namespace synth {
namespace bindings {
namespace python {
namespace detail {

namespace py = boost::python;

template <class Integral>
inline Integral get_integral(py::object const& obj) {
    return py::extract<Integral>(py::long_(obj));
}

template <class String>
inline String get_string(py::object const& obj) {
    return py::extract<String>(py::str(obj));
}

template <class Traits>
inline typename Traits::date_type get_date(py::object const& dt) {
    return Traits::to_date
        ( get_integral<typename Traits::size_type>(dt.attr("year"))
        , get_integral<typename Traits::size_type>(dt.attr("month"))
        , get_integral<typename Traits::size_type>(dt.attr("day"))
        );
}

template <class Traits>
inline typename Traits::time_type get_time(py::object const& dt) {
    return Traits::to_time
        ( get_integral<typename Traits::size_type>(dt.attr("hour"))
        , get_integral<typename Traits::size_type>(dt.attr("minute"))
        , get_integral<typename Traits::size_type>(dt.attr("second"))
        , get_integral<typename Traits::size_type>(dt.attr("microsecond")) * 1000
        );
}

template <class Traits>
inline typename Traits::duration_type get_duration(py::object const& timedelta) {
    return Traits::to_duration
          ( get_integral<typename Traits::size_type>(timedelta.attr("days")) * 24 * 60 * 60
          + get_integral<typename Traits::size_type>(timedelta.attr("seconds"))
          , get_integral<typename Traits::size_type>(timedelta.attr("microseconds")) * 1000
          );
}

template <class Traits>
inline typename Traits::timezone_type get_timezone(py::object const& dt) {
    if (py::object const& tzinfo = dt.attr("tzinfo")()) {
        (void) tzinfo;

        typename Traits::string_type   name;
        typename Traits::duration_type offset;
        typename Traits::string_type   dst_name;
        typename Traits::duration_type dst_offset;

        if (py::object const& utcoffset = dt.attr("utcoffset")()) {
            offset = get_duration<Traits>(utcoffset);
        }

        if (py::object const& dst = dt.attr("dst")()) {
            dst_offset = get_duration<Traits>(dst);
        }

        if (py::object const& tzname = dt.attr("tzname")()) {
            if (!Traits::is_empty(dst_offset)) {
                // TODO: name = tzinfo.tzname(dt without dst) or attempt to map common ones manually (e.g. EDT -> EST).
                dst_name = get_string<typename Traits::string_type>(tzname);
            }
            else {
                name = get_string<typename Traits::string_type>(tzname);
                // TODO: dst_name = tzinfo.tzname(dt with dst) or attempt to map common ones manually (e.g. EST -> EDT).
            }
        }

        return Traits::to_timezone(name, offset, dst_name, dst_offset);
    }
    else {
        return typename Traits::timezone_type();
    }
}

template <class Traits>
inline typename Traits::datetime_type get_datetime(py::object const& dt) {
    return Traits::to_datetime( get_date<Traits>(dt)
                              , get_time<Traits>(dt)
                              , get_timezone<Traits>(dt)
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

}}}}} // namespace ajg::synth::bindings::python::detail

#endif // AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED
