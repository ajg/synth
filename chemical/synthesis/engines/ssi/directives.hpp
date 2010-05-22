
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ENGINES_SSI_DIRECTIVES_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ENGINES_SSI_DIRECTIVES_HPP_INCLUDED

#include <string>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <chemical/synthesis/engines/detail.hpp>

namespace chemical {
namespace synthesis {
namespace ssi {

using detail::text;

#define FOR_EACH_ATTRIBUTE(if_statement) { \
    typename Engine::match_type const& attrs = detail::get_nested<1>(args.match); \
    BOOST_FOREACH(typename Engine::match_type const& attr, attrs.nested_results()) { \
        typename Engine::string_type const value = args.engine.attribute_string(attr(args.engine.quoted_value)); \
        typename Engine::string_type const name = algorithm::to_lower_copy(attr(args.engine.name).str()); \
        if_statement else throw_exception(invalid_attribute(args.engine.template convert<char>(name))); \
    } \
}

//
// config_directive
////////////////////////////////////////////////////////////////////////////////

struct config_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("config"));
        }
        
        void render(typename Engine::args_type const& args) const {
            FOR_EACH_ATTRIBUTE(
                if (name == text("sizefmt")) {
                    detail::validate_option(value, "sizefmt",
                        assign::list_of("bytes")("abbrev"));
                    args.options.size_format = value;
                }
                else if (name == text("timefmt")) args.options.time_format = value;
                else if (name == text("echomsg")) args.options.echo_message = value;
                else if (name == text("errmsg"))  args.options.error_message = value;
            );
        }
    };
};


//
// echo_directive
////////////////////////////////////////////////////////////////////////////////

struct echo_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("echo"));
        }
        
        void render(typename Engine::args_type const& args) const {
            typename Engine::string_type encoding = text("entity");
            FOR_EACH_ATTRIBUTE(
                if (name == text("var")) {
                    typename Engine::string_type const result = 
                        args.engine.lookup_variable(args.context, args.options, value);

                         if (encoding == text("none"))   args.stream << result;
                    else if (encoding == text("url"))    args.stream << detail::uri_encode(result);
                    else if (encoding == text("entity")) args.stream << detail::escape_entities(result);
                    else BOOST_ASSERT(false);
                }
                else if (name == text("encoding")) {
                    detail::validate_option(value, "encoding",
                        assign::list_of("none")("url")("entity"));
                    encoding = value;
                }
            );
        }
    };
};

//
// exec_directive
////////////////////////////////////////////////////////////////////////////////

struct exec_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("exec"));
        }
        
        void render(typename Engine::args_type const& args) const {
            FOR_EACH_ATTRIBUTE(
                if (name == text("cgi")) {
                    // TODO:
                    // BOOST_ASSERT(detail::file_exists(value));
                    throw_exception(not_implemented("exec cgi"));
                }
                else if (name == text("cmd")) {
                    std::string const command = args.engine.template convert<char>(value);
                    detail::pipe pipe(command);
                    pipe.read_into(args.stream);
                }
            );
        }
    };
};
//
// flastmod_directive
////////////////////////////////////////////////////////////////////////////////

struct flastmod_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("flastmod"));
        }
        
        void render(typename Engine::args_type const& args) const {
            FOR_EACH_ATTRIBUTE(
                if (name == text("virtual")) {
                    // TODO: Parse REQUEST_URI and figure our path out.
                    throw_exception(not_implemented("fsize virtual"));
                }
                else if (name == text("file")) {
                    std::time_t const stamp = detail::stat_file(value).st_mtime;
                    args.stream << detail::format_time(args.options.time_format,
                        posix_time::from_time_t(stamp));
                }
            );
        }
    };
};

//
// fsize_directive
////////////////////////////////////////////////////////////////////////////////

struct fsize_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("fsize"));
        }
        
        void render(typename Engine::args_type const& args) const {
            bool const abbreviate = args.options.size_format == text("abbrev");
            detail::validate_option(args.options.size_format, "size_format",
                assign::list_of("bytes")("abbrev"));

            FOR_EACH_ATTRIBUTE(
                if (name == text("virtual")) {
                    // TODO: Parse REQUEST_URI and figure our path out.
                    throw_exception(not_implemented("fsize virtual"));
                }
                else if (name == text("file")) {
                    uintmax_t const size = detail::stat_file(value).st_size;
                    !abbreviate ? args.stream << size
                                : args.stream << detail::abbreviate_size
                                    <typename Engine::string_type>(size);
                }
            );
        }
    };
};

//
// if_directive
////////////////////////////////////////////////////////////////////////////////

struct if_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return  (engine.directive(text("if"))   >> engine.block) // A
                >> *(engine.directive(text("elif")) >> engine.block) // B
                >> !(engine.directive(text("else")) >> engine.block) // C
                >>  (engine.directive(text("endif")));               // D
        }
        
        void render(typename Engine::args_type const& args) const {

            {
              /*typename Engine::match_type const& if_   = get_nested<A>(args.match);
                typename Engine::match_type const& elifs = get_nested<B>(args.match);
                typename Engine::match_type const& else_ = get_nested<C>(args.match);
                typename Engine::match_type const& endif = get_nested<D>(args.match);

                DUMP(if_.str());
                DUMP(elifs.str());
                DUMP(else_.str());
                DUMP(endif.str());*/
            }


            /*BOOST_FOREACH( typename Engine::match_type const& nested
                         , args.match.nested_results()) {
                DUMP(nested.str());
                DUMP(nested.regex_id());
                DUMP(nested.nested_results().size());
                typename Engine::string_type const name =
                    nested[xpressive::s1].str();
                DUMP(name);

                if (name == text("if")) {
                    SAY("if");
                }
                else if (name == text("elif")) {
                    SAY("elif");
                }
                else if (name == text("else")) {
                    SAY("else");
                }
                else if (name == text("endif")) {
                    SAY("endif");
                }
            }*/
        }
    };
};

//
// include_directive
////////////////////////////////////////////////////////////////////////////////

struct include_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("include"));
        }
        
        void render(typename Engine::args_type const& args) const {
            FOR_EACH_ATTRIBUTE(
                if (name == text("virtual")) {
                    // TODO: Parse REQUEST_URI and figure our path out.
                    throw_exception(not_implemented("include virtual"));
                }
                else if (name == text("file")) {
                    args.engine.render_file(args.stream, value, args.context, args.options);
                }
            );
        }
    };
};

//
// printenv_directive
////////////////////////////////////////////////////////////////////////////////

struct printenv_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("printenv"));
        }

        void render(typename Engine::args_type const& args) const {
            FOR_EACH_ATTRIBUTE(
                if (false) {} // Do nothing.
            );

            typedef typename Engine::char_type char_type;
            typedef typename Engine::environment_type::value_type name_value;

            BOOST_FOREACH(name_value const& nv, args.engine.environment) {
                args.stream << args.engine.template convert<char_type>(nv.first) << '='
                            << args.engine.template convert<char_type>(nv.second) << std::endl;
            }
        }
    };
};

//
// set_directive
////////////////////////////////////////////////////////////////////////////////

struct set_directive {
    template <class Engine>
    struct definition {
        definition() :
            variables_("${" >> (xpressive::s1 = +xpressive::_w) >> '}') {}

        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("set"));
        }

        static typename Engine::string_type replace
            ( typename Engine::args_type         const& args
            , typename Engine::string_match_type const& match
            ) {
            return args.engine.lookup_variable(args.context,
                args.options, match[xpressive::s1].str());
        }
        
        void render(typename Engine::args_type const& args) const {
            optional<typename Engine::string_type> name_;
            optional<typename Engine::value_type>  value_;

            FOR_EACH_ATTRIBUTE(
                if (name == text("var")) {
                    if (name_) throw_exception(invalid_attribute("duplicate name"));
                    else name_ = value;
                }
                else if (name == text("value")) {
                    if (value_) throw_exception(invalid_attribute("duplicate value"));

                    boost::function<typename Engine::string_type(
                            typename Engine::string_match_type const&)> const
                        formatter = boost::bind(&definition::replace, boost::cref(args), _1);
                    value_ = xpressive::regex_replace(value, variables_, formatter);
                }
            );

            if (!name_)  throw_exception(missing_attribute("name"));
            if (!value_) throw_exception(missing_attribute("value"));

            args.context[*name_] = *value_;
        }

      private:

        typename Engine::string_regex_type const variables_;
    };
};

#undef FOR_EACH_ATTRIBUTE

}}} // namespace chemical::synthesis::ssi

#endif // CHEMICAL_SYNTHESIS_ENGINES_SSI_DIRECTIVES_HPP_INCLUDED

