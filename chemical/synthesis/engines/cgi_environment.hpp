
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ENGINES_CGI_ENVIRONMENT_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ENGINES_CGI_ENVIRONMENT_HPP_INCLUDED

#include <set>
#include <string>
#include <cstdlib>

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/assign/list_of.hpp>
#include <chemical/synthesis/engines/detail.hpp>

namespace chemical {
namespace synthesis {


template < class Engine
      // , char* (*Source)(char const*) = std::getenv
         >
struct cgi_environment {
  public:

    typedef Engine                             engine_type;
    typedef typename engine_type::char_type    char_type;
    typedef typename engine_type::value_type   value_type;
    typedef typename engine_type::string_type  string_type;
    typedef typename engine_type::context_type context_type;
    typedef std::set<string_type>              variables_type;

  public:

    cgi_environment(engine_type const& engine)
        : engine_(engine)
        , variables_(assign::list_of<string_type>
            (detail::text("AUTH_TYPE"))
            (detail::text("AUTH_USER"))
            (detail::text("CONTENT_LENGTH"))
            (detail::text("CONTENT_TYPE"))
            (detail::text("DOCUMENT_ROOT"))
            (detail::text("GATEWAY_INTERFACE"))
            (detail::text("HTTP_ACCEPT"))
            (detail::text("HTTP_ACCEPT_CHARSET"))
            (detail::text("HTTP_ACCEPT_ENCODING"))
            (detail::text("HTTP_ACCEPT_LANGUAGE"))
            (detail::text("HTTP_CONNECTION"))
            (detail::text("HTTP_COOKIE"))
            (detail::text("HTTP_FORWARDED"))
            (detail::text("HTTP_HOST"))
            (detail::text("HTTP_IF_MODIFIED_SINCE"))
            (detail::text("HTTP_PROXY_CONNECTION"))
            (detail::text("HTTP_REFERER"))
            (detail::text("HTTP_USER_AGENT"))
            (detail::text("HTTPS"))
            (detail::text("LAST_MODIFIED"))
            (detail::text("PATH"))
            (detail::text("PATH_INFO"))
            (detail::text("PATH_TRANSLATED"))
            (detail::text("QUERY_STRING"))
            (detail::text("REMOTE_ADDR"))
            (detail::text("REMOTE_HOST"))
            (detail::text("REMOTE_IDENT"))
            (detail::text("REMOTE_PORT"))
            (detail::text("REMOTE_USER"))
            (detail::text("REQUEST_METHOD"))
            (detail::text("REQUEST_URI"))
            (detail::text("SCRIPT_FILENAME"))
            (detail::text("SCRIPT_NAME"))
            (detail::text("SERVER_ADDR"))
            (detail::text("SERVER_ADMIN"))
            (detail::text("SERVER_NAME"))
            (detail::text("SERVER_PORT"))
            (detail::text("SERVER_PROTOCOL"))
            (detail::text("SERVER_SIGNATURE"))
            (detail::text("SERVER_SOFTWARE"))
                .to_container(variables_)
        ) {}

  public:

    void operator ()(context_type& context) const {
        BOOST_FOREACH(string_type const& variable, variables_) {
            if (!detail::find_value(variable, context)) {
                if (optional<value_type> const value = get(variable)) {
                    context.insert(std::make_pair(variable, *value));
                }
            }
        }
    }

  private:

    optional<value_type> get(string_type const& name) const {
        std::string const name_ = engine_.template convert<char>(name);
     // char const *const value = (*Source)(name_.c_str());
        if (char const *const value = std::getenv(name_.c_str())) {
            return value_type(engine_.template
                convert<char_type>(std::string(value)));
        }
        else {
            return none;
        }
    }

  private:

    engine_type const& engine_;
    variables_type const variables_;
};

/*
struct standard_environment {
    template <class Engine>
    typename Engine::value_type get( typename Engine::this_type const& engine
                                   , typename Engine::string_type const& name
                                   ) const {
        std::string const name_ = engine.template convert<char>(name);
        std::string const value = std::getenv(name_.c_str());
        typedef typename Engine::value_type::char_type C;
        return engine.template convert<C>(value);
    }

    optional<value_type> get(string_type const& name) const {
        std::string const name_ = engine_.template convert<char>(name);
     // char const *const value = (*Source)(name_.c_str());
        if (char const *const value = std::getenv(name_.c_str())) {
            return value_type(engine_.template
                convert<char_type>(std::string(value)));
        }
        else {
            return none;
        }
    }

    template <class Engine>
    void list( typename Engine::this_type const& engine
             , typename Engine::stream_type& out
             ) const {


    }
};*/

}} // namespace chemical::synthesis

#endif // CHEMICAL_SYNTHESIS_ENGINES_CGI_ENVIRONMENT_HPP_INCLUDED
