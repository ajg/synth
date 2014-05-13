//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_TMPL_OPTIONS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_TMPL_OPTIONS_HPP_INCLUDED

#include <vector>

namespace ajg {
namespace synth {
namespace engines {
namespace tmpl {

enum tag_mode
    { xml
    , html
    , loose
    };

template <class Value>
struct options {
  public:

    typedef options                                                             options_type;
    typedef Value                                                               value_type;

    typedef typename value_type::traits_type                                    traits_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::paths_type                                    paths_type;

  public:

    BOOST_STATIC_CONSTANT(boolean_type,   case_sensitive   = false);
    BOOST_STATIC_CONSTANT(boolean_type,   shortcut_syntax  = true);
    BOOST_STATIC_CONSTANT(boolean_type,   loop_variables   = true);
    BOOST_STATIC_CONSTANT(boolean_type,   global_variables = false);
    BOOST_STATIC_CONSTANT(tmpl::tag_mode, tag_mode         = loose); // TODO: Implement.

  public:

    options(paths_type const& directories = paths_type()) : directories(directories) {}

  public:

    paths_type directories;
};

}}}} // namespace ajg::synth::engines::tmpl

#endif // AJG_SYNTH_ENGINES_TMPL_OPTIONS_HPP_INCLUDED
