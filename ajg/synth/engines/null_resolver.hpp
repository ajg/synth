//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_NULL_RESOLVER_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_NULL_RESOLVER_HPP_INCLUDED

#include <map>
#include <string>

#include <boost/optional.hpp>
#include <boost/mpl/void.hpp>

namespace ajg {
namespace synth {

using boost::optional;

//
// null_resolver
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Options>
struct null_resolver : Options::abstract_resolver_type {
    typedef Options                               options_type;
    typedef typename options_type::string_type    string_type;
    typedef typename options_type::value_type     value_type;
    typedef typename options_type::context_type   context_type;
    typedef typename options_type::arguments_type arguments_type;
    typedef std::map<string_type, string_type>    patterns_type;

    virtual optional<string_type> resolve( string_type  const& path
                                         , context_type const& context
                                         , options_type const& options
                                         ) {
        return boost::none;
    }

    virtual optional<string_type> reverse( string_type    const& name
                                         , arguments_type const& arguments
                                         , context_type   const& context
                                         , options_type   const& options
                                         ) {
        typename patterns_type::const_iterator it = patterns_.find(name);
        if (it == patterns_.end()) {
            return boost::none;
        }

        string_type suffix;
        BOOST_FOREACH(value_type const& arg, arguments.first) {
            suffix += "/" + arg.to_string();
        }
        return it->second + suffix;
    }

    explicit null_resolver(patterns_type patterns) : patterns_(patterns) {}
    virtual ~null_resolver() {}

  private:

    patterns_type patterns_;
}; // null_resolver

}} // namespace ajg::synth

#endif // AJG_SYNTH_ENGINES_NULL_RESOLVER_HPP_INCLUDED

