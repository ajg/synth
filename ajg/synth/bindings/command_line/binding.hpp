//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_COMMAND_LINE_BINDING_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_COMMAND_LINE_BINDING_HPP_INCLUDED

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

#include <ajg/synth/bindings/base_binding.hpp>
#include <ajg/synth/templates/stream_template.hpp>

namespace ajg {
namespace synth {
namespace bindings {
namespace command_line {

namespace pt = boost::property_tree;

template <class Traits>
struct binding : bindings::base_binding<Traits, templates::stream_template> {
  public:

    typedef binding                                                             binding_type;
    typedef Traits                                                              traits_type;
    typedef typename binding::base_binding_type                                 base_type;

    typedef typename base_type::context_type                                    context_type;
    typedef typename base_type::options_type                                    options_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;
    typedef typename traits_type::paths_type                                    paths_type;

    typedef typename pt::basic_ptree<string_type, string_type>                  ptree_type; // TODO: basic_ptree<string_type, value_type>

  public:

    template <class Source>
    binding( Source&             source
           , string_type  const& engine
           , string_type  const& replacement
           , paths_type   const& directories
           ) : base_type(source, engine, get_options(replacement, directories)) {}

  private:

    inline static options_type get_options(string_type const& replacement, paths_type const& directories) {
        options_type options;
        options.default_value = replacement;
        options.debug         = false; // TODO: Turn into a flag.
        options.directories   = directories;
        return options;
    }

  public:

    void render_to_stream(ostream_type& ostream, ptree_type const& ptree) const {
        context_type context((ptree));
        return base_type::render_to_stream(ostream, context);
    }

    string_type render_to_string(ptree_type const& ptree) const {
        context_type context((ptree));
        return base_type::render_to_string(context);
    }

    void render_to_path(string_type const& path, ptree_type const& ptree) const {
        context_type context((ptree));
        return base_type::render_to_path(path, context);
    }
};

}}}} // namespace ajg::synth::bindings::command_line

#endif // AJG_SYNTH_BINDINGS_COMMAND_LINE_BINDING_HPP_INCLUDED
