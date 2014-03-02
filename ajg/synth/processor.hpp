//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_PROCESSOR_HPP_INCLUDED
#define AJG_SYNTH_PROCESSOR_HPP_INCLUDED

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

namespace ajg {
namespace synth {

namespace pt = boost::property_tree;

template <class MultiTemplate>
struct processor : MultiTemplate {

  public:

    typedef MultiTemplate                                      base_type;
    typedef typename base_type::stream_type                    stream_type;
    typedef typename base_type::boolean_type                   boolean_type;
    typedef typename base_type::string_type                    string_type;
    typedef typename base_type::directories_type               directories_type;
    typedef typename pt::basic_ptree<string_type, string_type> context_type; // TODO: basic_ptree<string_type, value_type>

  public:

    template <class Source>
    processor( Source&                 source
             , string_type      const& engine_name
             , boolean_type     const  autoescape
             , string_type      const& default_value
             , directories_type const& directories
             ) : base_type(source, engine_name, autoescape, default_value, directories) {}

  public:

    void render( stream_type&        stream
               , context_type const& context = context_type()
            // , options_type const& options = options_type()
               ) const {
        return base_type::template render<processor>(stream, context);
    }

    string_type render_to_string( context_type const& context = context_type()
                             // , options_type const& options = options_type()
                                ) const {
        return base_type::template render_to_string<processor>(context);
    }

    void render_to_file( string_type  const& filepath
                       , context_type const& context = context_type()
                    // , options_type const& options = options_type()
                       ) const {
        return base_type::template render_to_file<processor>(filepath, context);
    }

  public: // TODO: Replace (in c++11) with `friend MultiTemplate;`

    template <class Context>
    inline static Context adapt_context(context_type const& parent) {
        Context context;

        BOOST_FOREACH(typename context_type::value_type const& child, parent) {
            context[child.first] = child.second;
        }

        return context;
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_PROCESSOR_HPP_INCLUDED

