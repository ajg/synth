//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED

#include <utility>

#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include <ajg/synth/detail/text.hpp>

namespace ajg {
namespace synth {
namespace bindings {
namespace python {

namespace py = boost::python;

template <class Value>
struct conversions {
  public:

    typedef Value                                                               value_type;

    typedef typename value_type::arguments_type                                 arguments_type;
    typedef typename value_type::traits_type                                    traits_type;

    typedef typename traits_type::none_type                                     none_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::integer_type                                  integer_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::symbols_type                                  symbols_type;
    typedef typename traits_type::date_type                                     date_type;
    typedef typename traits_type::time_type                                     time_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::timezone_type                                 timezone_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

  private:

    typedef detail::text<string_type>                                           text;

  public:

    inline static boost::optional<boolean_type> make_boolean(py::object const& obj) {
        py::extract<boolean_type> e(obj);
        return e.check() ? boost::optional<boolean_type>(e()) : boost::none;
    }

    inline static boost::optional<size_type> make_size(py::object const& obj) {
        py::extract<size_type> e((py::long_(obj)));
        return e.check() ? boost::optional<size_type>(e()) : boost::none;
    }

    inline static boost::optional<number_type> make_number(py::object const& obj) {
        py::extract<number_type> e(obj);
        return e.check() ? boost::optional<number_type>(e()) : boost::none;
    }

    inline static string_type make_string(py::object const& obj) {
        return py::extract<string_type>(py::str(obj));
    }

    inline static date_type make_date(py::object const& dt) {
        return traits_type::to_date
            ( make_size(dt.attr("year")).get_value_or(0)
            , make_size(dt.attr("month")).get_value_or(0)
            , make_size(dt.attr("day")).get_value_or(0)
            );
    }

    /*
    inline time_type make_time(py::object const& dt) {
        return traits_type::to_time
            ( make_size(dt.attr("hour")).get_value_or(0)
            , make_size(dt.attr("minute")).get_value_or(0)
            , make_size(dt.attr("second")).get_value_or(0)
            , make_size(dt.attr("microsecond")).get_value_or(0) * 1000
            );
    }
    */

    inline static duration_type make_time_as_duration(py::object const& dt) {
        return traits_type::to_duration
            ( make_size(dt.attr("hour")).get_value_or(0)
            , make_size(dt.attr("minute")).get_value_or(0)
            , make_size(dt.attr("second")).get_value_or(0)
            , make_size(dt.attr("microsecond")).get_value_or(0) * 1000
            );
    }

    inline static duration_type make_duration(py::object const& timedelta) {
        return traits_type::to_duration
            ( make_size(timedelta.attr("days")).get_value_or(0) * 24 * 60 * 60
            + make_size(timedelta.attr("seconds")).get_value_or(0)
            , make_size(timedelta.attr("microseconds")).get_value_or(0) * 1000
            );
    }

    inline static timezone_type make_timezone(py::object const& dt) {
        if (py::object const& tzinfo = dt.attr("tzinfo")) {
            (void) tzinfo;

            string_type   name;
            duration_type offset;
            string_type   dst_name;
            duration_type dst_offset;

            if (py::object const& utcoffset = dt.attr("utcoffset")()) {
                offset = make_duration(utcoffset);
            }

            if (py::object const& dst = dt.attr("dst")()) {
                dst_offset = make_duration(dst);
            }

            if (py::object const& tzname = dt.attr("tzname")()) {
                if (!traits_type::is_empty(dst_offset)) {
                    // TODO: name = tzinfo.tzname(dt without dst) or attempt to map common ones manually (e.g. EDT -> EST).
                    dst_name = make_string(tzname);
                }
                else {
                    name = make_string(tzname);
                    // TODO: dst_name = tzinfo.tzname(dt with dst) or attempt to map common ones manually (e.g. EST -> EDT).
                }
            }

            return traits_type::to_timezone(name, offset, dst_name, dst_offset);
        }
        else {
            return traits_type::empty_timezone();
        }
    }

    inline static datetime_type make_datetime(py::object const& dt) {
        return traits_type::to_datetime
            ( make_date(dt)
            , make_time_as_duration(dt)
            , make_timezone(dt)
            );
    }

    inline static py::object make_object(value_type const& value) {
        BOOST_ASSERT(value.initialized());

        // DSHOW(value.type_name());
        // DSHOW(value.is_unit());
        // DSHOW(value.is_boolean());
        // DSHOW(value.is_numeric());
        // DSHOW(value.is_chronologic());
        // DSHOW(value.is_textual());
        // DSHOW(value.is_sequential());
        // DSHOW(value.is_associative());
        // DSHOW(value);

             if (value.template is<py::object>()) return value.template as<py::object>();
        // else if (value.template is<py::tuple>())  return value.template as<py::tuple>();
        // else if (value.template is<py::dict>())   return value.template as<py::dict>();
        // else if (value.template is<py::list>())   return value.template as<py::list>();
        // else if (value.template is<py::str>())    return value.template as<py::str>();
        // TODO: py::handle<>(value.template as<PyObject*>());
        else if (value.template is<PyObject*>())  AJG_SYNTH_THROW(not_implemented("make_object::PyObject*"));
        else {
                 if (value.is_unit())        return py::object(); // == None
                 if (value.is_boolean())     return py::object(value.to_boolean());
            else if (value.is_chronologic()) AJG_SYNTH_THROW(not_implemented("make_object::chronologic"));
            else if (value.is_textual())     return py::object(value.to_string());
            else if (value.is_numeric())     return py::object(value.to_number()); // Must come after textual due to C++ chars being both numeric and textual.
            else if (value.is_sequential()) {
                py::list list;
                BOOST_FOREACH(value_type const& element, value) {
                    list.append(make_object(element));
                }
                return list;
            }
            else if (value.is_associative()) {
                py::dict dict;
                BOOST_FOREACH(value_type const& key, value.attributes()) {
                    if (typename value_type::attribute_type const attribute = value.attribute(key)) {
                        dict[make_object(key)] = make_object(*attribute);
                    }
                }
                return dict;
            }
            else {
                AJG_SYNTH_THROW(not_implemented("make_object<" + text::narrow(value.type_name()) + ">"));
            }
        }
    }

    inline static std::pair<py::tuple, py::dict> make_args(arguments_type const& arguments, py::list& list, py::dict& dict) {
        BOOST_FOREACH(typename arguments_type::first_type::value_type const& value, arguments.first) {
            list.append(make_object(value));
        }

        BOOST_FOREACH(typename arguments_type::second_type::value_type const& pair, arguments.second) {
            dict[pair.first] = make_object(pair.second);
        }

        return std::pair<py::tuple, py::dict>((py::tuple(list)), dict);
    }

    inline static std::pair<py::tuple, py::dict> make_args(arguments_type const& arguments) {
        py::list list;
        py::dict dict;
        return make_args(arguments, list, dict);
    }

    inline static std::pair<py::tuple, py::dict> make_args_with(value_type const& v0, arguments_type arguments) {
        arguments.first.insert(arguments.first.begin(), 1, v0);
        return make_args(arguments);
    }

    /*
    inline static std::pair<py::tuple, py::dict> make_args_with(value_type const& v0, value_type const& v1, arguments_type arguments) {
        arguments.first.insert(arguments.first.begin(), 1, v1);
        arguments.first.insert(arguments.first.begin(), 1, v0);
        return make_args(arguments);
    }

    inline static std::pair<py::tuple, py::dict> make_args_with_object(py::object const& obj, arguments_type arguments) {
        py::list list;
        py::dict dict;
        list.append(obj);
        return make_args(arguments, list, dict);
    }
    */

    inline static std::pair<py::tuple, py::dict> make_args_with_objects(py::object const& o1, py::object const& o2, arguments_type const& arguments) {
        py::list list;
        py::dict dict;
        list.append(o1);
        list.append(o2);
        return make_args(arguments, list, dict);
    }
};

}}}} // namespace ajg::synth::bindings::python

#endif // AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED
