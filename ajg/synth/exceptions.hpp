//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_EXCEPTIONS_HPP_INCLUDED
#define AJG_SYNTH_EXCEPTIONS_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <string>
#include <typeinfo>
#include <stdexcept>
#include <exception>

namespace ajg {
namespace synth {

struct exception {
    exception() {}
    ~exception() throw () {}
};

//
// not_implemented
////////////////////////////////////////////////////////////////////////////////////////////////////

struct not_implemented : public exception, public std::runtime_error {
    not_implemented(std::string const& feature) : std::runtime_error("not implemented: " + feature) {}
    ~not_implemented() throw () {}
};

//
// path_error
////////////////////////////////////////////////////////////////////////////////////////////////////

struct path_error : public exception, public std::runtime_error {
    std::string const path;

    path_error(std::string const& path, std::string const& message) : std::runtime_error(message), path(path) {}
    ~path_error() throw () {}
};

//
// read_error
////////////////////////////////////////////////////////////////////////////////////////////////////

struct read_error : public path_error {
    read_error(std::string const& path, std::string const& reason) : path_error(path, "reading `" + path + "` failed (" + reason + ")") {}
    ~read_error() throw () {}
};

//
// write_error
////////////////////////////////////////////////////////////////////////////////////////////////////

struct write_error : public path_error {
    write_error(std::string const& path, std::string const& reason) : path_error(path, "writing `" + path + "` failed (" + reason + ")") {}
    ~write_error() throw () {}
};

//
// conversion_error
////////////////////////////////////////////////////////////////////////////////////////////////////

struct conversion_error : public exception, public std::runtime_error {
    std::type_info const& from;
    std::type_info const& to;

    conversion_error(std::type_info const& from, std::type_info const& to)
        : std::runtime_error("could not convert value from `" +
              debug::unmangle(from.name()) + "` to `" +
              debug::unmangle(to.name()) + "`")
        , from(from)
        , to(to) {}
    ~conversion_error() throw () {}
};

//
// parsing_error
////////////////////////////////////////////////////////////////////////////////////////////////////

struct parsing_error : public exception, public std::runtime_error {
    parsing_error(std::string const& line) : std::runtime_error("parsing error near `" + line + "`") {}
    ~parsing_error() throw () {}
};

//
// missing_variable
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_variable : public exception, public std::invalid_argument {
    std::string const name;

    missing_variable(std::string const& name) : std::invalid_argument("missing variable `" + name + "`"), name(name) {}
    ~missing_variable() throw () {}
};

//
// missing_attribute
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_attribute : public exception, public std::invalid_argument {
    std::string const name;

    missing_attribute(std::string const& name) : std::invalid_argument("missing attribute `" + name + "`"), name(name) {}
    ~missing_attribute() throw () {}
};

//
// missing_library
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_library : public exception, public std::invalid_argument {
    std::string const name;

    missing_library(std::string const& name) : std::invalid_argument("missing library `" + name + "`"), name(name) {}
    ~missing_library() throw () {}
};

//
// missing_key
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_key : public exception, public std::invalid_argument {
    std::string const name;

    missing_key(std::string const& name) : std::invalid_argument("missing key `" + name + "`"), name(name) {}
    ~missing_key() throw () {}
};

//
// missing_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_tag : public exception, public std::invalid_argument {
    std::string const name;

    missing_tag(std::string const& name) : std::invalid_argument("missing tag `" + name + "`"), name(name) {}
    ~missing_tag() throw () {}
};

//
// missing_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_filter : public exception, public std::invalid_argument {
    std::string const name;

    missing_filter(std::string const& name) : std::invalid_argument("missing filter `" + name + "`"), name(name) {}
    ~missing_filter() throw () {}
};

//
// missing_option
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_option : public exception, public std::invalid_argument {
    std::string const name;

    missing_option(std::string const& name) : std::invalid_argument("missing option `" + name + "`"), name(name) {}
    ~missing_option() throw () {}
};

//
// unknown_option
////////////////////////////////////////////////////////////////////////////////////////////////////

struct unknown_option : public exception, public std::invalid_argument {
    std::string const name;

    unknown_option() : std::invalid_argument("unknown option"), name() {}
    unknown_option(std::string const& name) : std::invalid_argument("unknown option `" + name + "`"), name(name) {}
    ~unknown_option() throw () {}
};

//
// empty_parameter
////////////////////////////////////////////////////////////////////////////////////////////////////

struct empty_parameter : public exception, public std::invalid_argument {
    std::string const option;

    empty_parameter(std::string const& option) : std::invalid_argument("empty parameter for option `" + option + "`"), option(option) {}
    ~empty_parameter() throw () {}
};

//
// invalid_parameter
////////////////////////////////////////////////////////////////////////////////////////////////////

struct invalid_parameter : public exception, public std::invalid_argument {
    std::string const option;

    invalid_parameter(std::string const& option) : std::invalid_argument("invalid parameter for option `" + option + "`"), option(option) {}
    ~invalid_parameter() throw () {}
};

//
// missing_parameter
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_parameter : public exception, public std::invalid_argument {
    std::string const option;

    missing_parameter(std::string const& option) : std::invalid_argument("missing parameter for option `" + option + "`"), option(option) {}
    ~missing_parameter() throw () {}
};

//
// superfluous_parameter
////////////////////////////////////////////////////////////////////////////////////////////////////

struct superfluous_parameter : public exception, public std::invalid_argument {
    std::string const option;

    superfluous_parameter(std::string const& option) : std::invalid_argument("superfluous parameter for option `" + option + "`"), option(option) {}
    ~superfluous_parameter() throw () {}
};

//
// missing_argument
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_argument : public exception, public std::invalid_argument {
    std::string const name;

    missing_argument() : std::invalid_argument("missing argument"), name() {}
    missing_argument(std::string const& name) : std::invalid_argument("missing argument `" + name + "`"), name(name) {}
    ~missing_argument() throw () {}
};

//
// superfluous_argument
////////////////////////////////////////////////////////////////////////////////////////////////////

struct superfluous_argument : public exception, public std::invalid_argument {
    std::string const name;

    superfluous_argument() : std::invalid_argument("superfluous argument") {}
    superfluous_argument(std::string const& name) : std::invalid_argument("superfluous argument `" + name + "`"), name(name) {}
    ~superfluous_argument() throw () {}
};

//
// unknown_argument
////////////////////////////////////////////////////////////////////////////////////////////////////

struct unknown_argument : public exception, public std::invalid_argument {
    std::string const value;

    unknown_argument(std::string const& value) : std::invalid_argument("unknown argument `" + value + "`"), value(value) {}
    ~unknown_argument() throw () {}
};

/*
//
// invalid_argument
//     TODO: Replace all uses of std::invalid_argument with this?
////////////////////////////////////////////////////////////////////////////////////////////////////

struct invalid_argument : public exception, public std::invalid_argument {
    std::string const name;

    invalid_argument(std::string const& name) : std::invalid_argument("invalid argument `" + name + "`"), name(name) {}
    ~invalid_argument() throw () {}
};
*/

//
// invalid_attribute
////////////////////////////////////////////////////////////////////////////////////////////////////

struct invalid_attribute : public exception, public std::invalid_argument {
    std::string const name;

    invalid_attribute(std::string const& name) : std::invalid_argument("invalid attribute `" + name + "`"), name(name) {}
    ~invalid_attribute() throw () {}
};

//
// duplicate_attribute
////////////////////////////////////////////////////////////////////////////////////////////////////

struct duplicate_attribute : public exception, public std::invalid_argument {
    std::string const name;

    duplicate_attribute(std::string const& name) : std::invalid_argument("duplicate attribute `" + name + "`"), name(name) {}
    ~duplicate_attribute() throw () {}
};

//
// invalid_method
////////////////////////////////////////////////////////////////////////////////////////////////////

struct invalid_method : public exception, public std::invalid_argument {
    std::string const name;

    invalid_method(std::string const& name) : std::invalid_argument("invalid method `" + name + "`"), name(name) {}
    ~invalid_method() throw () {}
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_EXCEPTIONS_HPP_INCLUDED
