//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_SSI_OPTIONS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_SSI_OPTIONS_HPP_INCLUDED

#include <vector>

#include <ajg/synth/detail/text.hpp>
#include <ajg/synth/engines/base_options.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace ssi {

template <class Value>
struct options : base_options<Value> {
  public:

    typedef options                                                             options_type;
    typedef Value                                                               value_type;

    typedef typename value_type::traits_type                                    traits_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::paths_type                                    paths_type;

  private:

    typedef detail::text<string_type>                                           text;

  public:

    BOOST_STATIC_CONSTANT(boolean_type, throw_on_errors    = false);
    BOOST_STATIC_CONSTANT(size_type,    max_regex_captures = 9);

  public:

    options( string_type const& echo_message  = text::literal("(none)")
           , paths_type  const& directories   = paths_type()
           , string_type const& size_format   = text::literal("bytes")
           , string_type const& time_format   = text::literal("%A, %d-%b-%Y %H:%M:%S %Z")
           , string_type const& error_message = text::literal("[an error occurred while processing this directive]")
           )
        : echo_message(echo_message)
        , directories(directories)
        , size_format(size_format)
        , time_format(time_format)
        , error_message(error_message) {}

  public:

    string_type  echo_message;
    paths_type   directories;
    string_type  size_format;
    string_type  time_format;
    string_type  error_message;
};

}}}} // namespace ajg::synth::engines::ssi

#endif // AJG_SYNTH_ENGINES_SSI_OPTIONS_HPP_INCLUDED
