//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_EXCEPTIONS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_EXCEPTIONS_HPP_INCLUDED

#include <string>
#include <stdexcept>

namespace ajg {
namespace synth {

//
// parsing_error exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct parsing_error : public std::runtime_error {
    parsing_error(std::string const& line)
        : std::runtime_error("parsing error near `" + line + "'") {}

    ~parsing_error() throw () {}
};

//
// missing_variable exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_variable : public std::invalid_argument {
    std::string const name;

    missing_variable(std::string const& name)
        : std::invalid_argument("missing variable `" + name + "'")
        , name(name) {}

    ~missing_variable() throw () {}
};

//
// missing_attribute exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_attribute : public std::invalid_argument {
    std::string const name;

    missing_attribute(std::string const& name)
        : std::invalid_argument("missing attribute `" + name + "'")
        , name(name) {}

    ~missing_attribute() throw () {}
};

//
// missing_library exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_library : public std::invalid_argument {
    std::string const name;

    missing_library(std::string const& name)
        : std::invalid_argument("missing library `" + name + "'")
        , name(name) {}

    ~missing_library() throw () {}
};

//
// missing_key exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_key : public std::invalid_argument {
    std::string const name;

    missing_key(std::string const& name)
        : std::invalid_argument("missing key `" + name + "'")
        , name(name) {}

    ~missing_key() throw () {}
};

//
// missing_tag exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_tag : public std::invalid_argument {
    std::string const name;

    missing_tag(std::string const& name)
        : std::invalid_argument("missing tag `" + name + "'")
        , name(name) {}

    ~missing_tag() throw () {}
};

//
// missing_filter exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_filter : public std::invalid_argument {
    std::string const name;

    missing_filter(std::string const& name)
        : std::invalid_argument("missing filter `" + name + "'")
        , name(name) {}

    ~missing_filter() throw () {}
};

//
// invalid_attribute exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct invalid_attribute : public std::invalid_argument {
    std::string const name;

    invalid_attribute(std::string const& name)
        : std::invalid_argument("invalid attribute `" + name + "'")
        , name(name) {}

    ~invalid_attribute() throw () {}
};

//
// duplicate_attribute exception
////////////////////////////////////////////////////////////////////////////////////////////////////

struct duplicate_attribute : public std::invalid_argument {
    std::string const name;

    duplicate_attribute(std::string const& name)
        : std::invalid_argument("duplicate attribute `" + name + "'")
        , name(name) {}

    ~duplicate_attribute() throw () {}
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ENGINES_EXCEPTIONS_HPP_INCLUDED
