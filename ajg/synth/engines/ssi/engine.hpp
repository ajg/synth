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
#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/engines/exceptions.hpp>
#include <ajg/synth/engines/base_engine.hpp>
#include <ajg/synth/engines/ssi/value.hpp>
#include <ajg/synth/engines/ssi/options.hpp>
#include <ajg/synth/engines/ssi/builtin_tags.hpp>

namespace ajg {
namespace synth {
namespace ssi {

using detail::operator ==;

template < class       Environment      = detail::standard_environment
         , bool        ThrowOnErrors    = false
         , std::size_t MaxRegexCaptures = 9
         >
struct engine : base_engine {

template <class BidirectionalIterator>
struct definition : base_engine::definition<BidirectionalIterator, definition<BidirectionalIterator> > {
  public:
    // Constants:

    BOOST_STATIC_CONSTANT(bool, throw_on_errors = ThrowOnErrors);

  public:

    typedef definition                                                          this_type;
    typedef base_engine::definition<BidirectionalIterator, this_type>           base_type;

    typedef typename base_type::id_type                                         id_type;
    typedef typename base_type::boolean_type                                    boolean_type;
    typedef typename base_type::size_type                                       size_type;
    typedef typename base_type::char_type                                       char_type;
    typedef typename base_type::match_type                                      match_type;
    typedef typename base_type::regex_type                                      regex_type;
    typedef typename base_type::frame_type                                      frame_type;
    typedef typename base_type::string_type                                     string_type;
    typedef typename base_type::stream_type                                     stream_type;
    typedef typename base_type::iterator_type                                   iterator_type;
    typedef typename base_type::definition_type                                 definition_type;
    typedef typename base_type::string_regex_type                               string_regex_type;
    typedef typename base_type::string_match_type                               string_match_type;

    typedef builtin_tags<this_type>                                             builtin_tags_type;
    typedef Environment                                                         environment_type;
    typedef ssi::value<char_type>                                               value_type;
    typedef std::vector<string_type>                                            whitelist_type;
    typedef std::vector<value_type>                                             sequence_type;
    typedef std::map<string_type, value_type>                                   context_type;
    typedef options<value_type>                                                 options_type;
    typedef typename value_type::traits_type                                    traits_type;

    struct args_type {
        this_type   const& engine;
        match_type  const& match;
        context_type&      context;
        options_type&      options;
        stream_type&       stream;
    };

  public:

    definition()
        : tag_start (traits_type::literal("<!--#"))
        , tag_end   (traits_type::literal("-->"))
        , environment() {
        using namespace xpressive;
//
// common grammar
////////////////////////////////////////////////////////////////////////////////////////////////////

        name
            // @see http://www.w3.org/TR/2000/WD-xml-2e-20000814#NT-Name
            = (alpha | '_' | ':') >> *(_w | (set= '_', ':', '-', '.'))
            ;
        quoted_value
            = '"'  >> *~as_xpr('"')  >> '"'
            | '`'  >> *~as_xpr('`')  >> '`'
            | '\'' >> *~as_xpr('\'') >> '\''
            ;
        variable
            = ~after('\\') >> "${" >> (s1 = +_w) >> '}'
            | ~after('\\') >> '$' >> (s1 = +_w)
            | "\\$"
            ;
        attribute
            = name >> *_s >> '=' >> *_s >> quoted_value
            ;
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
        skipper
            = as_xpr(tag_start);
            ;

        this->initialize_grammar();
        builtin_tags_.initialize(*this);
    }

  public:

    std::pair<string_type, string_type> parse_attribute( match_type   const& attr
                                                       , args_type    const& args
                                                       , boolean_type const  interpolate
                                                       ) const {
        // TODO: value, and possibly name, need to be unencoded
        //       (html entities) before processing, in some cases.
        string_type const temp  = extract_attribute(attr(args.engine.quoted_value));
        string_type const name  = boost::algorithm::to_lower_copy(attr(this->name).str());
        string_type const value = interpolate ? this->interpolate(args, temp) : temp;
        return std::make_pair(name, value);
    }

    template <class Match>
    string_type extract_attribute(Match const& attr) const {
        string_type const string = attr.str();
        BOOST_ASSERT(string.length() >= 2);
        return string.substr(1, string.length() - 2);
    }

    string_type lookup_variable( context_type const& context
                               , options_type const& options
                               , string_type  const& name
                               ) const {
        // First, check the context.
        if (optional<value_type const&> const value = detail::find_value(name, context)) {
            return value->to_string();
        }
        // Second, check for magic variables.
        else if (name == traits_type::literal("DOCUMENT_NAME")) {
            throw_exception(not_implemented("DOCUMENT_NAME"));
        }
        else if (name == traits_type::literal("DOCUMENT_URI")) {
            throw_exception(not_implemented("DOCUMENT_URI"));
        }
        else if (name == traits_type::literal("DATE_LOCAL")) {
            return detail::format_time(options.time_format, detail::local_now());
        }
        else if (name == traits_type::literal("DATE_GMT")) {
            return detail::format_time(options.time_format, detail::utc_now());
        }
        else if (name == traits_type::literal("LAST_MODIFIED")) {
            throw_exception(not_implemented("LAST_MODIFIED"));
        }
        // Third, check the environment.
        else if(optional<typename environment_type::mapped_type const> const value =
            detail::find_mapped_value(traits_type::narrow(name), environment)) {
            return traits_type::widen(*value);
        }
        // Otherwise, use the undefined echo message.
        else {
            return options.echo_message;
        }
    }

    void render( stream_type&        stream
               , frame_type   const& frame
               , context_type const& context
               , options_type const& options) const {
        // Make a non-const copy so that #set can modify it.
        context_type copy = context;
        render_block(stream, frame, copy, options);
    }

    void render_file( stream_type&        stream
                    , string_type  const& filepath
                    , context_type const& context
                    , options_type const& options
                    ) const {
        typedef file_template<char_type, engine> file_template_type;
        std::string const filepath_ = traits_type::narrow(filepath);
        file_template_type(filepath_, options.directories).render(stream, context, options);
    }

    void render_text( stream_type&        stream
                    , match_type   const& text
                    , context_type const& context
                    , options_type const& options
                    ) const {
        stream << text.str();
    }

    void render_block( stream_type&        stream
                     , match_type   const& block
                     , context_type const& context
                     , options_type const& options
                     ) const {
        BOOST_FOREACH(match_type const& nested, block.nested_results()) {
            render_match(stream, nested, context, options);
        }
    }

    void render_tag( stream_type&        stream
                   , match_type   const& match
                   , context_type const& context
                   , options_type const& options
                   ) const
    try {
        match_type const& match_ = detail::unnest(match);
        id_type    const  id     = match_.regex_id();

        if (typename builtin_tags_type::tag_type const tag = builtin_tags_.get(id)) {
            args_type const args =
                { *this
                , match_
                , const_cast<context_type&>(context)
                , const_cast<options_type&>(options)
                , const_cast<stream_type&>(stream)
                };
            tag(args);
        }
        else {
            throw_exception(missing_tag(traits_type::narrow(traits_type::to_string(id))));
        }
    }
    catch (std::exception const&) {
        if (throw_on_errors) throw;
        /* XXX: It's unclear whether this is helpful or even allowed; plus it's distracting in unit tests.
        else {
            std::cerr << std::endl << "error (" << e.what() << ") in `" << match.str() << "`" << std::endl;
        }*/

        stream << options.error_message;
    }

    void render_match( stream_type&        stream
                     , match_type   const& match
                     , context_type const& context
                     , options_type const& options
                     ) const {
             if (match == this->text)  render_text(stream, match, context, options);
        else if (match == this->block) render_block(stream, match, context, options);
        else if (match == this->tag)   render_tag(stream, match, context, options);
        else throw_exception(std::logic_error("invalid template state"));
    }

    /// Creates a regex object to parse a full SSI tag ("directive").
    /// Meaning: tag_start name attribute* tag_end
    ///
    /// \param  name The identifier that follows the tag_start.
    /// \return A regex object that can match an entire tag.
    /// \pre    name is a valid identifier, character-wise.
    /// \post   The name is stored in s1.

    regex_type make_tag(string_type const& name) const {
        using namespace xpressive;
        return tag_start >> *_s >> (s1 = name) >> (regex_type() = *(+_s >> attribute)) >> *_s >> tag_end;
    }

    boolean_type equals_regex(args_type const& args, string_match_type const& expr) const {
        string_type       const left    = parse_string(args, get_nested<A>(expr));
        string_type       const right   = get_nested<C>(expr)[xpressive::s1].str();
        string_regex_type const pattern = string_regex_type::compile(right);

        for (std::size_t i = 0; i <= MaxRegexCaptures; ++i) {
            args.context.erase(traits_type::to_string(i));
        }

        string_match_type match;
        if (xpressive::regex_search(left, match, pattern)) {
            std::size_t const limit = (std::min)(match.size(), MaxRegexCaptures);

            for (std::size_t i = 0; i <= limit; ++i) {
                string_type const key = traits_type::to_string(i);
                args.context.insert(std::make_pair(key, match[i].str()));
            }
        }

        return match;
    }

    boolean_type equals(args_type const& args, string_match_type const& expr) const {
        string_type const op = expr(this->comparison_operator).str();

        if (get_nested<C>(expr) == this->regex_expression) {
            if (op == traits_type::literal("=")
             || op == traits_type::literal("==")) return equals_regex(args, expr);
            if (op == traits_type::literal("!=")) return !equals_regex(args, expr);
            throw_exception(std::logic_error("invalid regex operator"));
        }
        else {
            string_type const left  = parse_string(args, get_nested<A>(expr));
            string_type const right = parse_string(args, get_nested<C>(expr));
            if (op == traits_type::literal("=")
             || op == traits_type::literal("==")) return left == right;
            if (op == traits_type::literal("!=")) return left != right;
            if (op == traits_type::literal("<"))  return left <  right;
            if (op == traits_type::literal(">"))  return left >  right;
            if (op == traits_type::literal("<=")) return left <= right;
            if (op == traits_type::literal(">=")) return left >= right;
            throw_exception(std::logic_error("invalid string operator"));
        }
    }

    template <class Args, class Match, class Initial, class Functor>
    Initial fold(Args const& args, Match const& match, Initial initial, Functor const& functor) const {
        BOOST_FOREACH(string_match_type const& operand, match.nested_results()) {
            initial = functor(initial, args.engine.evaluate_expression(args, operand));
        }

        return initial;
    }

    string_type parse_string(args_type const& args, string_match_type const& match) const {
        string_match_type const& string = get_nested<A>(match);
        if (string == raw_string)           return match.str();
        if (string == regex_expression)     return args.engine.extract_attribute(match);
        if (string == args.engine.variable) return args.engine.interpolate(args, match.str());
        if (string == quoted_string)        return args.engine.interpolate(args, args.engine.extract_attribute(match));
        throw_exception(std::logic_error("invalid string"));
    }

    boolean_type evaluate_expression(args_type const& args, string_match_type const& expr) const {
        if (expr == and_expression)        return fold(args, expr, true, std::logical_and<bool>());
        if (expr == or_expression)         return fold(args, expr, false, std::logical_or<bool>());
        if (expr == not_expression)        return !evaluate_expression(args, get_nested<A>(expr));
        if (expr == primary_expression)    return evaluate_expression(args, get_nested<A>(expr));
        if (expr == expression)            return evaluate_expression(args, get_nested<A>(expr));
        if (expr == string_expression)     return !parse_string(args, expr).empty();
        if (expr == comparison_expression) return equals(args, expr);
        throw_exception(std::logic_error("invalid expression"));

    }

    string_type interpolate(args_type const& args, string_type const& string) const {
        typedef typename base_type::string_match_type match_type;
        boost::function<string_type(match_type const&)> const formatter =
            boost::bind(replace_variable, boost::cref(args), _1);
        return xpressive::regex_replace(string, variable, formatter);
    }

  private:

    static string_type replace_variable(args_type const& args,
            typename base_type::string_match_type const& match) {
        return match.str() == traits_type::literal("\\$") ? traits_type::literal("$") :
            args.engine.lookup_variable(args.context, args.options, match[xpressive::s1].str());
    }

  public:

    string_type const tag_start;
    string_type const tag_end;

  public:

    environment_type const environment;
    options_type     const default_options;

  private:

    template <class E> friend struct ssi::builtin_tags;
    friend struct base_engine;

    regex_type tag, text, block, skipper;
    regex_type name, attribute, quoted_value;

    string_regex_type variable;
    string_regex_type raw_string, quoted_string;
    string_regex_type expression, primary_expression, not_expression;
    string_regex_type and_expression, or_expression, comparison_expression;
    string_regex_type string_expression, regex_expression, comparison_operator;

  private:

    whitelist_type    whitelist_;
    builtin_tags_type builtin_tags_;

}; // definition

}; // engine

}}} // namespace ajg::synth::ssi

#endif // AJG_SYNTH_ENGINES_SSI_ENGINE_HPP_INCLUDED
