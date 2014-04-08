//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_SSI_OPTIONS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_SSI_OPTIONS_HPP_INCLUDED

#include <vector>

#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace ssi {

template <class Value>
struct options {
    typedef options                                options_type;
    typedef Value                                  value_type;
    typedef typename value_type::traits_type       traits_type;
    typedef typename traits_type::string_type      string_type;
    typedef std::vector<string_type>               directories_type;

    options( string_type      const& echo_message  = traits_type::literal("(none)")
           , directories_type const& directories   = directories_type(/*1, "."*/)
           , string_type      const& size_format   = traits_type::literal("bytes")
           , string_type      const& time_format   = traits_type::literal("%A, %d-%b-%Y %H:%M:%S %Z")
           , string_type      const& error_message = traits_type::literal("[an error occurred while processing this directive]")
           )
        : echo_message(echo_message)
        , directories(directories)
        , size_format(size_format)
        , time_format(time_format)
        , error_message(error_message) {}

    string_type      echo_message;
    directories_type directories;
    string_type      size_format;
    string_type      time_format;
    string_type      error_message;
};

}}} // namespace ajg::synth::ssi

#endif // AJG_SYNTH_ENGINES_SSI_OPTIONS_HPP_INCLUDED
