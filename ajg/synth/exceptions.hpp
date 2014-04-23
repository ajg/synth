//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_EXCEPTIONS_HPP_INCLUDED
#define AJG_SYNTH_EXCEPTIONS_HPP_INCLUDED

#include <ajg/synth/config.hpp>

#include <string>
#include <typeinfo>
#include <stdexcept>
#include <exception>

namespace ajg {
namespace synth {
namespace detail {
std::string get_type_name(std::type_info const&);
}

// TODO: Introduce common synth::exception type (deriving from std::exception).

//
// not_implemented
////////////////////////////////////////////////////////////////////////////////////////////////////

struct not_implemented : public std::runtime_error {
    not_implemented(std::string const& feature) : std::runtime_error("not implemented: " + feature) {}
    ~not_implemented() throw () {}
};

//
// file_error
////////////////////////////////////////////////////////////////////////////////////////////////////

struct file_error : public std::runtime_error {
    std::string const path, action, reason;

    file_error( std::string const& path
              , std::string const& action
              , std::string const& reason
              )
        : std::runtime_error("could not " + action + " file `" + path + "' (" + reason + ")")
        , path(path), action(action), reason(reason) {}

    ~file_error() throw () {}
};

//
// conversion_error
//     TODO: Consider renaming bad_conversion.
////////////////////////////////////////////////////////////////////////////////////////////////////

struct conversion_error : public std::runtime_error {
    std::type_info const& from;
    std::type_info const& to;

    conversion_error(std::type_info const& from, std::type_info const& to)
        : std::runtime_error("could not convert value from `" +
              detail::get_type_name(from) + "' to `" +
              detail::get_type_name(to) + "'")
        , from(from), to(to) {}
    ~conversion_error() throw () {}
};

//
// bad_method
////////////////////////////////////////////////////////////////////////////////////////////////////

struct bad_method : public std::invalid_argument {
    std::string const name;

    bad_method(std::string const& name) : std::invalid_argument("bad method `" + name + "'"), name(name) {}
    ~bad_method() throw () {}
};

//
// parsing_error
////////////////////////////////////////////////////////////////////////////////////////////////////

struct parsing_error : public std::runtime_error {
    parsing_error(std::string const& line) : std::runtime_error("parsing error near `" + line + "'") {}
    ~parsing_error() throw () {}
};

//
// missing_variable
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_variable : public std::invalid_argument {
    std::string const name;

    missing_variable(std::string const& name) : std::invalid_argument("missing variable `" + name + "'"), name(name) {}
    ~missing_variable() throw () {}
};

//
// missing_attribute
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_attribute : public std::invalid_argument {
    std::string const name;

    missing_attribute(std::string const& name) : std::invalid_argument("missing attribute `" + name + "'"), name(name) {}
    ~missing_attribute() throw () {}
};

//
// missing_library
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_library : public std::invalid_argument {
    std::string const name;

    missing_library(std::string const& name) : std::invalid_argument("missing library `" + name + "'"), name(name) {}
    ~missing_library() throw () {}
};

//
// missing_key
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_key : public std::invalid_argument {
    std::string const name;

    missing_key(std::string const& name) : std::invalid_argument("missing key `" + name + "'"), name(name) {}
    ~missing_key() throw () {}
};

//
// missing_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_tag : public std::invalid_argument {
    std::string const name;

    missing_tag(std::string const& name) : std::invalid_argument("missing tag `" + name + "'"), name(name) {}
    ~missing_tag() throw () {}
};

//
// missing_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_filter : public std::invalid_argument {
    std::string const name;

    missing_filter(std::string const& name) : std::invalid_argument("missing filter `" + name + "'"), name(name) {}
    ~missing_filter() throw () {}
};

//
// missing_argument
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_argument : public std::invalid_argument {
    missing_argument() : std::invalid_argument("missing argument") {}
    ~missing_argument() throw () {}
};

//
// superfluous_argument
////////////////////////////////////////////////////////////////////////////////////////////////////

struct superfluous_argument : public std::invalid_argument {
    superfluous_argument() : std::invalid_argument("superfluous argument") {}
    ~superfluous_argument() throw () {}
};

//
// invalid_attribute
////////////////////////////////////////////////////////////////////////////////////////////////////

struct invalid_attribute : public std::invalid_argument {
    std::string const name;

    invalid_attribute(std::string const& name) : std::invalid_argument("invalid attribute `" + name + "'"), name(name) {}
    ~invalid_attribute() throw () {}
};

//
// duplicate_attribute
////////////////////////////////////////////////////////////////////////////////////////////////////

struct duplicate_attribute : public std::invalid_argument {
    std::string const name;

    duplicate_attribute(std::string const& name) : std::invalid_argument("duplicate attribute `" + name + "'"), name(name) {}
    ~duplicate_attribute() throw () {}
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_EXCEPTIONS_HPP_INCLUDED
