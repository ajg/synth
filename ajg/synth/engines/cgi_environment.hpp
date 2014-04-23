//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_CGI_ENVIRONMENT_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_CGI_ENVIRONMENT_HPP_INCLUDED

#if AJG_SYNTH_UNUSED

#include <set>
#include <string>
#include <cstdlib>

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/assign/list_of.hpp>
#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace engines {


template < class Engine
      // , char* (*Source)(char const*) = std::getenv
         >
struct cgi_environment {
  public:

    typedef Engine                             engine_type;
    typedef typename engine_type::traits_type  traits_type;
    typedef typename engine_type::char_type    char_type;
    typedef typename engine_type::value_type   value_type;
    typedef typename engine_type::string_type  string_type;
    typedef typename engine_type::context_type context_type;
    typedef std::set<string_type>              variables_type;

  public:

    cgi_environment(engine_type const& engine)
        : engine_(engine)
        , variables_(assign::list_of<string_type>
            (traits_type::literal("AUTH_TYPE"))
            (traits_type::literal("AUTH_USER"))
            (traits_type::literal("CONTENT_LENGTH"))
            (traits_type::literal("CONTENT_TYPE"))
            (traits_type::literal("DOCUMENT_ROOT"))
            (traits_type::literal("GATEWAY_INTERFACE"))
            (traits_type::literal("HTTP_ACCEPT"))
            (traits_type::literal("HTTP_ACCEPT_CHARSET"))
            (traits_type::literal("HTTP_ACCEPT_ENCODING"))
            (traits_type::literal("HTTP_ACCEPT_LANGUAGE"))
            (traits_type::literal("HTTP_CONNECTION"))
            (traits_type::literal("HTTP_COOKIE"))
            (traits_type::literal("HTTP_FORWARDED"))
            (traits_type::literal("HTTP_HOST"))
            (traits_type::literal("HTTP_IF_MODIFIED_SINCE"))
            (traits_type::literal("HTTP_PROXY_CONNECTION"))
            (traits_type::literal("HTTP_REFERER"))
            (traits_type::literal("HTTP_USER_AGENT"))
            (traits_type::literal("HTTPS"))
            (traits_type::literal("LAST_MODIFIED"))
            (traits_type::literal("PATH"))
            (traits_type::literal("PATH_INFO"))
            (traits_type::literal("PATH_TRANSLATED"))
            (traits_type::literal("QUERY_STRING"))
            (traits_type::literal("REMOTE_ADDR"))
            (traits_type::literal("REMOTE_HOST"))
            (traits_type::literal("REMOTE_IDENT"))
            (traits_type::literal("REMOTE_PORT"))
            (traits_type::literal("REMOTE_USER"))
            (traits_type::literal("REQUEST_METHOD"))
            (traits_type::literal("REQUEST_URI"))
            (traits_type::literal("SCRIPT_FILENAME"))
            (traits_type::literal("SCRIPT_NAME"))
            (traits_type::literal("SERVER_ADDR"))
            (traits_type::literal("SERVER_ADMIN"))
            (traits_type::literal("SERVER_NAME"))
            (traits_type::literal("SERVER_PORT"))
            (traits_type::literal("SERVER_PROTOCOL"))
            (traits_type::literal("SERVER_SIGNATURE"))
            (traits_type::literal("SERVER_SOFTWARE"))
                .to_container(variables_)
        ) {}

  public:

    void operator ()(context_type& context) const {
        BOOST_FOREACH(string_type const& variable, variables_) {
            if (!detail::find(variable, context)) { // TODO: detail::contains.
                if (optional<value_type> const value = get(variable)) {
                    context.insert(std::make_pair(variable, *value));
                }
            }
        }
    }

  private:

    optional<value_type> get(string_type const& name) const {
        std::string const name_ = traits_type::narrow(name);
     // char const *const value = (*Source)(name_.c_str());
        if (char const* const value = (std::getenv)(name_.c_str())) {
            return value_type(traits_type::widen(std::string(value)));
        }
        else {
            return none;
        }
    }

  private:

    engine_type    const& engine_;
    variables_type const  variables_;
};

/*
struct standard_environment {
    template <class Engine>
    typename Engine::value_type get(typename Engine::string_type const& name) const {
        std::string const name_ = traits_type::narrow(name);
        std::string const value = std::getenv(name_.c_str());
        return value_type(traits_type::widen(value));
    }

    optional<value_type> get(string_type const& name) const {
        std::string const name_ = traits_type::narrow(name);
     // char const *const value = (*Source)(name_.c_str());
        if (char const *const value = std::getenv(name_.c_str())) {
            return value_type(traits_type::widen(std::string(value)));
        }
        else {
            return none;
        }
    }
};*/

}}} // namespace ajg::synth::engines

#endif // AJG_SYNTH_UNUSED

#endif // AJG_SYNTH_ENGINES_CGI_ENVIRONMENT_HPP_INCLUDED
