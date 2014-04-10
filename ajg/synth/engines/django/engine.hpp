//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_ENGINE_HPP_INCLUDED

#include <ajg/synth/config.hpp>

#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <numeric>
#include <algorithm>

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/tokenizer.hpp>
#include <boost/noncopyable.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include <ajg/synth/templates.hpp>
#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/engines/exceptions.hpp>
#include <ajg/synth/engines/base_engine.hpp>
#include <ajg/synth/engines/django/value.hpp>
#include <ajg/synth/engines/django/loader.hpp>
#include <ajg/synth/engines/django/options.hpp>
#include <ajg/synth/engines/django/builtin_tags.hpp>
#include <ajg/synth/engines/django/builtin_filters.hpp>

namespace ajg {
namespace synth {
namespace django {
namespace {
using detail::operator ==;
using detail::find_mapped_value;
namespace x = boost::xpressive;
} // namespace

struct engine : base_engine {

template <class BidirectionalIterator>
struct definition : base_engine::definition<BidirectionalIterator, definition<BidirectionalIterator> > {
  public:

    typedef definition                                                          this_type;
    typedef base_engine::definition<BidirectionalIterator, this_type>           base_type;
    typedef django::loader<this_type>                                           loader_type;
    typedef typename base_type::id_type                                         id_type;
    typedef typename base_type::size_type                                       size_type;
    typedef typename base_type::char_type                                       char_type;
    typedef typename base_type::match_type                                      match_type;
    typedef typename base_type::regex_type                                      regex_type;
    typedef typename base_type::frame_type                                      frame_type;
    typedef typename base_type::string_type                                     string_type;
    typedef typename base_type::stream_type                                     stream_type;
    typedef typename base_type::symbols_type                                    symbols_type;
    typedef typename base_type::iterator_type                                   iterator_type;
    typedef typename base_type::definition_type                                 definition_type;
    typedef typename base_type::string_regex_type                               string_regex_type;

    typedef builtin_tags<this_type>                                             builtin_tags_type;
    typedef builtin_filters<this_type>                                          builtin_filters_type;
    typedef django::value<char_type>                                            value_type;
    typedef options<value_type>                                                 options_type;
    typedef typename value_type::traits_type                                    traits_type;
    typedef typename value_type::none_type                                      none_type;
    typedef typename value_type::boolean_type                                   boolean_type;
    typedef typename value_type::datetime_type                                  datetime_type;
    typedef typename value_type::duration_type                                  duration_type;
    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::names_type                                   names_type;
    typedef typename options_type::sequence_type                                sequence_type;
    typedef typename options_type::arguments_type                               arguments_type;

  private:

    symbols_type keywords_, reserved_;

    struct not_in {
        symbols_type const& symbols;
        explicit not_in(symbols_type const& symbols) : symbols(symbols) {}

        bool operator ()(typename match_type::value_type const& match) const {
            return this->symbols.find(match.str()) == this->symbols.end();
        }
    };

  public:

    std::map<string_type, string_type> markers; // TODO[c++11]: unordered_map.

    inline regex_type marker(string_type const& s, string_type const& name) {
        return x::as_xpr((this->markers[name] = s));
    }

    inline regex_type word    (string_type const s) { return x::as_xpr(s) >> x::_b; }
    inline regex_type word    (char const* const s) { return x::as_xpr(s) >> x::_b; }
    inline regex_type op      (char const* const s) { return this->word(*this->keywords_.insert(traits_type::literal(s)).first); }
    inline regex_type keyword (char const* const s) { return this->word(*this->keywords_.insert(traits_type::literal(s)).first) >> *x::_s; }
    inline regex_type reserved(char const* const s) { return this->word(*this->reserved_.insert(traits_type::literal(s)).first) >> *x::_s; }

    definition()
        : newline        (traits_type::literal("\n"))
        , ellipsis       (traits_type::literal("..."))
        , brace_open     (marker(traits_type::literal("{"),  traits_type::literal("openbrace")))
        , brace_close    (marker(traits_type::literal("}"),  traits_type::literal("closebrace")))
        , block_open     (marker(traits_type::literal("{%"), traits_type::literal("openblock")))
        , block_close    (marker(traits_type::literal("%}"), traits_type::literal("closeblock")))
        , comment_open   (marker(traits_type::literal("{#"), traits_type::literal("opencomment")))
        , comment_close  (marker(traits_type::literal("#}"), traits_type::literal("closecomment")))
        , variable_open  (marker(traits_type::literal("{{"), traits_type::literal("openvariable")))
        , variable_close (marker(traits_type::literal("}}"), traits_type::literal("closevariable"))) {
        using namespace xpressive;
//
// common grammar
////////////////////////////////////////////////////////////////////////////////////////////////////

        identifier
            = ((alpha | '_') >> *_w >> _b)
            ;
        restricted_identifier
            = identifier[ check(not_in(keywords_)) ]
            ;
        unreserved_identifier
            = restricted_identifier[ check(not_in(reserved_)) ]
            ;
        name
            = (id = restricted_identifier) >> *_s
            ;
        names
            = +name
            ;
        unreserved_name
            = (id = unreserved_identifier) >> *_s
            ;
        package
            = (id = (restricted_identifier >> *('.' >> identifier))) >> *_s
            ;
        packages
            = +package
            ;
        none_literal
            = word("None")
            ;
        true_literal
            = word("True")
            ;
        false_literal
            = word("False")
            ;
        boolean_literal
            = true_literal
            | false_literal
            ;
        number_literal
            = !(set= '-','+') >> +_d // Integral part.
                >> !('.' >> +_d)     // Floating part.
                >> !('e' >> +_d)     // Exponent part.
            ;
        string_literal
            = '"'  >> *~as_xpr('"')  >> '"'
            | '\'' >> *~as_xpr('\'') >> '\''
            ;
        variable_literal
            = restricted_identifier
            ;
        literal
            = none_literal
            | boolean_literal
            | number_literal
            | string_literal
            | variable_literal
            ;
        attribute_link
            = '.' >> identifier
            ;
        subscript_link
            = as_xpr('[') >> *_s >> x::ref(expression) >> *_s >> ']'
            ;
        link
            = attribute_link
            | subscript_link
            ;
        chain
            = literal >> *link
            ;
        unary_operator
            = op("not")
            ;
        binary_operator
            = op("==")
            | op("!=")
            | op("<=")
            | op(">=")
            | op("<")
            | op(">")
            | op("and")
            | op("or")
            | op("in")
            | op("not") >> *_s >> op("in")
            ;
        binary_expression
            = chain >> *(*_s >> binary_operator >> *_s >> x::ref(expression))
            ;
        unary_expression
            = unary_operator >> *_s >> x::ref(expression)
            ;
        nested_expression
            = as_xpr('(') >> *_s >> x::ref(expression) >> *_s >> ')'
            ;
        expression
            = unary_expression
            | binary_expression
            | nested_expression
            ;
        variable_names
            // TODO: Check whether whitespace can precede or follow ','.
            = name >> *(as_xpr(',') >> *_s >> name)
            ;
        filter
            = name >> !(as_xpr(':') >> chain)
            ;
        filters
            = filter >> *(as_xpr('|') >> filter)
            ;
        value
            = expression >> *(as_xpr('|') >> filter) >> *_s
            ;
        values
            = +value
            ;
        argument
            // TODO: Check whether whitespace can precede or follow '='.
            = !(restricted_identifier >> as_xpr('=')) >> value
            ;
        arguments
            = *argument
            ;
        skipper
            = block_open
            | block_close
            | comment_open
            | comment_close
            | variable_open
            | variable_close
            ;
        nothing
            = as_xpr('\0') // Xpressive barfs when default-constructed.
            ;
        html_namechar
            = ~(set = ' ', '\t', '\n', '\v', '\f', '\r', '>')
            ;
        html_whitespace
            = (set = ' ', '\t', '\n', '\v', '\f', '\r')
            ;
        html_tag
            = '<' >> !as_xpr('/')
                   // The tag's name:
                   >> (s1 = -+html_namechar)
                   // Attributes, if any:
                   >> !(+html_whitespace >> -*~as_xpr('>'))
               >> !as_xpr('/')
            >> '>'
            ;

        this->initialize_grammar();
        builtin_tags_.initialize(*this);
    }

  public:

    template <char_type Delimiter>
    sequence_type split_argument( value_type   const& argument
                                , context_type const& context
                                , options_type const& options
                                ) const {
        typedef boost::char_separator<char_type>                                separator_type;
        typedef typename value_type::token_type                                 token_type;
        typedef typename token_type::const_iterator                             iterator_type;
        typedef boost::tokenizer<separator_type, iterator_type, token_type>     tokenizer_type;
        typedef definition<iterator_type>                                       definition_type;

        BOOST_ASSERT(argument.is_literal());
        token_type const& source = argument.token();
        static char_type const delimiter[2] = { Delimiter, 0 };
        tokenizer_type const tokenizer(source, separator_type(delimiter, 0, keep_empty_tokens));
        static definition_type const tokenizable_definition;
        typename definition_type::match_type match;
        sequence_type sequence;

        BOOST_FOREACH(token_type const& token, tokenizer) {
            if (std::distance(token.begin(), token.end()) == 0) {
                sequence.push_back(value_type(none_type()));
            }
            else if (xpressive::regex_match(token.begin(), token.end(), match, tokenizable_definition.chain)) {
                try {
                    sequence.push_back(tokenizable_definition.evaluate_chain(match, context, options));
                }
                catch (missing_variable const& e) {
                    string_type const string(token.begin(), token.end());

                    if (traits_type::narrow(string) != e.name) {
                        throw_exception(e);
                    }

                    // A missing variable means an embedded argument was meant as a string literal.
                    value_type value = string;
                    value.token(match[0]);
                    sequence.push_back(value);
                }
            }
        }

        return sequence;
    }

    string_type extract_string(match_type const& match) const {
        BOOST_ASSERT(match == this->string_literal);
        return detail::unquote(match.str());
    }

    names_type extract_names(match_type const& match) const {
        names_type names;
        BOOST_FOREACH(match_type const& name, detail::select_nested(match, this->name)) {
            names.push_back(name[id].str());
        }
        return names;
    }

    void render( stream_type&        stream
               , frame_type   const& frame
               , context_type const& context
               , options_type const& options) const {
        render_block(stream, frame, context, options);
    }

    void render_file( stream_type&        stream
                    , string_type  const& filepath
                    , context_type const& context
                    , options_type const& options
                    ) const {
        typedef file_template<char_type, engine> file_template_type;
        file_template_type(filepath, options.directories).render(stream, context, options);
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
                   ) const {
        match_type const& match_ = detail::unnest(match);
        id_type    const  id     = match_.regex_id();

        if (typename builtin_tags_type::tag_type const tag = builtin_tags_.get(id)) {
            tag(*this, match_, context, options, stream);
        }
        else {
            throw_exception(missing_tag(traits_type::narrow(traits_type::to_string(id))));
        }
    }

    void render_match( stream_type&        stream
                     , match_type   const& match
                     , context_type const& context
                     , options_type const& options
                     ) const {
             if (match == text)  render_text(stream, match, context, options);
        else if (match == block) render_block(stream, match, context, options);
        else if (match == tag)   render_tag(stream, match, context, options);
        else throw_exception(std::logic_error("invalid template state"));
    }

    value_type apply_filters( value_type   const& value
                            , match_type   const& match
                            , context_type const& context
                            , options_type const& options
                            ) const {
        value_type result = value;

        BOOST_FOREACH(match_type const& filter, detail::select_nested(match, this->filter)) {
            BOOST_ASSERT(filter == this->filter);
            string_type const& name  = filter(this->name)[id].str();
            match_type  const& chain = filter(this->chain);

            arguments_type arguments;
            if (chain) {
                arguments.first.push_back(this->evaluate_chain(chain, context, options));
            }
            result = this->apply_filter(result, name, arguments, context, options);
        }

        return result;
    }

    value_type apply_filter( value_type     const& value
                           , string_type    const& name
                           , arguments_type const& arguments
                           , context_type   const& context
                           , options_type   const& options
                           ) const {
        typename options_type::filters_type::const_iterator it = options.loaded_filters.find(name);

        if (it != options.loaded_filters.end()) { // Let library filters override built-in ones.
            return it->second(options, context, value, arguments);
        }

        if (typename builtin_filters_type::filter_type const filter = builtin_filters_type::get(name)) {
            return filter(*this, value, arguments, context, options);
        }
        else {
            throw_exception(missing_filter(traits_type::narrow(name)));
        }
    }

    value_type evaluate( match_type   const& match
                       , context_type const& context
                       , options_type const& options
                       ) const {

        try {
            value_type const& value = this->evaluate_expression(match(this->expression), context, options);
            return this->apply_filters(value, match, context, options);
        }
        catch (missing_variable  const&) { return options.default_value; }
        catch (missing_attribute const&) { return options.default_value; }
    }

    arguments_type evaluate_arguments( match_type    const& match
                                     , context_type  const& context
                                     , options_type  const& options
                                     ) const {
        arguments_type arguments;
        BOOST_FOREACH(match_type const& arg, detail::select_nested(match, this->argument)) {
            value_type const& value = this->evaluate(arg(this->value), context, options);
            if (match_type const& name = arg(this->restricted_identifier)) {
                arguments.second[name.str()] = value; // Keyword argument.
            }
            else {
                arguments.first.push_back(value); // Positional argument.
            }
        }
        return arguments;
    }

    value_type evaluate_literal( match_type   const& match
                               , context_type const& context
                               , options_type const& options
                               ) const {
        BOOST_ASSERT(match == this->literal);
        match_type  const& literal = detail::unnest(match);
        string_type const  string  = match.str();

        if (literal == none_literal) {
            return value_type(none_type()).token(literal[0]);
        }
        else if (literal == boolean_literal) {
            match_type const& boolean = detail::unnest(literal);

            if (boolean == true_literal) {
                return value_type(boolean_type(true)).token(literal[0]);
            }
            else if (boolean == false_literal) {
                return value_type(boolean_type(false)).token(literal[0]);
            }
            else {
                throw_exception(std::logic_error("invalid boolean literal"));
            }
        }
        else if (literal == number_literal) {
            return value_type(traits_type::to_number(string)).token(literal[0]);
        }
        else if (literal == string_literal) {
            // Adjust the token by trimming the quotes.
            string_type const token = string_type(literal[0].first + 1, literal[0].second - 1);
            return value_type(extract_string(literal)).token(token);
        }
        else if (literal == variable_literal) {
            if (optional<value_type const&> const variable = detail::find_value(string, context)) {
                return variable->copy().token(literal[0]);
            }
            else {
                throw_exception(missing_variable(traits_type::narrow(string)));
            }
        }
        else {
            throw_exception(std::logic_error("invalid literal"));
        }
    }

    value_type evaluate_expression( match_type   const& match
                                  , context_type const& context
                                  , options_type const& options
                                  ) const {
        match_type const& expr = detail::unnest(match);

        if (expr == unary_expression) {
            return this->evaluate_unary(expr, context, options);
        }
        else if (expr == binary_expression) {
            return this->evaluate_binary(expr, context, options);
        }
        else if (expr == nested_expression) {
            match_type const& nested = expr(this->expression);
            return this->evaluate_expression(nested, context, options);
        }
        else {
            throw_exception(std::logic_error("invalid expression"));
        }
    }

    value_type evaluate_unary( match_type   const& match
                             , context_type const& context
                             , options_type const& options
                             ) const {
        BOOST_ASSERT(match == unary_expression);
        string_type const& op      = match(unary_operator).str();
        match_type  const& operand = match(expression);

        if (op == traits_type::literal("not")) {
            return !evaluate_expression(operand, context, options);
        }
        else {
            throw_exception(std::logic_error("invalid unary operator"));
        }
    }

    value_type evaluate_binary( match_type   const& match
                              , context_type const& context
                              , options_type const& options
                              ) const {
        BOOST_ASSERT(match == binary_expression);
        match_type const& chain = match(this->chain);
        value_type value = this->evaluate_chain(chain, context, options);
        string_type op;

        BOOST_FOREACH(match_type const& segment, detail::drop(match.nested_results(), 1)) {
            if (segment == binary_operator) {
                op = segment.str();
            }
            else if (!(segment == expression)) {
                throw_exception(std::logic_error("invalid binary expression"));
            }
            else if (op == traits_type::literal("==")) {
                value = value == this->evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal("!=")) {
                value = value != this->evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal("<")) {
                value = value < this->evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal(">")) {
                value = value > this->evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal("<=")) {
                value = value <= this->evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal(">=")) {
                value = value >= this->evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal("and")) {
                value = value ? this->evaluate_expression(segment, context, options) : value;
            }
            else if (op == traits_type::literal("or")) {
                value = value ? value : this->evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal("in")) {
                value_type const elements = this->evaluate_expression(segment, context, options);
                value = elements.contains(value);
            }
            else if (boost::algorithm::starts_with(op, traits_type::literal("not"))
                  && boost::algorithm::ends_with(op, traits_type::literal("in"))) {
                value_type const elements = this->evaluate_expression(segment, context, options);
                value = !elements.contains(value);
            }
            else {
                throw_exception(std::logic_error("invalid binary operator"));
            }
        }

        return value;
    }

    value_type evaluate_link( match_type   const& match
                            , context_type const& context
                            , options_type const& options
                            ) const {
        match_type const& link = detail::unnest(match);

        if (link == this->subscript_link) { // i.e. value[attribute]
            return this->evaluate(link(this->expression), context, options);
        }
        else if (link == this->attribute_link) { // i.e. value.attribute
            return string_type(link(this->identifier).str());
        }
        else {
            throw_exception(std::logic_error("invalid link"));
        }
    }

    value_type evaluate_chain( match_type   const& match
                             , context_type const& context
                             , options_type const& options
                             ) const {
        BOOST_ASSERT(match == this->chain);
        match_type const& lit = match(this->literal);
        value_type value = this->evaluate_literal(lit, context, options);

        BOOST_FOREACH(match_type const& link, detail::select_nested(match, this->link)) {
            value_type const attribute = this->evaluate_link(link, context, options);
            value = value.must_get_attribute(attribute);
        }

        return value;
    }

    optional<string_type> get_view_url( value_type     const& view
                                      , arguments_type const& arguments
                                      , context_type   const& context
                                      , options_type   const& options
                                      ) const {
        string_type name = view.to_string();

        BOOST_FOREACH(typename options_type::resolver_type const& resolver, options.resolvers) {
            if (optional<string_type> const& url = resolver->reverse(name, arguments, context, options)) {
                return url;
            }
        }
        return none;
    }

    void load_library( context_type&      context
                     , options_type&      options
                     , string_type const& library
                     , names_type  const* names   = 0
                     ) const {
        loader_type::load(context, options, library, names);
    }

  public:

    string_type const newline;
    string_type const ellipsis;

    regex_type const brace_open;
    regex_type const brace_close;
    regex_type const block_open;
    regex_type const block_close;
    regex_type const comment_open;
    regex_type const comment_close;
    regex_type const variable_open;
    regex_type const variable_close;

  /*private:

    template <class E> friend struct django::builtin_tags;
    template <class E> friend struct django::builtin_filters;
    friend base_engine; */

    regex_type tag;
    regex_type text;
    regex_type block;
    regex_type skipper;
    regex_type nothing;
    regex_type identifier;
    regex_type restricted_identifier;
    regex_type unreserved_identifier;
    regex_type unreserved_name;
    regex_type name;
    regex_type names;
    regex_type variable_names;
    regex_type package;
    regex_type packages;
    regex_type argument;
    regex_type arguments;
    regex_type value;
    regex_type values;
    regex_type filter;
    regex_type filters;
    regex_type chain;
    regex_type link;
    regex_type subscript_link;
    regex_type attribute_link;
    regex_type unary_operator;
    regex_type binary_operator;
    regex_type unary_expression;
    regex_type binary_expression;
    regex_type nested_expression;
    regex_type expression;
    regex_type none_literal;
    regex_type true_literal;
    regex_type false_literal;
    regex_type boolean_literal;
    regex_type string_literal;
    regex_type number_literal;
    regex_type variable_literal;
    regex_type literal;

    string_regex_type html_namechar;
    string_regex_type html_whitespace;
    string_regex_type html_tag;

  private:

    builtin_tags_type builtin_tags_;

}; // definition

}; // engine

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_ENGINE_HPP_INCLUDED
