//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_ENGINE_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <cstdlib>
#include <numeric>
#include <algorithm>

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/tokenizer.hpp>
#include <boost/noncopyable.hpp>

#include <ajg/synth/templates.hpp>
#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/detail/drop.hpp>
#include <ajg/synth/detail/text.hpp>
#include <ajg/synth/engines/base_engine.hpp>
#include <ajg/synth/engines/django/builtin_tags.hpp>
#include <ajg/synth/engines/django/builtin_filters.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace django {

template <class Traits>
struct engine : base_engine<Traits> {
  public:

    typedef engine                                                              engine_type;
    typedef Traits                                                              traits_type;

    typedef typename traits_type::none_type                                     none_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::integer_type                                  integer_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::url_type                                      url_type;
    typedef typename traits_type::symbols_type                                  symbols_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef typename engine_type::value_type                                    value_type;
    typedef typename engine_type::options_type                                  options_type;

    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::names_type                                   names_type;
    typedef typename options_type::arguments_type                               arguments_type;
    typedef typename options_type::renderer_type                                renderer_type;

    typedef typename value_type::sequence_type                                  sequence_type;

  public:

    template <class Iterator>
    struct kernel;

  public:

    inline static char const* name() { return "django"; }

  private:

    template <class K> friend struct django::builtin_tags;
    template <class K> friend struct django::builtin_filters;

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
    typedef builtin_filters<kernel_type>                                        builtin_filters_type;
    typedef std::map<string_type, string_type>                                  markers_type; // TODO[c++11]: unordered_map.

    typedef typename kernel_type::id_type                                       id_type;
    typedef typename kernel_type::regex_type                                    regex_type;
    typedef typename kernel_type::match_type                                    match_type;
    typedef typename kernel_type::string_regex_type                             string_regex_type;
    typedef typename kernel_type::string_match_type                             string_match_type;
    typedef detail::text<string_type>                                           text;

  private:

    template <class I> friend struct kernel;
    template <class K> friend struct django::builtin_tags;
    template <class K> friend struct django::builtin_filters;

  public:

    kernel()
        : newline        (text::literal("\n"))
        , ellipsis       (text::literal("..."))
        , brace_open     (marker(text::literal("{"),  text::literal("openbrace")))
        , brace_close    (marker(text::literal("}"),  text::literal("closebrace")))
        , block_open     (marker(text::literal("{%"), text::literal("openblock")))
        , block_close    (marker(text::literal("%}"), text::literal("closeblock")))
        , comment_open   (marker(text::literal("{#"), text::literal("opencomment")))
        , comment_close  (marker(text::literal("#}"), text::literal("closecomment")))
        , variable_open  (marker(text::literal("{{"), text::literal("openvariable")))
        , variable_close (marker(text::literal("}}"), text::literal("closevariable")))
        {
//
// common grammar
////////////////////////////////////////////////////////////////////////////////////////////////////

        identifier
            = ((x::alpha | '_') >> *_w >> _b)
            ;
        keyword_identifier
            = identifier[ x::check(in(keywords_)) ]
            ;
        nonkeyword_identifier
            = identifier[ x::check(not_in(keywords_)) ]
            ;
        unreserved_identifier
            = nonkeyword_identifier[ x::check(not_in(reserved_)) ]
            ;
        name
            = (id = nonkeyword_identifier) >> *_s
            ;
        names
            = +name
            ;
        package
            = (id = (nonkeyword_identifier >> *('.' >> identifier))) >> *_s
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
            = !(x::set = '-','+') >> +_d >> !('.' >> +_d) >> !('e' >> +_d)
            ;
        string_literal
            =   '\"' >> (str = *~as_xpr('\"')) >> '\"'
            |   '\'' >> (str = *~as_xpr('\'')) >> '\''
            | "_(\"" >> (str = *~as_xpr('\"')) >> "\")"
            | "_(\'" >> (str = *~as_xpr('\'')) >> "\')"
            ;
        variable_literal
            = nonkeyword_identifier
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
            = as_xpr("==")
            | as_xpr("!=")
            | as_xpr("<=")
            | as_xpr(">=")
            | as_xpr("<")
            | as_xpr(">")
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
            = !(nonkeyword_identifier >> as_xpr('=')) >> value
            ;
        arguments
            = *argument
            ;
        this->skipper
            = block_open
            | block_close    // TODO: Comment out if it is allowed on its own outside a block tag.
            | comment_open
            | comment_close  // TODO: Comment out if it is allowed on its own outside a comment tag.
            | variable_open
         // | variable_close // Note: This is allowed outside of a variable tag.
            ;
        html_namechar
            = ~(x::set = ' ', '\t', '\n', '\v', '\f', '\r', '>')
            ;
        html_whitespace
            = (x::set = ' ', '\t', '\n', '\v', '\f', '\r')
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

  private:

    struct in {
        symbols_type const& symbols;
        explicit in(symbols_type const& symbols) : symbols(symbols) {}

        boolean_type operator ()(typename match_type::value_type const& match) const {
            return this->symbols.find(match.str()) != this->symbols.end();
        }
    };

    struct not_in {
        symbols_type const& symbols;
        explicit not_in(symbols_type const& symbols) : symbols(symbols) {}

        boolean_type operator ()(typename match_type::value_type const& match) const {
            return this->symbols.find(match.str()) == this->symbols.end();
        }
    };

  public: // TODO: Make protected, and make builtin_tags/builtin_filters friends.

    inline regex_type marker  (string_type const& s, string_type const& name) { return as_xpr((this->markers[name] = s)); }
    inline regex_type word    (string_type const s) { return as_xpr(s) >> _b; }
    inline regex_type word    (char const* const s) { return as_xpr(s) >> _b; }
    inline regex_type op      (char const* const s) { return this->word(*this->keywords_.insert(text::literal(s)).first); }
    inline regex_type keyword (char const* const s) { return this->word(*this->keywords_.insert(text::literal(s)).first) >> *_s; }
    inline regex_type reserved(char const* const s) { return this->word(*this->reserved_.insert(text::literal(s)).first) >> *_s; }

    sequence_type split_argument( options_type const& options
                                , state_type   const& state
                                , value_type   const& argument
                                , context_type&       context
                                , char_type    const  delimiter
                                ) const {
        typedef typename string_type::const_iterator                                string_iterator_type;
        typedef kernel<string_iterator_type>                                        string_kernel_type;
        typedef typename string_kernel_type::match_type                             string_match_type;
        typedef typename string_kernel_type::state_type                             string_state_type;
        typedef typename string_kernel_type::range_type                             string_range_type;

        typedef boost::char_separator<char_type>                                    separator_type;
        typedef boost::tokenizer<separator_type, string_iterator_type, string_type> tokenizer_type;

        BOOST_ASSERT(argument.is_literal());
        string_type token = argument.token();
        size_type const l = token.length();

        if (l >= 2) { // Adjust the token by trimming the quotes:
            if ((token[0] == char_type('"')  && token[l - 1] == char_type('"')) ||
                (token[0] == char_type('\'') && token[l - 1] == char_type('\''))) {
                token = token.substr(1, l - 2);
            }
        }

        char_type const d[2] = { delimiter, 0 };
        tokenizer_type const tokenizer(token, separator_type(d, 0, boost::keep_empty_tokens));
        static string_kernel_type const string_kernel;
        static string_range_type  const string_range;
        string_match_type               string_match;
        sequence_type                   sequence;

        BOOST_FOREACH(string_type const& t, tokenizer) {
            if (std::distance(t.begin(), t.end()) == 0) {
                sequence.push_back(value_type(none_type()));
            }
            else if (x::regex_match(t.begin(), t.end(), string_match, string_kernel.chain)) {
                try {
                    string_state_type const string_state(string_range, options);
                    sequence.push_back(string_kernel.evaluate_chain(options, string_state, string_match, context));
                }
                catch (missing_variable const& e) {
                    string_type const string(t.begin(), t.end());

                    if (text::narrow(string) != e.name) {
                        throw;
                    }

                    // A missing variable means an embedded argument was meant as a string literal.
                    value_type value = string;
                    value.token(string_match[0]);
                    sequence.push_back(value);
                }
            }
        }

        return sequence;
    }

    path_type extract_path(match_type const& match) const {
        return traits_type::to_path(this->extract_string(match));
    }

    string_type extract_string(match_type const& match) const {
        BOOST_ASSERT(this->is(match, this->string_literal));
        return text::unquote(match[str].str()); // TODO: Handle _(...).
    }

    names_type extract_names(match_type const& match) const {
        names_type names;
        BOOST_FOREACH(match_type const& name, this->select_nested(match, this->name)) {
            names.push_back(name[id].str());
        }
        return names;
    }

    void render( ostream_type&       ostream
               , options_type const& options
               , state_type   const& state
               , context_type&       context
               ) const {
        // context.safe(false);

        context.format(text::literal("TEMPLATE_STRING_IF_INVALID"), text::literal(""),          false);
        context.format(text::literal("DATE_FORMAT"),                text::literal("N j, Y"),    false);
        context.format(text::literal("DATETIME_FORMAT"),            text::literal("N j, Y, P"), false);
        context.format(text::literal("MONTH_DAY_FORMAT"),           text::literal("F j"),       false);
        context.format(text::literal("SHORT_DATE_FORMAT"),          text::literal("m/d/Y"),     false);
        context.format(text::literal("SHORT_DATETIME_FORMAT"),      text::literal("m/d/Y P"),   false);
        context.format(text::literal("TIME_FORMAT"),                text::literal("P"),         false);
        context.format(text::literal("YEAR_MONTH_FORMAT"),          text::literal("F Y"),       false);
        context.format(text::literal("SPACE_FORMAT"),               text::literal("&nbsp;"),    false);

        this->render_block(ostream, options, state, state.match, context);
    }

    void render_path( ostream_type&       ostream
                    , options_type const& options
                    , state_type   const& state
                    , path_type    const& path
                    , context_type&       context
                    ) const {
        // TODO: options.path_renderer(path)(ostream, context, ...);
        typedef boost::shared_ptr<templates::path_template<engine_type> const> path_template_type;
        options.template parse<path_template_type>(path)->render_to_stream(ostream, context);
    }

    void render_plain( ostream_type&       ostream
                     , options_type const& options
                     , state_type   const& state
                     , match_type   const& plain
                     , context_type&       context
                     ) const {
        ostream << plain.str();
    }

    void render_block( ostream_type&       ostream
                     , options_type const& options
                     , state_type   const& state
                     , match_type   const& block
                     , context_type&       context
                     ) const {
        BOOST_FOREACH(match_type const& nested, block.nested_results()) {
            this->render_match(ostream, options, state, nested, context);
        }
    }

    void render_tag( ostream_type&       ostream
                   , options_type const& options
                   , state_type   const& state
                   , match_type   const& match
                   , context_type&       context
                   ) const {
        match_type const& m  = this->unnest(match);
        id_type    const  id = m.regex_id();

        if (typename builtin_tags_type::tag_type const tag = builtin_tags_.get(id)) {
            tag(*this, options, state, m, context, ostream);
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("missing built-in tag"));
        }
    }

    void render_match( ostream_type&       ostream
                     , options_type const& options
                     , state_type   const& state
                     , match_type   const& match
                     , context_type&       context
                     ) const {
             if (this->is(match, this->plain)) this->render_plain(ostream, options, state, match, context);
        else if (this->is(match, this->block)) this->render_block(ostream, options, state, match, context);
        else if (this->is(match, this->tag))   this->render_tag(ostream, options, state, match, context);
        else AJG_SYNTH_THROW(std::logic_error("invalid template state"));
    }

    value_type apply_filters( value_type   const& value
                            , options_type const& options
                            , state_type   const& state
                            , match_type   const& match
                            , context_type&       context
                            ) const {
        value_type v = value;

        BOOST_FOREACH(match_type const& filter, this->select_nested(match, this->filter)) {
            BOOST_ASSERT(this->is(filter, this->filter));
            string_type const& name  = filter(this->name)[id].str();
            match_type  const& chain = filter(this->chain);

            arguments_type arguments;
            if (chain) {
                arguments.first.push_back(this->evaluate_chain(options, state, chain, context));
            }
            v = this->apply_filter(v, options, state, name, arguments, context);
        }

        return v;
    }

    value_type apply_filter( value_type     const& value
                           , options_type   const& options
                           , state_type     const& state
                           , string_type    const& name
                           , arguments_type const& arguments
                           , context_type&         context
                           ) const {
        // Let library filters override built-in ones.
        if (boost::optional<typename options_type::filter_type> const& filter = state.get_filter(name)) {
            return (*filter)(value, arguments, context);
        }
        else if (typename builtin_filters_type::filter_type const filter = builtin_filters_type::get(name)) {
            return filter(*this, options, state, value, arguments, context);
        }
        else {
            AJG_SYNTH_THROW(missing_filter(text::narrow(name)));
        }
    }

    value_type evaluate( options_type const& options
                       , state_type   const& state
                       , match_type   const& match
                       , context_type&       context
                       ) const {

        try {
            value_type const& value = this->evaluate_expression(options, state, match(this->expression), context);
            return this->apply_filters(value, options, state, match, context);
        }
        catch (missing_variable  const&) { return context.format(text::literal("TEMPLATE_STRING_IF_INVALID")); }
        catch (missing_attribute const&) { return context.format(text::literal("TEMPLATE_STRING_IF_INVALID")); }
    }

    arguments_type evaluate_arguments( options_type  const& options
                                     , state_type    const& state
                                     , match_type    const& match
                                     , context_type&        context
                                     ) const {
        arguments_type arguments;
        BOOST_FOREACH(match_type const& arg, this->select_nested(match, this->argument)) {
            value_type const& value = this->evaluate(options, state, arg(this->value), context);
            if (match_type const& name = arg(this->nonkeyword_identifier)) {
                arguments.second[name.str()] = value; // Keyword argument.
            }
            else {
                arguments.first.push_back(value); // Positional argument.
            }
        }
        return arguments;
    }

    // TODO: Allow traits_type or value_type to produce the actual values, so that e.g. the Python
    //       binding can generate true Python bools, numbers, strings, etc. from literals.
    value_type evaluate_literal( options_type  const& options
                               , state_type    const& state
                               , match_type    const& match
                               , context_type&        context
                               ) const {
        BOOST_ASSERT(this->is(match, this->literal));
        match_type  const& literal = this->unnest(match);
        string_type const  string  = match.str();
        string_type const  token   = literal[0];

        if (this->is(literal, this->none_literal)) {
            return value_type(none_type()).token(token);
        }
        else if (this->is(literal, this->boolean_literal)) {
            match_type const& boolean = this->unnest(literal);

            if (this->is(boolean, this->true_literal)) {
                return value_type(boolean_type(true)).token(token);
            }
            else if (this->is(boolean, this->false_literal)) {
                return value_type(boolean_type(false)).token(token);
            }
            else {
                AJG_SYNTH_THROW(std::logic_error("invalid boolean literal"));
            }
        }
        else if (this->is(literal, this->number_literal)) {
            if (string.find(char_type('.')) == string_type::npos) {
                // TODO[c++11]: atoll (long long)
                long const l = (std::atol)(text::narrow(string).c_str());
                return value_type(integer_type(l)).token(token);
            }
            else {
                double const d = (std::atof)(text::narrow(string).c_str());
                return value_type(floating_type(d)).token(token);
            }
        }
        else if (this->is(literal, this->string_literal)) {
            return value_type(extract_string(literal)).token(token);
        }
        else if (this->is(literal, this->variable_literal)) {
            if (boost::optional<value_type> const& variable = context.get(string)) {
                return variable->metacopy().token(token);
            }
            else {
                // TODO: Don't use exceptions for unexceptional control flow.
                /*AJG_SYNTH_THROW*/throw (missing_variable(text::narrow(string)));
            }
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("invalid literal"));
        }
    }

    value_type evaluate_expression( options_type const& options
                                  , state_type   const& state
                                  , match_type   const& match
                                  , context_type&       context
                                  ) const {
        match_type const& expr = this->unnest(match);

        if (this->is(expr, this->unary_expression)) {
            return this->evaluate_unary(options, state, expr, context);
        }
        else if (this->is(expr, this->binary_expression)) {
            return this->evaluate_binary(options, state, expr, context);
        }
        else if (this->is(expr, this->nested_expression)) {
            match_type const& nested = expr(this->expression);
            return this->evaluate_expression(options, state, nested, context);
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("invalid expression"));
        }
    }

    value_type evaluate_unary( options_type const& options
                             , state_type   const& state
                             , match_type   const& match
                             , context_type&       context
                             ) const {
        BOOST_ASSERT(this->is(match, this->unary_expression));
        string_type const& op      = match(unary_operator).str();
        match_type  const& operand = match(expression);

        if (op == text::literal("not")) {
            return !evaluate_expression(options, state, operand, context);
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("invalid unary operator"));
        }
    }

    value_type evaluate_binary( options_type const& options
                              , state_type   const& state
                              , match_type   const& match
                              , context_type&       context
                              ) const {
        BOOST_ASSERT(this->is(match, this->binary_expression));
        match_type const& chain = match(this->chain);
        value_type value = this->evaluate_chain(options, state, chain, context);
        string_type op;

        BOOST_FOREACH(match_type const& segment, detail::drop(match.nested_results(), 1)) {
            if (this->is(segment, this->binary_operator)) {
                op = segment.str();
            }
            else if (!(this->is(segment, this->expression))) {
                AJG_SYNTH_THROW(std::logic_error("invalid binary expression"));
            }
            else if (op == text::literal("==")) {
                value = value == this->evaluate_expression(options, state, segment, context);
            }
            else if (op == text::literal("!=")) {
                value = value != this->evaluate_expression(options, state, segment, context);
            }
            else if (op == text::literal("<")) {
                value = value < this->evaluate_expression(options, state, segment, context);
            }
            else if (op == text::literal(">")) {
                value = value > this->evaluate_expression(options, state, segment, context);
            }
            else if (op == text::literal("<=")) {
                value = value <= this->evaluate_expression(options, state, segment, context);
            }
            else if (op == text::literal(">=")) {
                value = value >= this->evaluate_expression(options, state, segment, context);
            }
            else if (op == text::literal("and")) {
                value = value ? this->evaluate_expression(options, state, segment, context) : value;
            }
            else if (op == text::literal("or")) {
                value = value ? value : this->evaluate_expression(options, state, segment, context);
            }
            else if (op == text::literal("in")) {
                value_type const elements = this->evaluate_expression(options, state, segment, context);
                value = elements.contains(value);
            }
            else if (text::begins_with(op, text::literal("not"))
                  && text::ends_with(op, text::literal("in"))) {
                value_type const elements = this->evaluate_expression(options, state, segment, context);
                value = !elements.contains(value);
            }
            else {
                AJG_SYNTH_THROW(std::logic_error("invalid binary operator"));
            }
        }

        return value;
    }

    value_type evaluate_link( options_type const& options
                            , state_type   const& state
                            , match_type   const& match
                            , context_type&       context
                            ) const {
        match_type const& link = this->unnest(match);

        if (this->is(link, this->subscript_link)) { // i.e. value[attribute]
            return this->evaluate(options, state, link(this->expression), context);
        }
        else if (this->is(link, this->attribute_link)) { // i.e. value.attribute
            return string_type(link(this->identifier).str());
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("invalid link"));
        }
    }

    value_type evaluate_chain( options_type const& options
                             , state_type   const& state
                             , match_type   const& match
                             , context_type&       context
                             ) const {
        BOOST_ASSERT(this->is(match, this->chain));
        match_type const& lit = match(this->literal);
        value_type value = this->evaluate_literal(options, state, lit, context);

        BOOST_FOREACH(match_type const& link, this->select_nested(match, this->link)) {
            value_type const attribute = this->evaluate_link(options, state, link, context);
            value = value.must_get_attribute(attribute);
        }

        return value;
    }

    url_type get_view_url( options_type   const& options
                         , state_type     const& state
                         , string_type    const& view
                         , arguments_type const& arguments
                         , context_type&         context
                         ) const {
        BOOST_FOREACH(typename options_type::resolver_type const& resolver, options.resolvers) {
            if (url_type const& url = resolver->reverse(view, arguments, context, options)) {
                return url;
            }
        }
        return url_type();
    }

  private:

    // NOTE: These must be near or at the top so they are initialized by the time they're needed.
    symbols_type       keywords_;
    symbols_type       reserved_;
    markers_type       markers;

  public:

    // TODO: Move these out of the kernel.
    string_type  const newline;
    string_type  const ellipsis;

    regex_type   const brace_open;
    regex_type   const brace_close;
    regex_type   const block_open;
    regex_type   const block_close;
    regex_type   const comment_open;
    regex_type   const comment_close;
    regex_type   const variable_open;
    regex_type   const variable_close;

  protected:

    regex_type identifier;
    regex_type keyword_identifier;
    regex_type nonkeyword_identifier;
    regex_type unreserved_identifier;
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
    regex_type number_literal;
    regex_type string_literal;
    regex_type variable_literal;
    regex_type literal;
    regex_type polyadic_tag; // TODO: Rename custom_tag or library_tag.

    string_regex_type html_namechar;
    string_regex_type html_whitespace;
    string_regex_type html_tag;

  private:

    x::placeholder<state_type> _state;

  private:

    builtin_tags_type builtin_tags_;

}; // kernel

}}}} // namespace ajg::synth::engines::django

#endif // AJG_SYNTH_ENGINES_DJANGO_ENGINE_HPP_INCLUDED
