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
    std::string const filepath, action, reason;

    file_error( std::string const& filepath
              , std::string const& action
              , std::string const& reason
              )
        : std::runtime_error("could not " + action + " file `" + filepath + "' (" + reason + ")")
        , filepath(filepath), action(action), reason(reason) {}

    ~file_error() throw () {}
};

namespace detail { std::string get_type_name(std::type_info const&); }

//
// conversion_error
//     TODO: Consider renaming bad_conversion.
////////////////////////////////////////////////////////////////////////////////////////////////////

struct conversion_error : public std::runtime_error {
    conversion_error(std::type_info const& a, std::type_info const& b)
        : std::runtime_error("could not convert value of type `"
                                 + detail::get_type_name(a) + "' to `"
                                 + detail::get_type_name(b) + "'") {}
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

}} // namespace ajg::synth

#endif // AJG_SYNTH_EXCEPTIONS_HPP_INCLUDED
