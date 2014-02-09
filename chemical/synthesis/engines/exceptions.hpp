
//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ENGINES_EXCEPTIONS_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ENGINES_EXCEPTIONS_HPP_INCLUDED

#include <string>
#include <stdexcept>

namespace chemical {
namespace synthesis {

//
// not_implemented exception
////////////////////////////////////////////////////////////////////////////////

struct not_implemented : public std::runtime_error {
    not_implemented(std::string const& feature)
        : std::runtime_error("not implemented: " + feature) {}

    ~not_implemented() throw () {}
};

//
// parsing_error exception
////////////////////////////////////////////////////////////////////////////////

struct parsing_error : public std::runtime_error {
    parsing_error(std::string const& line)
        : std::runtime_error("parsing error near `" + line + "'") {}

    ~parsing_error() throw () {}
};

//
// missing_variable exception
////////////////////////////////////////////////////////////////////////////////

struct missing_variable : public std::invalid_argument {
    std::string const name;

    missing_variable(std::string const& name)
        : std::invalid_argument("missing context variable `" + name + "'")
        , name(name) {}

    ~missing_variable() throw () {}
};

//
// missing_attribute exception
////////////////////////////////////////////////////////////////////////////////

struct missing_attribute : public std::invalid_argument {
    std::string const name;

    missing_attribute(std::string const& name)
        : std::invalid_argument("missing attribute `" + name + "'")
        , name(name) {}

    ~missing_attribute() throw () {}
};

//
// invalid_attribute exception
////////////////////////////////////////////////////////////////////////////////

struct invalid_attribute : public std::invalid_argument {
    std::string const name;

    invalid_attribute(std::string const& name)
        : std::invalid_argument("invalid attribute `" + name + "'")
        , name(name) {}

    ~invalid_attribute() throw () {}
};

//
// duplicate_attribute exception
////////////////////////////////////////////////////////////////////////////////

struct duplicate_attribute : public std::invalid_argument {
    std::string const name;

    duplicate_attribute(std::string const& name)
        : std::invalid_argument("duplicate attribute `" + name + "'")
        , name(name) {}

    ~duplicate_attribute() throw () {}
};

//
// file_error exception
////////////////////////////////////////////////////////////////////////////////

struct file_error : public std::runtime_error {
    std::string const filepath, action, reason;

    file_error( std::string const& filepath
              , std::string const& action
              , std::string const& reason
              )
        : std::runtime_error("could not " + action +
            " file `" + filepath + "' (" + reason + ")")
        , filepath(filepath), action(action), reason(reason) {}

    ~file_error() throw () {}
};

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_ENGINES_EXCEPTIONS_HPP_INCLUDED
