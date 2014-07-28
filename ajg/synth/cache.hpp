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
};

template <typename Template>
struct caching_for;

template <typename Engine>
inline void prime_all() {
    templates::buffer_template<Engine>::prime();
    templates::path_template<Engine>::prime();
    templates::stream_template<Engine>::prime();
    templates::string_template<Engine>::prime();
}

template <typename Engine>
struct caching_for<templates::buffer_template<Engine> > {
    BOOST_STATIC_CONSTANT(caching_flags, value = buffer_caching);
};

template <typename Engine>
struct caching_for<templates::path_template<Engine> > {
    BOOST_STATIC_CONSTANT(caching_flags, value = path_caching);
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

    inline boolean_type enabled(options_type const& options) const {
        caching_type const c = caching_for<template_type>::value;
        AJG_SYNTH_ASSERT((c & (c - 1)) == 0);
        return (options.caching & c) || (options.caching & all_caching);
    }

    cached_type get_or_parse(source_type source, options_type const& options) {
        std::cerr << "cache: size for (" << caching_for<template_type>::value << "): " << this->cache_.size() << std::endl;
        std::cerr << "cache: address for (" << caching_for<template_type>::value << "): " << &this->cache_ << std::endl;


        if (!this->enabled(options)) {
            std::cerr << "cache: disabled for (" << caching_for<template_type>::value << "): " << template_type::key(source) << std::endl;
            return cached_type(new template_type(source, options));
        }

        key_type const key = template_type::key(source);
        std::pair<it_type, it_type> const r = this->cache_.equal_range(key);

        for (it_type it = r.first; it != r.second; ++it) {
            if (!it->second->compatible(source, options)) {
                std::cerr << "cache: incompatible for (" << caching_for<template_type>::value << "): " << key << std::endl;
                continue;
            }
            else if (it->second->stale(source, options)) {
                std::cerr << "cache: stale for (" << caching_for<template_type>::value << "): " << key << std::endl;
                this->cache_.erase(it);
                break;
            }
            else {
                std::cerr << "cache: fresh for (" << caching_for<template_type>::value << "): " << key << std::endl;
                return it->second;
            }
        }

        std::cerr << "cache: missing for (" << caching_for<template_type>::value << "): " << key << std::endl;
        cached_type const t(new template_type(source, options));
        this->cache_.insert(std::pair<key_type, cached_type>(key, t));
        return t;
    }

  private:

    cache_type cache_;
};

template <typename Template>
extern inline typename cache<Template>::cached_type parse_template
        ( typename Template::source_type         source
        , typename Template::options_type const& options
        ) {
    static cache<Template> global_cache;
    // TODO: Make thread-safe or at least thread-local.
    return global_cache.get_or_parse(source, options);
}


}} // namespace ajg::synth

#endif // AJG_SYNTH_CACHE_HPP_INCLUDED
