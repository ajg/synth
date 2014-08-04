//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_COMMAND_LINE_BINDING_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_COMMAND_LINE_BINDING_HPP_INCLUDED

#include <boost/property_tree/ptree.hpp>

#include <ajg/synth/engines.hpp>
#include <ajg/synth/bindings/base_binding.hpp>
#include <ajg/synth/templates/stream_template.hpp>

namespace ajg {
namespace synth {
namespace bindings {
namespace command_line {

namespace pt = boost::property_tree;

template <class Traits>
struct binding : bindings::base_binding< Traits
                                       // TODO: Use value_type
                                       , pt::basic_ptree<typename Traits::string_type, typename Traits::string_type>
                                       , templates::stream_template
                                       , engines::django::engine
                                       , engines::ssi::engine
                                       , engines::tmpl::engine
                                       > {
  public:

    typedef binding                                                             binding_type;
    typedef Traits                                                              traits_type;
    typedef typename binding::base_binding_type                                 base_type;

    typedef typename base_type::source_type                                     source_type;
    typedef typename base_type::options_type                                    options_type;

    typedef typename traits_type::string_type                                   string_type;

  public:

    binding( source_type         source
           , string_type  const& engine
           , options_type const& options
           ) : base_type(source, engine, options) {}

  public:

    using base_type::render_to_stream;
    using base_type::render_to_string;
    using base_type::render_to_path;
};

}}}} // namespace ajg::synth::bindings::command_line

#endif // AJG_SYNTH_BINDINGS_COMMAND_LINE_BINDING_HPP_INCLUDED
