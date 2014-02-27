//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_SSI_OPTIONS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_SSI_OPTIONS_HPP_INCLUDED

#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace ssi {

template <class String>
struct options {
    typedef String string_type;

    options()
        : size_format(detail::text("bytes"))
        , time_format(detail::text("%A, %d-%b-%Y %H:%M:%S %Z"))
        , echo_message(detail::text("(none)"))
        , error_message(detail::text("[an error occurred while processing this directive]")) {}

    string_type size_format;
    string_type time_format;
    string_type echo_message;
    string_type error_message;
};

}}} // namespace ajg::synth::ssi

#endif // AJG_SYNTH_ENGINES_SSI_OPTIONS_HPP_INCLUDED
