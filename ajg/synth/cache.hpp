//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_CACHE_HPP_INCLUDED
#define AJG_SYNTH_CACHE_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <map>
#include <string>
#include <vector>

#include <ajg/synth/detail/text.hpp>
#include <ajg/synth/templates.hpp>

namespace ajg {
namespace synth {

enum caching_flags {
    no_caching     = 0,
    all_caching    = 1,
    path_caching   = 2,
    buffer_caching = 4,
    string_caching = 8
    // TODO: timestamp/checksum/forever/aggressive
};

template <class Options, class Engine>
struct cache {
  public:

    typedef cache                                                               cache_type;
    typedef Options                                                             options_type;
    typedef Engine                                                              engine_type;

    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::traits_type                                  traits_type;
    typedef typename options_type::caching_type                                 caching_type;

    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::paths_type                                    paths_type;
    typedef typename traits_type::buffer_type                                   buffer_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    // TODO: source_type and source()
    // typedef std::pair<path_type, size_type>                                     info_type;

  private:

    typedef templates::path_template<engine_type>                               path_template_type;
    typedef boost::shared_ptr<path_template_type const>                         path_cached_type; // TODO[c++11]: Use unique_ptr
    typedef std::multimap<path_type, path_cached_type>                          path_cache_type;

    typedef templates::buffer_template<engine_type>                             buffer_template_type;
    typedef boost::shared_ptr<buffer_template_type const>                       buffer_cached_type; // TODO[c++11]: Use unique_ptr
    typedef std::multimap<size_type, buffer_cached_type>                        buffer_cache_type;

    typedef detail::text<string_type>                                           text;


  public:

    cache() {}

  public:

    inline static void prime() {
        buffer_template_type::prime();
        path_template_type::prime();
        templates::stream_template<engine_type>::prime();
        templates::string_template<engine_type>::prime();
    }

    inline static boolean_type enabled(caching_type const c, options_type const& options) {
        AJG_SYNTH_ASSERT((c & (c - 1)) == 0);
        return (options.caching & c) || (options.caching & all_caching);
    }

    buffer_cached_type get_or_parse(buffer_type const& buffer, options_type const& options) {
        if (!cache_type::enabled(buffer_caching, options)) {
            return buffer_cached_type(new buffer_template_type(buffer, options));
        }

        typedef typename buffer_cache_type::iterator it_type;
        std::pair<it_type, it_type> const r = this->buffer_cache_.equal_range(buffer.second);

        for (it_type it = r.first; it != r.second; ++it) {
            if (!it->second->compatible(buffer, options)) {
                continue;
            }
            else if (it->second->stale(buffer, options)) {
                this->buffer_cache_.erase(it);
                break;
            }
            else {
                return it->second;
            }
        }

        buffer_cached_type const t(new buffer_template_type(buffer, options));
        this->buffer_cache_.insert(std::pair<size_type, buffer_cached_type>(buffer.second, t));
        return t;
    }

    path_cached_type get_or_parse(path_type const& path, options_type const& options) {
        if (!cache_type::enabled(path_caching, options)) {
            return path_cached_type(new path_template_type(path, options));
        }

        typedef typename path_cache_type::iterator it_type;
        std::pair<it_type, it_type> const r = this->path_cache_.equal_range(path);

        for (it_type it = r.first; it != r.second; ++it) {
            if (!it->second->compatible(path, options)) {
                continue;
            }
            else if (it->second->stale(path, options)) {
                this->path_cache_.erase(it);
                break;
            }
            else {
                return it->second;
            }
        }

        path_cached_type const t(new path_template_type(path, options));
        this->path_cache_.insert(std::pair<path_type, path_cached_type>(path, t));
        return t;
    }

  private:

    path_cache_type   path_cache_;
    buffer_cache_type buffer_cache_;
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_CACHE_HPP_INCLUDED
