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
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::paths_type                                    paths_type;
    typedef typename traits_type::names_type                                    names_type;
    typedef typename traits_type::symbols_type                                  symbols_type;
    typedef typename traits_type::date_type                                     date_type;
    typedef typename traits_type::time_type                                     time_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::region_type                                   region_type;
    typedef typename traits_type::timezone_type                                 timezone_type;
    typedef typename traits_type::language_type                                 language_type;
    typedef typename traits_type::formats_type                                  formats_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef std::pair<char const*, size_type>                                   buffer_type;

  private:

    typedef detail::text<string_type>                                           text;

  public:

    /*
    inline static boost::optional<boolean_type> make_boolean(py::object const& obj) {
        py::extract<boolean_type> e(obj);
        return e.check() ? boost::optional<boolean_type>(e()) : boost::none;
    }
    */

    inline static boolean_type make_boolean(py::object const& obj) {
        return boolean_type(obj);
    }

    inline static boost::optional<size_type> make_size(py::object const& obj) {
        py::extract<size_type> e((py::long_(obj)));
        return e.check() ? boost::optional<size_type>(e()) : boost::none;
    }

    inline static boost::optional<number_type> make_number(py::object const& obj) {
        // FIXME: Use number_type only for non-integer numbers.
        py::extract<number_type> e(obj);
        return e.check() ? boost::optional<number_type>(e()) : boost::none;
    }

    inline static path_type make_path(py::object const& obj) {
        return make_string(obj);
    }

    inline static string_type make_string(py::object const& obj) {
        return make_string(obj.ptr());
    }

    inline static string_type make_string(PyObject* const o) {
        if (o == 0) {
            AJG_SYNTH_THROW(std::invalid_argument("null object"));
        }

        char*      data;
        Py_ssize_t size;

        if (PyUnicode_Check(o)) {
    #if PY_MAJOR_VERSION >= 3
            if ((data = PyUnicode_AsUTF8AndSize(o, &size)) == 0) {
                AJG_SYNTH_THROW(std::invalid_argument("invalid unicode object"));
            }
            return string_type(data, size);
    #else
            return make_string(PyUnicode_AsUTF8String(o));
            // XXX: Works only when there are solely ASCII characters.
            // return string_type(PyUnicode_AS_DATA(o), PyUnicode_GET_DATA_SIZE(o));
    #endif
        }
    #if PY_MAJOR_VERSION >= 3
        else if (PyBytes_Check(o)) {
            if (PyBytes_AsStringAndSize(o, &data, &size) == -1) {
                AJG_SYNTH_THROW(std::invalid_argument("invalid bytes object"));
            }
            return string_type(data, size);
        }
    #else
        else if (PyString_Check(o)) {
            if (PyString_AsStringAndSize(o, &data, &size) == -1) {
                AJG_SYNTH_THROW(std::invalid_argument("invalid str object"));
            }
            return string_type(data, size);
        }
    #endif
        else {
            return make_string(PyObject_Str(o));
        }

        /*
        else if (PyObject_HasAttrString(o, "__unicode__")) {
            return make_string(obj.attr("__unicode__")());
            // return make_string(PyObject_GetAttrString(o, "__unicode__")(...));
        }
        else if (PyObject_HasAttrString(o, "__str__")) {
            return make_string(obj.attr("__str__")());
            // return make_string(PyObject_GetAttrString(o, "__str__")(...));
        }
        else {
    #if PY_MAJOR_VERSION >= 3
            AJG_SYNTH_THROW(std::invalid_argument("object must be unicode or bytes"));
    #else
            AJG_SYNTH_THROW(std::invalid_argument("object must be unicode or str"));
    #endif
        }
        */
    }

    /*
    inline static string_type make_string(py::object const& obj) {
        PyObject* const o = obj.ptr();
        if (PyString_Check(o) || PyUnicode_Check(o)) {
            buffer_type const buffer = make_buffer(o);
            return string_type(buffer.first, buffer.second);
        }
        else if (PyObject_HasAttrString(o, "__unicode__")) {
            py::object  const& unicode = obj.attr("__unicode__")();
            buffer_type const  buffer  = make_buffer(unicode.ptr());
            return string_type(buffer.first, buffer.second);
        }
        else if (PyObject_HasAttrString(o, "__str__")) {
            py::object  const& str    = obj.attr("__str__")();
            buffer_type const  buffer = make_buffer(str.ptr());
            return string_type(buffer.first, buffer.second);
        }
        return py::extract<string_type>(py::str(obj));
    }
    */

    // TODO: Investigate using something like: (or creating a utf<{8,16,32}>_iterator)
    // inline static std::pair<char_type const*, size_type> make_buffer(PyObject* const o) {
    //     if (PyString_Check(o)) { use char template }
    //     else if (PyUnicode_Check(o)) { use Py_UNICODE template }
    // }

    /*
    inline static buffer_type make_buffer(PyObject* const o) {
        BOOST_ASSERT(PyString_Check(o) || PyUnicode_Check(o));

        char*      data;
        Py_ssize_t size;

        if (PyString_AsStringAndSize(o, &data, &size) == -1) {
            AJG_SYNTH_THROW(std::invalid_argument("buffer source"));
        }

        return buffer_type(data, size);
    }
    */

    /*
    inline static buffer_type make_buffer(PyObject* const o) {
        if (PyString_Check(o)) {
            char*      data;
            Py_ssize_t size;

            if (PyString_AsStringAndSize(o, &data, &size) == -1) {
                AJG_SYNTH_THROW(std::invalid_argument("str/bytes object"));
            }
            return buffer_type(data, size);
        }
        else if (PyUnicode_Check(o)) {
            return buffer_type(PyUnicode_AS_DATA(o), PyUnicode_GET_DATA_SIZE(o));
        }
        else {
            AJG_SYNTH_THROW(std::invalid_argument("object must be unicode or str/bytes"));
        }
    }
    */

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
            , make_size(dt.attr("microsecond")).get_value_or(0)
            );
    }
    */

    inline static duration_type make_time_as_duration(py::object const& dt) {
        return traits_type::to_duration
            ( make_size(dt.attr("hour")).get_value_or(0)
            , make_size(dt.attr("minute")).get_value_or(0)
            , make_size(dt.attr("second")).get_value_or(0)
            , make_size(dt.attr("microsecond")).get_value_or(0)
            );
    }

    inline static duration_type make_duration(py::object const& timedelta) {
        return traits_type::to_duration
            ( make_size(timedelta.attr("days")).get_value_or(0) * 24
            , 0
            , make_size(timedelta.attr("seconds")).get_value_or(0)
            , make_size(timedelta.attr("microseconds")).get_value_or(0)
            );
    }

    inline static timezone_type make_timezone(py::object const tzinfo, py::object dt = make_none()) {
        if (!tzinfo) {
            return traits_type::empty_timezone();
        }

        string_type const s = make_string(tzinfo);

        if (traits_type::is_region(s)) {
            return traits_type::to_region_timezone(s);
        }

        string_type   std_name;
        duration_type std_offset;
        string_type   dst_name;
        duration_type dst_offset;

        if (!dt) {
            dt = make_dt(traits_type::utc_datetime());
        }

        if (py::object const& utcoffset = tzinfo.attr("utcoffset")(dt)) {
            std_offset = make_duration(utcoffset);
        }

        if (py::object const& dst = tzinfo.attr("dst")(dt)) {
            dst_offset = make_duration(dst);
        }

        if (py::object const& tzname = tzinfo.attr("tzname")(dt)) {
            if (!traits_type::is_empty(dst_offset)) {
                // TODO: std_name = tzinfo.tzname(dt without dst) or use tz_database (e.g. EDT -> EST).
                dst_name = make_string(tzname);
            }
            else {
                std_name = make_string(tzname);
                // TODO: dst_name = tzinfo.tzname(dt with dst) or use tz_database (e.g. EST -> EDT).
            }
        }

        return traits_type::to_posix_timezone(std_name, std_offset, dst_name, dst_offset);
    }

    inline static datetime_type make_datetime(py::object const dt) {
        if (!dt) {
            return traits_type::empty_datetime();
        }
        return traits_type::to_datetime
            ( make_date(dt)
            , make_time_as_duration(dt)
            , make_timezone(dt.attr("tzinfo"), dt)
            );
    }

    inline static py::object make_dt(datetime_type const& datetime) {
        date_type     const date = traits_type::to_date(datetime);
        duration_type const time = traits_type::to_duration(traits_type::to_utc_time(datetime));
        // TODO: obj.attr("tzinfo", make_tzinfo(traits_type::to_timezone(datetime)))
        return py::object(py::handle<>(PyDateTime_FromDateAndTime(
            static_cast<int>(date.year()),
            static_cast<int>(date.month()),
            static_cast<int>(date.day()),
            static_cast<int>(time.hours()),
            static_cast<int>(time.minutes()),
            static_cast<int>(time.seconds()),
            static_cast<int>(time.fractional_seconds()))));
    }

    // TODO: Handle non-region typezones as datetime.tzinfo/PyDateTime_TZInfo.
    // For now, we're relying on the fact that pytz accepts region strings.
    inline static py::object make_tzinfo(timezone_type const& timezone) {
        if (traits_type::to_boolean(timezone)) {
            return py::object(traits_type::to_string(timezone));
        }
        return make_none();
    }

    inline static language_type make_language(py::object const& l) {
        if (l) {
            string_type  const code = make_string(l[0]);
            boolean_type const rtl  = make_boolean(l[1]);
            return language_type(std::make_pair(code, rtl));
        }
        return language_type();
    }

    inline static formats_type make_formats(py::dict const& fmts) {
        if (!fmts) {
            return formats_type();
        }
        py::stl_input_iterator<py::tuple> begin(fmts.items()), end;
        formats_type formats;

        BOOST_FOREACH(py::tuple const& item, std::make_pair(begin, end)) {
            string_type const& name   = make_string(item[0]);
            string_type const& format = make_string(item[1]);
            // formats.insert(typename formats_type::value_type(name, format));
            formats[name] = format;
        }
        return formats;

        /*if (fmts) {
            py::list const names = fmts.keys();

            // TODO: Replace with stl_input_iterator version.
            for (std::size_t i = 0, n = py::len(names); i < n; ++i) {
                py::object const name = names[i];
                context.format(make_string(name), make_string(names[name]));
            }
        }
        */
    }

    inline static names_type make_names(py::list const& lst) {
        // TODO: Avoid temporary symbols.
        symbols_type const symbols = make_symbols(lst);
        return names_type(symbols.begin(), symbols.end());
    }

    inline static symbols_type make_symbols(py::object const& obj) {
        // Note: stl_input_iterator<string_type> doesn't work with unicode object or a string_type
        //       other than std::string.
        py::list const& syms = py::list(obj);

        symbols_type symbols;
        if (long const l = py::len(syms)) {
            for (long i = 0; i < l; ++i) {
                symbols.insert(make_string(syms[i]));
            }
        }
        return symbols;
    }

    inline static paths_type make_paths(py::list const& dirs) {
        paths_type paths;
        if (long const l = py::len(dirs)) {
            paths.reserve(l);

            for (long i = 0; i < l; ++i) {
                paths.push_back(make_path(dirs[i]));
            }
        }
        return paths;
    }

    /*inline static paths_type make_paths(py::list const& dirs) {
        py::stl_input_iterator<path_type> begin(dirs), end;
        return paths_type(begin, end);
    }*/

    inline static py::object make_none() {
        return py::object();
    }

    /*
    inline static py::object make_object(py::object const& object) {
        return object;
    }
    */

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

             if (value.template is<py::object>())  return value.template as<py::object>();
        else if (value.template is<py::tuple>())   return value.template as<py::tuple>();
        else if (value.template is<py::dict>())    return value.template as<py::dict>();
        else if (value.template is<py::list>())    return value.template as<py::list>();
        else if (value.template is<py::str>())     return value.template as<py::str>();
        else if (value.template is<PyObject*>())   return py::object(py::handle<>(value.template as<PyObject*>()));
     // else if (value.template is<void*>())       return py::long_(reinterpret_cast<intptr_t>(value.template as<void*>()));
     // else if (value.template is<void const*>()) return py::long_(reinterpret_cast<intptr_t>(value.template as<void const*>()));
        else if (value.template is<void>())        return make_none();
        else if (value.is_unit())                  return make_none();
        else if (value.is_boolean())               return py::object(value.to_boolean());
        else if (value.is_textual())               return py::object(value.to_string());
        else if (value.is_numeric())               return py::object(value.to_number()); // Must come after textual due to C++ chars being both numeric and textual.
        else if (value.is_chronologic())           return make_dt(value.to_datetime());
        else if (value.is_timezone())              return make_tzinfo(value.to_timezone());
        // TODO: Rather than force conversion of complex types, they should be held in bidirectional
        //       wrappers that convert as needed via __bool__, __str__, etc.
        else if (value.is_sequential())            return make_list(value);
        else if (value.is_associative())           return make_dict(value);
        else AJG_SYNTH_THROW(not_implemented("make_object(" + text::narrow(value.type_name()) + ")"));
    }

    inline static py::list make_list(value_type const& value) {
        py::list list;
        BOOST_FOREACH(value_type const& element, value) {
            list.append(make_object(element));
        }
        return list;
    }

    inline static py::dict make_dict(value_type const& value) {
        py::dict dict;
        BOOST_FOREACH(value_type const& key, value.attributes()) {
            if (typename value_type::attribute_type const attribute = value.attribute(key)) {
                dict[make_object(key)] = make_object(*attribute);
            }
        }
        return dict;
    }

    // TODO: Split this into make_args and make_kwargs, and remove make_args_with.
    inline static std::pair<py::tuple, py::dict> make_args(arguments_type const& arguments) {
        py::list list;
        py::dict dict;

        BOOST_FOREACH(typename arguments_type::first_type::value_type const& value, arguments.first) {
            list.append(make_object(value));
        }

        BOOST_FOREACH(typename arguments_type::second_type::value_type const& pair, arguments.second) {
            dict[pair.first] = make_object(pair.second);
        }

        return std::pair<py::tuple, py::dict>(py::tuple(list), dict);
    }

    inline static std::pair<py::tuple, py::dict> make_args_with(value_type const& v0, arguments_type arguments) {
        arguments.first.reserve(arguments.first.size() + 1);
        arguments.first.insert(arguments.first.begin(), 1, v0);
        return make_args(arguments);
    }

    inline static std::pair<py::tuple, py::dict> make_args_with(value_type const& v0, value_type const& v1, arguments_type arguments) {
        arguments.first.reserve(arguments.first.size() + 2);
        arguments.first.insert(arguments.first.begin(), 1, v1);
        arguments.first.insert(arguments.first.begin(), 1, v0);
        return make_args(arguments);
    }

    /*
    inline static std::pair<py::tuple, py::dict> make_args(arguments_type const& arguments, py::list& list, py::dict& dict) {
        BOOST_FOREACH(typename arguments_type::first_type::value_type const& value, arguments.first) {
            list.append(make_object(value));
        }

        BOOST_FOREACH(typename arguments_type::second_type::value_type const& pair, arguments.second) {
            dict[pair.first] = make_object(pair.second);
        }

        return std::pair<py::tuple, py::dict>(py::tuple(list), dict);
    }

    inline static std::pair<py::tuple, py::dict> make_args_with_object(py::object const& obj, arguments_type arguments) {
        py::list list;
        py::dict dict;
        list.append(obj);
        return make_args(arguments, list, dict);
    }

    inline static std::pair<py::tuple, py::dict> make_args_with_objects(py::object const& o1, py::object const& o2, arguments_type const& arguments) {
        py::list list;
        py::dict dict;
        list.append(o1);
        list.append(o2);
        return make_args(arguments, list, dict);
    }
    */
};

}}}} // namespace ajg::synth::bindings::python

#endif // AJG_SYNTH_BINDINGS_PYTHON_DETAIL_HPP_INCLUDED
