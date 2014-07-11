//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_CACHE_HPP_INCLUDED
#define AJG_SYNTH_CACHE_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <string>
#include <vector>
#include <cstring>
// #include <sys/stat.h>

#include <ajg/synth/detail/text.hpp>
#include <ajg/synth/templates.hpp>

namespace ajg {
namespace synth {

template <class Options, class Engine>
struct cache {
  public:

    typedef cache                                                               cache_type;
    typedef Options                                                             options_type;
    typedef Engine                                                              engine_type;

    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::traits_type                                  traits_type;

    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::paths_type                                    paths_type;
    typedef typename traits_type::ostream_type                                  ostream_type;


    // TODO: source_type and source()
    // typedef std::pair<path_type, size_type>                                     info_type;

  /*
  private:

    // TODO[c++11]: Use unique_ptr
    typedef boost::shared_ptr<std::pair<std::time_t, state_type> >              cached_type;
    typedef std::map<path_type, cached_type>                                    cache_type;

    typedef detail::text<string_type>                                           text;


  public:

    cache() {

    }
  */

  public:

    inline static void prime() {
        templates::buffer_template<engine_type>::prime();
        templates::path_template<engine_type>::prime();
        templates::stream_template<engine_type>::prime();
        templates::string_template<engine_type>::prime();
    }

    void render_path_to_stream( path_type    const& path
                              , ostream_type&       ostream
                              , context_type&       context
                              , options_type const& options
                              ) {
        templates::path_template<engine_type> const t(path, options);
        return t.render_to_stream(ostream, context);
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_CACHE_HPP_INCLUDED
