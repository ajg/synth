
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ENGINES_SSI_DIRECTIVES_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ENGINES_SSI_DIRECTIVES_HPP_INCLUDED

#include <string>
#include <functional>

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
using detail::get_nested;
using detail::operator ==;
using namespace detail::placeholders;

#define FOREACH_ATTRIBUTE_IN(x, if_statement) do { \
    typename Engine::match_type const& attrs = get_nested<1>(x); \
    BOOST_FOREACH(typename Engine::match_type const& attr, attrs.nested_results()) { \
        /* TODO: value, and possibly name, need to be unencoded (html entities) before processing, in some cases. */ \
        typename Engine::string_type const value = args.engine.attribute_string(attr(args.engine.quoted_value)); \
        typename Engine::string_type const name = algorithm::to_lower_copy(attr(args.engine.name).str()); \
        if_statement else throw_exception(invalid_attribute(args.engine.template convert<char>(name))); \
    } \
} while (0)

#define NO_ATTRIBUTES_IN(x) FOREACH_ATTRIBUTE_IN(x, if (false) {})

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
            FOREACH_ATTRIBUTE_IN(args.match,
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
            FOREACH_ATTRIBUTE_IN(args.match,
                if (name == text("var")) {
                    typename Engine::string_type const result = 
                        args.engine.lookup_variable(args.context, args.options, value);

                         if (encoding == text("none"))   args.stream << result;
                    else if (encoding == text("url"))    args.stream << detail::uri_encode(result);
                    else if (encoding == text("entity")) args.stream << detail::escape_entities(result);
                    else CHEMICAL_UNREACHABLE;
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
            FOREACH_ATTRIBUTE_IN(args.match,
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
            FOREACH_ATTRIBUTE_IN(args.match,
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

            FOREACH_ATTRIBUTE_IN(args.match,
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
            return  (engine.directive(text("if"))   >> engine.block)
                >> *(engine.directive(text("elif")) >> engine.block)
                >> !(engine.directive(text("else")) >> engine.block)
                >>  (engine.directive(text("endif")));
        }
        
        typename Engine::string_regex_type
            variable, raw_string, quoted_string,
            expression, primary_expression, not_expression,
            and_expression, or_expression, comparison_expression,
            string_expression, regex_expression, comparison_operator;

        void initialize(Engine const& engine) {
            using namespace xpressive;

            quoted_string
                = '"'  >> *(~as_xpr('"')  | "\\\"") >> '"'
                | '`'  >> *(~as_xpr('`')  | "\\`")  >> '`'
                | '\'' >> *(~as_xpr('\'') | "\\'")  >> '\''
                ;
            raw_string
                = /*+*/*~set[space | (set= '!', '&', '|', '$', '=', 
                      '(', ')', '{', '}', '<', '>', '"', '`', '\'', '\\')]
                ;
            regex_expression
                = '/' >> *(~as_xpr('/') | "\\\\")  >> '/'
                ;
            variable
                = "${" >> *~as_xpr('}') >> '}'
                | "$" >> +_w
                ;
            string_expression // A
                = quoted_string
                | raw_string
                | variable
                ;
            expression // A
                = xpressive::ref(and_expression)
                | xpressive::ref(or_expression)
                ;
            primary_expression // A
                = '(' >> *_s >> expression >> *_s >> ')'
                | xpressive::ref(comparison_expression)
                | xpressive::ref(string_expression)
                | xpressive::ref(not_expression)
                ;
            not_expression // ! A
                = '!' >> *_s >> expression
                ;
            comparison_operator
                = as_xpr("=") | "==" | "!=" | "<" | ">" | "<=" | ">="
                ;
            comparison_expression // A op C
                = string_expression >> *_s >> comparison_operator
                      >> *_s >> (string_expression | regex_expression)
                ;
            and_expression // A (&& B)*
                = primary_expression >> *(*_s >> "&&" >> *_s >> expression)
                ;
            or_expression // A (|| B)*
                = primary_expression >> *(*_s >> "||" >> *_s >> expression)
                ;
        }
        
        bool compare( typename Engine::args_type         const& args
                    , typename Engine::string_match_type const& expr) const {
            string_type const op = expr(comparison_operator).str();
            string_type const left = get_nested<A>(expr).str();
            string_type const right = get_nested<C>(expr).str();
                
                 if (op == text("="))  return left == right;
            else if (op == text("==")) return left == right;
            else if (op == text("!=")) return left != right;
            else if (op == text("<"))  return left <  right;
            else if (op == text(">"))  return left >  right;
            else if (op == text("<=")) return left <= right;
            else if (op == text(">=")) return left >= right;
            else CHEMICAL_UNREACHABLE;
        }

        template <class Args, class Match, class Functor>
        bool fold(Args const& args, Match const& match, bool initial, Functor const& functor) const {
            BOOST_FOREACH( typename Engine::string_match_type const& operand, match.nested_results()) {
                initial = functor(initial, evaluate(args, operand));
            }

            return initial;
        }

        bool evaluate( typename Engine::args_type         const& args
                     , typename Engine::string_match_type const& expr) const {
            if (expr == expression
             || expr == primary_expression
             || expr == string_expression) {
                // Simply recurse down one 'nesting' level.
                return evaluate(args, get_nested<A>(expr));
            }
            else if (expr == not_expression) {
                return !evaluate(args, get_nested<A>(expr));
            }
            else if (expr == comparison_expression) {
                return compare(args, expr);
            }
            else if (expr == and_expression) {
                return fold(args, expr, true, std::logical_and<bool>());
            }
            else if (expr == or_expression) {
                return fold(args, expr, false, std::logical_or<bool>());
            }
            else if (expr == raw_string) {
                return expr.length() != 0;
            }
            else if (expr == quoted_string) {
                return !args.engine.attribute_string(expr).empty();
            }
            else {
                CHEMICAL_UNREACHABLE;
            }
        }
        
        void render(typename Engine::args_type const& args) const {
            bool condition = false;
            typename Engine::string_match_type match;

            BOOST_FOREACH( typename Engine::match_type const& nested
                         , args.match.nested_results()) {

                if (nested == args.engine.block) {
                    if (condition) {
                        args.engine.render_block(args.stream, nested,
                            args.context, args.options);
                        break;
                    }
                }
                else {
                    typename Engine::string_type const name =
                        nested[xpressive::s1].str();

                    if (name == text("if")
                     || name == text("elif")) {
                        bool set = false;

                        FOREACH_ATTRIBUTE_IN(nested,
                            if (name == text("expr")) {
                                if (set) {
                                    throw_exception(duplicate_attribute("expr"));
                                }
                                else {
                                    set = true;
                                }

                                if (xpressive::regex_match(value, match, expression)) {
                                    condition = evaluate(args, match);
                                }
                                else {
                                    throw_exception(invalid_attribute("expr"));
                                }
                            }
                        );

                        if (!set) {
                            throw_exception(missing_attribute("expr"));
                        }
                    }
                    else {
                        NO_ATTRIBUTES_IN(nested);

                             if (name == text("else"))  condition = true;
                        else if (name == text("endif")) break;
                        else CHEMICAL_UNREACHABLE;
                    }
                }
            }
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
            FOREACH_ATTRIBUTE_IN(args.match,
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
            NO_ATTRIBUTES_IN(args.match);

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
            variables_("${" >> (xpressive::s1 = +xpressive::_w) >> '}'
                      |'$' >> (xpressive::s1 = +xpressive::_w)) {}

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

            FOREACH_ATTRIBUTE_IN(args.match,
                if (name == text("var")) {
                    if (name_) throw_exception(duplicate_attribute("name"));
                    else name_ = value;
                }
                else if (name == text("value")) {
                    if (value_) throw_exception(duplicate_attribute("value"));

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

#undef FOREACH_ATTRIBUTE_IN
#undef NO_ATTRIBUTES_IN

}}} // namespace chemical::synthesis::ssi

#endif // CHEMICAL_SYNTHESIS_ENGINES_SSI_DIRECTIVES_HPP_INCLUDED

