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

namespace py = boost::python;

// TODO: Refactor these into a utility struct predicated on Traits.

template <class Traits>
inline boost::optional<typename Traits::boolean_type> get_boolean(py::object const& obj) {
    py::extract<typename Traits::boolean_type> e(obj);
    return e.check() ? boost::optional<typename Traits::boolean_type>(e()) : boost::none;
}

template <class Traits>
inline boost::optional<typename Traits::size_type> get_size(py::object const& obj) {
    py::extract<typename Traits::size_type> e((py::long_(obj)));
    return e.check() ? boost::optional<typename Traits::size_type>(e()) : boost::none;
}

template <class Traits>
inline boost::optional<typename Traits::number_type> get_number(py::object const& obj) {
    py::extract<typename Traits::number_type> e(obj);
    return e.check() ? boost::optional<typename Traits::number_type>(e()) : boost::none;
}

template <class Traits>
inline typename Traits::string_type get_string(py::object const& obj) {
    return py::extract<typename Traits::string_type>(py::str(obj));
}

template <class Traits>
inline typename Traits::date_type get_date(py::object const& dt) {
    return Traits::to_date
        ( get_size<Traits>(dt.attr("year")).get_value_or(0)
        , get_size<Traits>(dt.attr("month")).get_value_or(0)
        , get_size<Traits>(dt.attr("day")).get_value_or(0)
        );
}

/*
template <class Traits>
inline typename Traits::time_type get_time(py::object const& dt) {
    return Traits::to_time
        ( get_size<Traits>(dt.attr("hour")).get_value_or(0)
        , get_size<Traits>(dt.attr("minute")).get_value_or(0)
        , get_size<Traits>(dt.attr("second")).get_value_or(0)
        , get_size<Traits>(dt.attr("microsecond")).get_value_or(0) * 1000
        );
}
*/

template <class Traits>
inline typename Traits::duration_type get_time_as_duration(py::object const& dt) {
    return Traits::to_duration
        ( get_size<Traits>(dt.attr("hour")).get_value_or(0)
        , get_size<Traits>(dt.attr("minute")).get_value_or(0)
        , get_size<Traits>(dt.attr("second")).get_value_or(0)
        , get_size<Traits>(dt.attr("microsecond")).get_value_or(0) * 1000
        );
}

template <class Traits>
inline typename Traits::duration_type get_duration(py::object const& timedelta) {
    return Traits::to_duration
          ( get_size<Traits>(timedelta.attr("days")).get_value_or(0) * 24 * 60 * 60
          + get_size<Traits>(timedelta.attr("seconds")).get_value_or(0)
          , get_size<Traits>(timedelta.attr("microseconds")).get_value_or(0) * 1000
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
    typedef Value value_type;
    BOOST_ASSERT(value.initialized());

    // SHOW(debug::unmangle(typeid(value_type).name()));
    // SHOW(debug::unmangle(value.type().name()));
    // SHOW(value);

         if (value.template is<py::object>()) return value.template as<py::object>();
         /*
    else if (value.template is<py::tuple>())  return value.template as<py::tuple>();
    else if (value.template is<py::dict>())   return value.template as<py::dict>();
    else if (value.template is<py::list>())   return value.template as<py::list>();
    else if (value.template is<py::str>())    return value.template as<py::str>();
    */
    // TODO: py::handle<>(value.template as<PyObject*>());
    else if (value.template is<PyObject*>())  AJG_SYNTH_THROW(not_implemented("from_value::PyObject*"));
    else {
        /*
        SHOW(debug::unmangle(value.type().name()));
        SHOW(value.is_unit());
        SHOW(value.is_boolean());
        SHOW(value.is_numeric());
        SHOW(value.is_chronologic());
        SHOW(value.is_textual());
        SHOW(value.is_sequential());
        SHOW(value.is_associative());
        SHOW(value);
        */

             if (value.is_unit())        return py::object(); // == None
             if (value.is_boolean())     return py::object(value.to_boolean());
        else if (value.is_chronologic()) AJG_SYNTH_THROW(not_implemented("from_value::chronologic"));
        else if (value.is_textual())     return py::object(value.to_string());
        else if (value.is_numeric())     return py::object(value.to_number()); // Must come after textual due to C++ chars being both numeric and textual.
        else if (value.is_sequential()) {
            py::list list;
            BOOST_FOREACH(value_type const& element, value) {
                list.append(from_value(element));
            }
            return list;
        }
        else if (value.is_associative()) {
            py::dict dict;
            BOOST_FOREACH(value_type const& key, value.attributes()) {
                if (typename value_type::attribute_type const attribute = value.attribute(key)) {
                    dict[from_value(key)] = from_value(*attribute);
                }
            }
            return dict;
        }
        else {
            AJG_SYNTH_THROW(not_implemented("from_value::" + debug::unmangle(value.type().name())));
        }

        /*
        try {

            return value.template to_<py::object>();

        }
        catch (not_implemented const&) {
            return

        }*/
    }
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

template <class V0, class Arguments>
inline std::pair<py::tuple, py::dict> from_arguments_with(V0 const& v0, Arguments arguments) {
    arguments.first.insert(arguments.first.begin(), 1, v0);
    return from_arguments(arguments);
}

/*
template <class V0, class V1, class Arguments>
inline std::pair<py::tuple, py::dict> from_arguments_with(V0 const& v0, V1 const& v1, Arguments arguments) {
    arguments.first.insert(arguments.first.begin(), 1, v1);
    arguments.first.insert(arguments.first.begin(), 1, v0);
    return from_arguments(arguments);
}

template <class Arguments>
inline std::pair<py::tuple, py::dict> from_arguments_with_object(py::object const& obj, Arguments arguments) {
    py::list list;
    py::dict dict;
    list.append(obj);
    return from_arguments(arguments, list, dict);
}
*/

template <class Arguments>
inline std::pair<py::tuple, py::dict> from_arguments_with_objects(py::object const& o1, py::object const& o2, Arguments arguments) {
    py::list list;
    py::dict dict;
    list.append(o1);
    list.append(o2);
    return from_arguments(arguments, list, dict);
}

}}}} // namespace ajg::synth::bindings::python

#endif // AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED
