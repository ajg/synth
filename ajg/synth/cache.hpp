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

template <typename Engine>
inline void prime_all() {
    templates::buffer_template<Engine>::prime();
    templates::path_template<Engine>::prime();
    templates::stream_template<Engine>::prime();
    templates::string_template<Engine>::prime();
}

enum caching_mask {
    caching_none        = 0,
    caching_all         = (1 << 0),
    caching_paths       = (1 << 1),
    caching_buffers     = (1 << 2),
    caching_strings     = (1 << 3),
    // caching_streams  = (1 << 4),

    caching_per_thread  = (1 << 10),
    caching_per_process = (1 << 11)
};

template <typename Template>
struct caching_mask_for;

template <typename Engine>
struct caching_mask_for<templates::buffer_template<Engine> > {
    static caching_mask const value = caching_buffers;
};

template <typename Engine>
struct caching_mask_for<templates::path_template<Engine> > {
    static caching_mask const value = caching_paths;
};

template <typename Engine>
struct caching_mask_for<templates::stream_template<Engine> > {
    static caching_mask const value = caching_none;
};

template <typename Engine>
struct caching_mask_for<templates::string_template<Engine> > {
    static caching_mask const value = caching_strings;
};

template <class Template>
struct cache {
  public:

    typedef Template                                                            template_type;

    typedef typename template_type::options_type                                options_type;
    typedef typename template_type::engine_type                                 engine_type;
    typedef typename template_type::source_type                                 source_type;
    typedef typename template_type::key_type                                    key_type;

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

    typedef boost::shared_ptr<template_type const>                              cached_type; // TODO[c++11]: Use unique_ptr?

  private:

    typedef std::multimap<key_type, cached_type>                                cache_type;
    typedef typename cache_type::iterator                                       it_type;
    typedef detail::text<string_type>                                           text;


  public:

    cache() {}

  public:

    cached_type get_or_parse(source_type const& source, options_type const& options) {
        key_type const key = template_type::key(source);
        std::pair<it_type, it_type> const r = this->cache_.equal_range(key);

        for (it_type it = r.first; it != r.second; ++it) {
            if (it->second->same(source, options)) {
                if (it->second->stale(source, options)) {
                    // TODO: Introduce a way to reuse the template's state by re-parsing the source,
                    //       that way the contained xpressive::match_results can be reused too,
                    //       which is recommended as it is consumes a good chunk of memory.
                    it->second.reset(new template_type(source, options));
                }
                return it->second;
            }
        }

        cached_type const t(new template_type(source, options));
        this->cache_.insert(std::pair<key_type, cached_type>(key, t));
        return t;
    }

  private:

    cache_type cache_;
};

// TODO: Make the cache used a parameter.
template <typename Template>
inline typename cache<Template>::cached_type parse_template
        ( typename Template::source_type         source
        , typename Template::options_type const& options
        ) {
    caching_mask const m = caching_mask_for<Template>::value;
    AJG_SYNTH_ASSERT((m & (m - 1)) == 0);
    bool const enabled = (options.caching & m) || (options.caching & caching_all);
    if (!enabled) {
        return typename cache<Template>::cached_type(new Template(source, options));
    }
    // XXX: static cache<Template> global_cache;

    else if (options.caching & caching_per_thread) {
        // FIXME: Destroy at program end to avoid leak (currently sigsegvs from Python.)
        static AJG_SYNTH_THREAD_LOCAL cache<Template>* thread_cache = 0;
        if (thread_cache == 0) thread_cache = new cache<Template>;
        return thread_cache->get_or_parse(source, options);
    }
    else if (options.caching & caching_per_process) {
        // FIXME: Destroy at program end to avoid leak (currently sigsegvs from Python.)
        // FIXME: Make thread-safe (consider using concurrent hopscotch hashing.)
        static cache<Template>* process_cache = new cache<Template>;
        return process_cache->get_or_parse(source, options);
    }
    AJG_SYNTH_THROW(std::invalid_argument("caching must be per-process or per-thread"));
}

}} // namespace ajg::synth

#endif // AJG_SYNTH_CACHE_HPP_INCLUDED
