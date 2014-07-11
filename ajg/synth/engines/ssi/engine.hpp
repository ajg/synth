//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_SSI_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_SSI_ENGINE_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <numeric>
#include <cstdlib>
#include <utility>
#include <algorithm>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>

#include <ajg/synth/templates.hpp>
#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/detail/find.hpp>
#include <ajg/synth/detail/standard_environment.hpp>
#include <ajg/synth/engines/base_engine.hpp>
#include <ajg/synth/engines/ssi/builtin_tags.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace ssi {

template <class Traits>
struct engine : base_engine<Traits> {
  public:

    typedef engine                                                              engine_type;
    typedef Traits                                                              traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef typename engine_type::value_type                                    value_type;
    typedef typename engine_type::options_type                                  options_type;

    typedef typename options_type::context_type                                 context_type;

    typedef detail::standard_environment                                        environment_type;
    typedef std::vector<string_type>                                            whitelist_type;

  public:

    BOOST_STATIC_CONSTANT(boolean_type, throw_on_errors    = false);
    BOOST_STATIC_CONSTANT(size_type,    max_regex_captures = 9);

  public:

    template <class Iterator>
    struct kernel;

  public:

    inline static char const* name() { return "ssi"; }

  private:

    template <class K> friend struct ssi::builtin_tags;

}; // engine

template <class Traits>
template <class Iterator>
struct engine<Traits>::kernel : base_engine<Traits>::AJG_SYNTH_TEMPLATE base_kernel<Iterator> {
  public:

    typedef kernel                                                              kernel_type;
    typedef Iterator                                                            iterator_type;
    typedef engine                                                              engine_type;
    typedef typename kernel_type::state_type                                    state_type;

  protected:

    typedef builtin_tags<kernel_type>                                           builtin_tags_type;
    typedef typename kernel_type::id_type                                       id_type;
    typedef typename kernel_type::regex_type                                    regex_type;
    typedef typename kernel_type::match_type                                    match_type;
    typedef typename kernel_type::string_regex_type                             string_regex_type;
    typedef typename kernel_type::string_match_type                             string_match_type;
    typedef detail::text<string_type>                                           text;

  private:

    template <class K> friend struct ssi::builtin_tags;

    // TODO: Rename to parameters_type or such to avoid confusion with other kinds of arguments.
    struct args_type {
        kernel_type   const& kernel;
        match_type    const& match;
        options_type  const& options;
        context_type&        context;
        ostream_type&        ostream;
    };

  public:

    kernel()
        : tag_start (text::literal("<!--#"))
        , tag_end   (text::literal("-->"))
        , environment()
        {
//
// common grammar
////////////////////////////////////////////////////////////////////////////////////////////////////

        name
            // @see http://www.w3.org/TR/2000/WD-xml-2e-20000814#NT-Name
            = (x::alpha | '_' | ':') >> *(_w | (x::set = '_', ':', '-', '.'))
            ;
        quoted_value
            = '"'  >> *~as_xpr('"')  >> '"'
            | '`'  >> *~as_xpr('`')  >> '`'
            | '\'' >> *~as_xpr('\'') >> '\''
            ;
        variable
            = ~x::after('\\') >> "${" >> (s1 = +_w) >> '}'
            | ~x::after('\\') >> '$' >> (s1 = +_w)
            | "\\$"
            ;
        attribute
            = name >> *_s >> '=' >> *_s >> quoted_value
            ;
        raw_string
            = /*+*/*~x::set
                [ x::space
                | (x::set = '!', '&', '|', '$', '=', '(', ')', '{', '}', '<', '>', '"', '`', '\'', '\\', '/')
                ]
            ;
        quoted_string
            = '\'' >> *(~as_xpr('\'') | "\\'")  >> '\''
            | '`'  >> *(~as_xpr('`')  | "\\`")  >> '`'
            | '"'  >> *(~as_xpr('"')  | "\\\"") >> '"'
            ;
        regex_expression
            = '/' >> (s1 = *(~as_xpr('/') | "\\\\")) >> '/'
            ;
        string_expression
            = quoted_string
            | raw_string
            | variable
            ;
        expression
            = x::ref(and_expression)
            | x::ref(or_expression)
            ;
        primary_expression
            = '(' >> *_s >> expression >> *_s >> ')'
            | x::ref(comparison_expression)
            | x::ref(string_expression)
            | x::ref(not_expression)
            ;
        not_expression
            = '!' >> *_s >> expression
            ;
        comparison_operator
            = as_xpr("=") | "==" | "!=" | "<" | ">" | "<=" | ">="
            ;
        comparison_expression
            = string_expression >> *_s >> comparison_operator >> *_s >> (string_expression | regex_expression)
            ;
        and_expression
            = primary_expression >> *(*_s >> "&&" >> *_s >> expression)
            ;
        or_expression
            = primary_expression >> *(*_s >> "||" >> *_s >> expression)
            ;
        this->skipper
            = as_xpr(tag_start);
            ;

        this->initialize_grammar();
        builtin_tags_.initialize(*this);
    }

  public: // TODO: Make protected, and make builtin_tags/builtin_filters friends.

    std::pair<string_type, string_type> parse_attribute( match_type   const& attr
                                                       , args_type    const& args
                                                       , boolean_type const  interpolate
                                                       ) const {
        // TODO: value, and possibly name, need to be unencoded
        //       (html entities) before processing, in some cases.
        string_type const temp  = extract_attribute(attr(this->quoted_value));
        string_type const name  = text::lower(attr(this->name).str());
        string_type const value = interpolate ? this->interpolate(args, temp) : temp;
        return std::make_pair(name, value);
    }

    template <class Match>
    string_type extract_attribute(Match const& attr) const {
        string_type const string = attr.str();
        BOOST_ASSERT(string.length() >= 2);
        return string.substr(1, string.length() - 2);
    }

    string_type lookup_variable( context_type&       context
                               , options_type const& options
                               , string_type  const& name
                               ) const {
        string_type const time_format = context.format(text::literal("timefmt"));

        // First, check the context.
        if (boost::optional<value_type> const value = context.get(name)) {
            return string_type(value->to_string());
        }
        // Second, check for magic variables.
        else if (name == text::literal("DOCUMENT_NAME")) {
            AJG_SYNTH_THROW(not_implemented("DOCUMENT_NAME"));
        }
        else if (name == text::literal("DOCUMENT_URI")) {
            AJG_SYNTH_THROW(not_implemented("DOCUMENT_URI"));
        }
        else if (name == text::literal("DATE_LOCAL")) {
            return traits_type::format_datetime(time_format, traits_type::local_datetime(context.timezone()));
        }
        else if (name == text::literal("DATE_GMT")) {
            // return traits_type::format_time(time_format, traits_type::utc_time());
            return traits_type::format_datetime(time_format, traits_type::utc_datetime());
        }
        else if (name == text::literal("LAST_MODIFIED")) {
            AJG_SYNTH_THROW(not_implemented("LAST_MODIFIED"));
        }
        // Third, check the environment.
        else if (boost::optional<typename environment_type::mapped_type> const variable =
                    detail::find(text::narrow(name), this->environment)) {
            return text::widen(*variable);
        }
        // Otherwise, use the undefined echo message.
        else {
            return context.format(text::literal("echomsg"));
        }
    }

    void render( ostream_type&       ostream
               , options_type const& options
               , state_type   const& state
               , context_type&       context
               ) const {
        static char const* errormsg = "[an error occurred while processing this directive]";

        context.format(text::literal("echomsg"),  text::literal("(none)"),                   false);
        context.format(text::literal("errormsg"), text::literal(errormsg),                   false);
        context.format(text::literal("sizefmt"),  text::literal("bytes"),                    false);
        context.format(text::literal("timefmt"),  text::literal("%A, %d-%b-%Y %H:%M:%S %Z"), false);

        this->render_block(ostream, state.match, context, options);
    }

    void render_path( ostream_type&       ostream
                    , path_type    const& path
                    , context_type&       context
                    , options_type const& options
                    ) const {
        options.template render_path_to_stream<engine_type>(path, ostream, context);
    }

    void render_plain( ostream_type&       ostream
                     , match_type   const& plain
                     , context_type&       context
                     , options_type const& options
                     ) const {
        ostream << plain.str();
    }

    void render_block( ostream_type&       ostream
                     , match_type   const& block
                     , context_type&       context
                     , options_type const& options
                     ) const {
        BOOST_FOREACH(match_type const& nested, block.nested_results()) {
            this->render_match(ostream, nested, context, options);
        }
    }

    void render_tag( ostream_type&       ostream
                   , match_type   const& match
                   , context_type&       context
                   , options_type const& options
                   ) const
    try {
        match_type const& match_ = this->unnest(match);
        id_type    const  id     = match_.regex_id();

        if (typename builtin_tags_type::tag_type const tag = builtin_tags_.get(id)) {
            args_type const args =
                { *this
                , match_
                , options
                , context
                , ostream
                };
            tag(args);
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("missing built-in tag"));
        }
    }
    catch (std::exception const&) {
        if (throw_on_errors) throw;
        /* XXX: It's unclear whether this is helpful or even allowed; plus it's distracting in unit tests.
        else {
            std::cerr << std::endl << "error (" << e.what() << ") in `" << match.str() << "`" << std::endl;
        }*/

        ostream << context.format(text::literal("errormsg"));
    }

    void render_match( ostream_type&       ostream
                     , match_type   const& match
                     , context_type&       context
                     , options_type const& options
                     ) const {
             if (this->is(match, this->plain)) this->render_plain(ostream, match, context, options);
        else if (this->is(match, this->block)) this->render_block(ostream, match, context, options);
        else if (this->is(match, this->tag))   this->render_tag(ostream, match, context, options);
        else AJG_SYNTH_THROW(std::logic_error("invalid template state"));
    }

///
/// make_tag:
///     Creates a regex for a full SSI tag ("directive"); i.e. tag_start name attribute* tag_end.
////////////////////////////////////////////////////////////////////////////////////////////////////

    regex_type make_tag(string_type const& name) const {
        return this->tag_start >> *_s >> (s1 = name) >> *(+_s >> this->attribute) >> *_s >> this->tag_end;
    }

    boolean_type equals_regex(args_type const& args, string_match_type const& str, string_match_type const& regex) const {
        string_type       const left    = parse_string(args, str);
        string_type       const right   = regex[s1].str();
        string_regex_type const pattern = string_regex_type::compile(right);
        size_type         const n       = max_regex_captures;

        for (std::size_t i = 0; i <= n; ++i) {
            args.context.unset(text::stringize(i));
        }

        string_match_type match;
        if (x::regex_search(left, match, pattern)) {
            std::size_t const limit = (std::min)(match.size(), n);

            for (std::size_t i = 0; i <= limit; ++i) {
                string_type const key = text::stringize(i);
                args.context.set(key, match[i].str());
            }
        }

        return match;
    }

    boolean_type equals(args_type const& args, string_match_type const& expr) const {
        string_type const op = expr(this->comparison_operator).str();

        if (string_match_type const& regex = expr(this->regex_expression)) {
            string_match_type const& left  = expr(this->string_expression);

            if (op == text::literal("=")
             || op == text::literal("==")) return equals_regex(args, left, regex);
            if (op == text::literal("!=")) return !equals_regex(args, left, regex);
            AJG_SYNTH_THROW(std::logic_error("invalid regex operator"));
        }
        else {
            string_type const left  = parse_string(args, expr(this->string_expression, 0));
            string_type const right = parse_string(args, expr(this->string_expression, 1));
            if (op == text::literal("=")
             || op == text::literal("==")) return left == right;
            if (op == text::literal("!=")) return left != right;
            if (op == text::literal("<"))  return left <  right;
            if (op == text::literal(">"))  return left >  right;
            if (op == text::literal("<=")) return left <= right;
            if (op == text::literal(">=")) return left >= right;
            AJG_SYNTH_THROW(std::logic_error("invalid string operator"));
        }
    }

    template <class Args, class Match, class Initial, class Functor>
    Initial fold(Args const& args, Match const& match, Initial initial, Functor const& functor) const {
        BOOST_FOREACH(string_match_type const& operand, match.nested_results()) {
            initial = functor(initial, this->evaluate_expression(args, operand));
        }

        return initial;
    }

    string_type parse_string(args_type const& args, string_match_type const& match) const {
        string_match_type const& string = this->unnest_(match);
        if (this->is_(string, this->raw_string))           return match.str();
        if (this->is_(string, this->regex_expression))     return this->extract_attribute(match);
        if (this->is_(string, this->variable))             return this->interpolate(args, match.str());
        if (this->is_(string, this->quoted_string))        return this->interpolate(args, this->extract_attribute(match));
        AJG_SYNTH_THROW(std::logic_error("invalid string"));
    }

    boolean_type evaluate_expression(args_type const& args, string_match_type const& expr) const {
        if (this->is_(expr, this->and_expression))        return fold(args, expr, boolean_type(true), std::logical_and<boolean_type>());
        if (this->is_(expr, this->or_expression))         return fold(args, expr, boolean_type(false), std::logical_or<boolean_type>());
        if (this->is_(expr, this->not_expression))        return !evaluate_expression(args, this->unnest_(expr));
        if (this->is_(expr, this->primary_expression))    return evaluate_expression(args, this->unnest_(expr));
        if (this->is_(expr, this->expression))            return evaluate_expression(args, this->unnest_(expr));
        if (this->is_(expr, this->string_expression))     return !parse_string(args, expr).empty();
        if (this->is_(expr, this->comparison_expression)) return equals(args, expr);
        AJG_SYNTH_THROW(std::logic_error("invalid expression"));

    }

    string_type interpolate(args_type const& args, string_type const& string) const {
        boost::function<string_type(string_match_type const&)> const formatter =
            boost::bind(replace_variable, boost::cref(args), _1);
        return x::regex_replace(string, variable, formatter);
    }

  private:

    static string_type replace_variable(args_type const& args, string_match_type const& match) {
        return match.str() == text::literal("\\$") ? text::literal("$") :
            args.kernel.lookup_variable(args.context, args.options, match[s1].str());
    }

  public:

    // TODO: Move these out of the kernel.
    string_type const tag_start;
    string_type const tag_end;

    environment_type const environment;

  private:

    regex_type name;
    regex_type attribute;
    regex_type quoted_value;

    string_regex_type variable;
    string_regex_type raw_string;
    string_regex_type quoted_string;
    string_regex_type expression;
    string_regex_type primary_expression;
    string_regex_type not_expression;
    string_regex_type and_expression;
    string_regex_type or_expression;
    string_regex_type comparison_expression;
    string_regex_type string_expression;
    string_regex_type regex_expression;
    string_regex_type comparison_operator;

  private:

    whitelist_type    whitelist_;
    builtin_tags_type builtin_tags_;

}; // kernel

}}}} // namespace ajg::synth::engines::ssi

#endif // AJG_SYNTH_ENGINES_SSI_ENGINE_HPP_INCLUDED
