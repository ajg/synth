//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_BASE_OPTIONS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_BASE_OPTIONS_HPP_INCLUDED

#include <boost/noncopyable.hpp>

namespace ajg {
namespace synth {
namespace engines {

//
// base_options
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Value>
struct base_options : boost::noncopyable {
  public:

    typedef Value                                                               value_type;

    typedef typename value_type::behavior_type                                  behavior_type;
    typedef typename value_type::range_type                                     range_type;
    typedef typename value_type::sequence_type                                  sequence_type;
    typedef typename value_type::mapping_type                                   mapping_type;
    typedef typename value_type::traits_type                                    traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::date_type                                     date_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::paths_type                                    paths_type;
    typedef typename traits_type::names_type                                    names_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;
};

}}} // namespace ajg::synth::engines

#endif // AJG_SYNTH_ENGINES_BASE_OPTIONS_HPP_INCLUDED

