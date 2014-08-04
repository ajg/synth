//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_NULL_RESOLVER_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_NULL_RESOLVER_HPP_INCLUDED

#include <map>
#include <string>

#include <boost/optional.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace null {

template <class Options>
struct resolver : Options::abstract_resolver {
  public:

    typedef Options                                                             options_type;
    typedef typename options_type::traits_type                                  traits_type;
    typedef typename options_type::size_type                                    size_type;
    typedef typename options_type::string_type                                  string_type;
    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::url_type                                     url_type;
    typedef typename options_type::arguments_type                               arguments_type;
    typedef std::map<string_type, string_type>                                  patterns_type;

  private:

    typedef detail::text<string_type>                                           text;

  public:

    virtual url_type resolve( string_type  const& path
                            , context_type const& context
                            , options_type const& options
                            ) {
        return url_type();
    }

    virtual url_type reverse( string_type    const& name
                            , arguments_type const& arguments
                            , context_type   const& context
                            , options_type   const& options
                            ) {
        // NOTE: This resolver is only meant to be used in tests, as it does no escaping.
        typename patterns_type::const_iterator it = patterns_.find(name);
        if (it == patterns_.end()) {
            return url_type();
        }

        string_type path, query;
        for (auto const& arg : arguments.first) {
            path += text::literal("/") + arg.to_string();
        }
        size_type i = 0;
        for (auto const& karg : arguments.second) {
            query += i++ ? text::literal("&") : text::literal("?");
            query += karg.first + text::literal("=") + karg.second.to_string();
        }
        return url_type(it->second + path + query);
    }

    explicit resolver(patterns_type patterns) : patterns_(patterns) {}
    virtual ~resolver() {}

  private:

    patterns_type patterns_;
}; // resolver

}}}} // namespace ajg::synth::engines::null

#endif // AJG_SYNTH_ENGINES_NULL_RESOLVER_HPP_INCLUDED

