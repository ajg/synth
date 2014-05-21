//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED

#include <utility>

#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

namespace ajg {
namespace synth {
namespace bindings {
namespace python {

using boost::optional;
namespace py = boost::python;

// TODO: Refactor these into a utility struct predicated on Traits.

template <class Traits>
inline typename Traits::size_type get_size(py::object const& obj) {
    return py::extract<typename Traits::size_type>(py::long_(obj));
}

template <class Traits>
inline typename Traits::string_type get_string(py::object const& obj) {
    return py::extract<typename Traits::string_type>(py::str(obj));
}

template <class Traits>
inline typename Traits::date_type get_date(py::object const& dt) {
    return Traits::to_date
        ( get_size<Traits>(dt.attr("year"))
        , get_size<Traits>(dt.attr("month"))
        , get_size<Traits>(dt.attr("day"))
        );
}

/*
template <class Traits>
inline typename Traits::time_type get_time(py::object const& dt) {
    return Traits::to_time
        ( get_size<Traits>(dt.attr("hour"))
        , get_size<Traits>(dt.attr("minute"))
        , get_size<Traits>(dt.attr("second"))
        , get_size<Traits>(dt.attr("microsecond")) * 1000
        );
}
*/

template <class Traits>
inline typename Traits::duration_type get_time_as_duration(py::object const& dt) {
    return Traits::to_duration
        ( get_size<Traits>(dt.attr("hour"))
        , get_size<Traits>(dt.attr("minute"))
        , get_size<Traits>(dt.attr("second"))
        , get_size<Traits>(dt.attr("microsecond")) * 1000
        );
}

template <class Traits>
inline typename Traits::duration_type get_duration(py::object const& timedelta) {
    return Traits::to_duration
          ( get_size<Traits>(timedelta.attr("days")) * 24 * 60 * 60
          + get_size<Traits>(timedelta.attr("seconds"))
          , get_size<Traits>(timedelta.attr("microseconds")) * 1000
          );
}

template <class Traits>
inline typename Traits::timezone_type get_timezone(py::object const& dt) {
    if (py::object const& tzinfo = dt.attr("tzinfo")) {
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
                dst_name = get_string<Traits>(tzname);
            }
            else {
                name = get_string<Traits>(tzname);
                // TODO: dst_name = tzinfo.tzname(dt with dst) or attempt to map common ones manually (e.g. EST -> EDT).
            }
        }

        return Traits::to_timezone(name, offset, dst_name, dst_offset);
    }
    else {
        return Traits::empty_timezone();
    }
}

template <class Traits>
inline typename Traits::datetime_type get_datetime(py::object const& dt) {
    return Traits::to_datetime( get_date<Traits>(dt)
                              , get_time_as_duration<Traits>(dt)
                              , get_timezone<Traits>(dt)
                              );
}

template <class Value>
inline py::object from_value(Value const& value) {
    return value.template to_<py::object>();
}

template <class Arguments>
inline std::pair<py::tuple, py::dict> from_arguments(Arguments const& arguments, py::list& list, py::dict& dict) {
    BOOST_FOREACH(typename Arguments::first_type::value_type const& value, arguments.first) {
        list.append(from_value(value));
    }

    BOOST_FOREACH(typename Arguments::second_type::value_type const& pair, arguments.second) {
        dict[pair.first] = from_value(pair.second);
    }

    return std::pair<py::tuple, py::dict>((py::tuple(list)), dict);
}

template <class Arguments>
inline std::pair<py::tuple, py::dict> from_arguments(Arguments const& arguments) {
    py::list list;
    py::dict dict;
    return from_arguments(arguments, list, dict);
}

template <class Value, class Arguments>
inline std::pair<py::tuple, py::dict> from_arguments_with(Value const& p0, Arguments arguments) {
    arguments.first.insert(arguments.first.begin(), 1, p0);
    return from_arguments(arguments);
}


template <class Arguments>
inline std::pair<py::tuple, py::dict> from_arguments_with_object(py::object const& obj, Arguments arguments) {
    py::list list;
    py::dict dict;
    list.append(obj);
    return from_arguments(arguments, list, dict);
}


}}}} // namespace ajg::synth::bindings::python

#endif // AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED
