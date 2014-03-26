//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_SSI_DIRECTIVES_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_SSI_DIRECTIVES_HPP_INCLUDED

#include <string>
#include <functional>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace ssi {

using detail::text;
using detail::get_nested;
using detail::operator ==;
using namespace detail::placeholders;

//
// FOREACH_ATTRIBUTE_IN, NO_ATTRIBUTES_IN:
//     Macros to facilitate iterating over and validating directive attributes.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum { interpolated = true, raw = false };

#define FOREACH_ATTRIBUTE_IN(x, how, if_statement) do {                      \
    BOOST_FOREACH( typename Engine::match_type const& attr                   \
                 , get_nested<1>(x).nested_results()) {                      \
        typename Engine::string_type name, value;                            \
        tie(name, value) = args.engine.parse_attribute(attr, args, how);     \
        if_statement else throw_exception                                    \
            (invalid_attribute(args.engine.template transcode<char>(name))); \
    }                                                                        \
} while (0)

#define NO_ATTRIBUTES_IN(x) FOREACH_ATTRIBUTE_IN(x, raw, if (false) {})

//
// config_directive
////////////////////////////////////////////////////////////////////////////////////////////////////

struct config_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("config"));
        }

        void render(typename Engine::args_type const& args) const {
            FOREACH_ATTRIBUTE_IN(args.match, interpolated,
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
////////////////////////////////////////////////////////////////////////////////////////////////////

struct echo_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("echo"));
        }

        void render(typename Engine::args_type const& args) const {
            typename Engine::string_type encoding = text("entity");
            FOREACH_ATTRIBUTE_IN(args.match, interpolated,
                if (name == text("var")) {
                    typename Engine::string_type const result =
                        args.engine.lookup_variable(args.context, args.options, value);

                    args.stream <<
                        AJG_CASE_OF(encoding,
                            ((text("none"),   result))
                            ((text("url"),    detail::uri_encode(result)))
                            ((text("entity"), detail::escape_entities(result))));
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
////////////////////////////////////////////////////////////////////////////////////////////////////

struct exec_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("exec"));
        }

        void render(typename Engine::args_type const& args) const {
            FOREACH_ATTRIBUTE_IN(args.match, interpolated,
                if (name == text("cgi")) {
                    // TODO:
                    // BOOST_ASSERT(detail::file_exists(value));
                    throw_exception(not_implemented("exec cgi"));
                }
                else if (name == text("cmd")) {
                    std::string const command = args.engine.template transcode<char>(value);
                    detail::pipe pipe(command);
                    pipe.read_into(args.stream);
                }
            );
        }
    };
};
//
// flastmod_directive
////////////////////////////////////////////////////////////////////////////////////////////////////

struct flastmod_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("flastmod"));
        }

        void render(typename Engine::args_type const& args) const {
            FOREACH_ATTRIBUTE_IN(args.match, interpolated,
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
////////////////////////////////////////////////////////////////////////////////////////////////////

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

            FOREACH_ATTRIBUTE_IN(args.match, interpolated,
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
////////////////////////////////////////////////////////////////////////////////////////////////////

template <std::size_t MaxRegexCaptures = 9>
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
            raw_string, quoted_string,
            expression, primary_expression, not_expression,
            and_expression, or_expression, comparison_expression,
            string_expression, regex_expression, comparison_operator;

        void initialize(Engine const& engine) {
            using namespace xpressive;

            raw_string
                = /*+*/*~set[space | (set= '!', '&', '|', '$', '=',
                      '(', ')', '{', '}', '<', '>', '"', '`', '\'', '\\', '/')]
                ;
            quoted_string
                = '\'' >> *(~as_xpr('\'') | "\\'")  >> '\''
                | '`'  >> *(~as_xpr('`')  | "\\`")  >> '`'
                | '"'  >> *(~as_xpr('"')  | "\\\"") >> '"'
                ;
            regex_expression
                = '/' >> (s1 = *(~as_xpr('/') | "\\\\")) >> '/'
                ;
            string_expression // A
                = quoted_string
                | raw_string
                | engine.variable
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

        bool compare_regex( typename Engine::args_type         const& args
                          , typename Engine::string_match_type const& expr) const {
            typename Engine::string_type const
                left  = parse_string(args, get_nested<A>(expr)),
                right = get_nested<C>(expr)[xpressive::s1].str();
            typename Engine::string_match_type match;
            typename Engine::string_regex_type const pattern =
                Engine::string_regex_type::compile(right/*, xpressive::regex_constants::optimize*/);

            for (std::size_t i = 0; i <= MaxRegexCaptures; ++i) {
                args.context.erase(boost::lexical_cast<typename Engine::string_type>(i));
            }

            if (xpressive::regex_search(left, match, pattern)) {
                std::size_t const limit = (std::min)(match.size(), MaxRegexCaptures);

                for (std::size_t i = 0; i <= limit; ++i) {
                    typename Engine::string_type const key =
                        boost::lexical_cast<typename Engine::string_type>(i);
                    args.context.insert(std::make_pair(key, match[i].str()));
                }
            }

            return match;
        }

        bool compare( typename Engine::args_type         const& args
                    , typename Engine::string_match_type const& expr) const {
            typename Engine::string_type const op = expr(comparison_operator).str();

            if (get_nested<C>(expr) == regex_expression) {
                bool const matched = compare_regex(args, expr);
                std::logic_error const error("comparison operator");
                return AJG_CASE_OF_ELSE(op, ((text("="),  matched))
                                                 ((text("=="), matched))
                                                 ((text("!="), !matched)),
                                                     (throw_exception(error), 0));
            }
            else {
                typename Engine::string_type const
                    left  = parse_string(args, get_nested<A>(expr)),
                    right = parse_string(args, get_nested<C>(expr));
                return AJG_CASE_OF(op,
                    ((text("="),  left == right))
                    ((text("=="), left == right))
                    ((text("!="), left != right))
                    ((text("<"),  left <  right))
                    ((text(">"),  left >  right))
                    ((text("<="), left <= right))
                    ((text(">="), left >= right)));
            }
        }

        template <class Args, class Match, class Initial, class Functor>
        Initial fold(Args const& args, Match const& match, Initial initial, Functor const& functor) const {
            BOOST_FOREACH( typename Engine::string_match_type const& operand, match.nested_results()) {
                initial = functor(initial, evaluate_expression(args, operand));
            }

            return initial;
        }

        typename Engine::string_type parse_string( typename Engine::args_type         const& args
                                                 , typename Engine::string_match_type const& match
                                                 ) const {
                return AJG_CASE_OF(get_nested<A>(match),
                    ((raw_string,           match.str()))
                    ((regex_expression,     args.engine.extract_attribute(match)))
                    ((args.engine.variable, args.engine.interpolate(args, match.str())))
                    ((quoted_string,        args.engine.interpolate(args,
                                                args.engine.extract_attribute(match)))));
        }

        bool evaluate_expression( typename Engine::args_type         const& args
                                , typename Engine::string_match_type const& expr) const {
            return AJG_CASE_OF(expr,
                ((and_expression,        fold(args, expr, true, std::logical_and<bool>())))
                ((or_expression,         fold(args, expr, false, std::logical_or<bool>())))
                ((not_expression,        !evaluate_expression(args, get_nested<A>(expr))))
                ((primary_expression,    evaluate_expression(args, get_nested<A>(expr))))
                ((expression,            evaluate_expression(args, get_nested<A>(expr))))
                ((string_expression,     !parse_string(args, expr).empty()))
                ((comparison_expression, compare(args, expr))));
        }

        bool evaluate_directive( typename Engine::args_type  const& args
                               , typename Engine::match_type const& directive
                               ) const {
            bool has_expr = false, result = false;
            typename Engine::string_match_type match;
            typename Engine::string_type const name =
                directive[xpressive::s1].str();

            if (name == text("if")
             || name == text("elif")) {
                FOREACH_ATTRIBUTE_IN(directive, raw,
                    if (name == text("expr")) {
                        if (!has_expr) has_expr = true;
                        else throw_exception(duplicate_attribute("expr"));

                        if (xpressive::regex_match(value, match, expression)) {
                            result = evaluate_expression(args, match);
                        }
                        else {
                            throw_exception(invalid_attribute("expr"));
                        }
                    }
                );

                if (has_expr) return result;
                else throw_exception(missing_attribute("expr"));
            }
            else {
                NO_ATTRIBUTES_IN(directive);
                return AJG_CASE_OF(name, ((text("else"),  true))
                                         ((text("endif"), false)));
            }
        }

        void render(typename Engine::args_type const& args) const {
            bool condition = false;

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
                    condition = evaluate_directive(args, nested);
                }
            }
        }
    };
};

//
// include_directive
////////////////////////////////////////////////////////////////////////////////////////////////////

struct include_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("include"));
        }

        void render(typename Engine::args_type const& args) const {
            FOREACH_ATTRIBUTE_IN(args.match, interpolated,
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
////////////////////////////////////////////////////////////////////////////////////////////////////

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
                args.stream << args.engine.template transcode<char_type>(nv.first) << '='
                            << args.engine.template transcode<char_type>(nv.second) << std::endl;
            }
        }
    };
};

//
// set_directive
////////////////////////////////////////////////////////////////////////////////////////////////////

struct set_directive {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            return engine.directive(text("set"));
        }

        void render(typename Engine::args_type const& args) const {
            optional<typename Engine::string_type> name_;
            optional<typename Engine::value_type>  value_;

            FOREACH_ATTRIBUTE_IN(args.match, interpolated,
                if (name == text("var")) {
                    if (name_) throw_exception(duplicate_attribute("name"));
                    else name_ = value;
                }
                else if (name == text("value")) {
                    if (value_) throw_exception(duplicate_attribute("value"));
                    else value_ = value;
                }
            );

            if (!name_)  throw_exception(missing_attribute("name"));
            if (!value_) throw_exception(missing_attribute("value"));

            args.context[*name_] = *value_;
        }
    };
};

#undef FOREACH_ATTRIBUTE_IN
#undef NO_ATTRIBUTES_IN

}}} // namespace ajg::synth::ssi

#endif // AJG_SYNTH_ENGINES_SSI_DIRECTIVES_HPP_INCLUDED

