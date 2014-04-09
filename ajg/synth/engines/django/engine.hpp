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

using detail::operator ==;
using detail::find_mapped_value;
namespace x = boost::xpressive;

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

    std::map<string_type, string_type> markers;

    inline regex_type word(string_type const& n) {
        namespace x = boost::xpressive;
        return x::as_xpr(n) >> x::_b;
    }

    inline regex_type reserved(string_type const& n) {
        namespace x = boost::xpressive;
        this->reserved_.insert(n);
        return x::as_xpr(n) >> x::_b >> *x::_s;
    }

    inline regex_type keyword(string_type const& k) {
        namespace x = boost::xpressive;
        this->keywords_.insert(k);
        return x::as_xpr(k) >> x::_b >> *x::_s;
    }

    inline regex_type xxx(string_type const& k) {
        namespace x = boost::xpressive;
        this->keywords_.insert(k);
        return x::as_xpr(k) >> x::_b;
    }

    inline regex_type yyy(string_type const& k) {
        namespace x = boost::xpressive;
        return x::as_xpr(k) >> x::_b;
    }

    template <char_type C>
    inline static regex_type token() {
        namespace x = boost::xpressive;
        return x::as_xpr(C) >> *x::_s;
    }

    /*
    template <char_type C, char_type D>
    inline static regex_type token() {
        namespace x = boost::xpressive;
        return x::as_xpr(C) >> x::as_xpr(D) >> *x::_s;
    }
    */

    template <typename Char, size_type M, size_type N>
    inline regex_type marker(Char const (&m)[M], Char const (&n)[N]) {
        this->markers[traits_type::literal(n)] = traits_type::literal(m);
        return x::as_xpr(m);
    }

    /*
    inline static regex_type token(char_type const* const s) {
        namespace x = boost::xpressive;
        return x::as_xpr(s) >> *x::_s;
    }
    */

    /*inline static regex_type token(string_type const& t) {
        namespace x = boost::xpressive;
        return x::as_xpr(t) >> *x::_s;
    }*/

    definition()
        : newline        (traits_type::literal("\n"))
        , ellipsis       (traits_type::literal("..."))
        , brace_open     (marker("{",  "openbrace"))
        , brace_close    (marker("}",  "closebrace"))
        , block_open     (marker("{%", "openblock"))
        , block_close    (marker("%}", "closeblock"))
        , comment_open   (marker("{#", "opencomment"))
        , comment_close  (marker("#}", "closecomment"))
        , variable_open  (marker("{{", "openvariable"))
        , variable_close (marker("}}", "closevariable")) {
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
            = token<'['>() >> x::ref(expression) >> ']'
            ;
        link
            = attribute_link
            | subscript_link
            ;
        chain
            // TODO: Consider generalizing literal to expression
            = literal >> *link >> *_s
            ;
        unary_operator
            = xxx("not")
            ;
        binary_operator
            = yyy("==")
            | yyy("!=")
            | yyy("<=")
            | yyy(">=")
            | yyy("<")
            | yyy(">")
            | xxx("and")
            | xxx("or")
            | xxx("in")
            | xxx("not") >> *_s >> xxx("in")
            ;
        binary_expression
            = chain >> *(binary_operator >> *_s >> x::ref(expression))
            ;
        unary_expression
            = unary_operator >> *_s >> x::ref(expression)
            ;
        nested_expression
            = token<'('>() >> x::ref(expression) >> token<')'>()
            ;
        expression
            = unary_expression
            | binary_expression
            | nested_expression
            ;
        arguments
            = *expression
            ;
        variables
            = name >> *(token<','>() >> name)
            ;
        filter
            = name >> !(token<':'>() >> chain)
            ;
        filters
            = *(as_xpr('|') >> filter)
            ;
        pipeline
            = filter >> *(as_xpr('|') >> filter)
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
        // Handles "string" or 'string'.
        // TODO: Escape sequences, etc.
        BOOST_ASSERT(match == this->string_literal);
        string_type const string = match.str();
        BOOST_ASSERT(string.size() >= 2);
        return string.substr(1, string.size() - 2);
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
            return it->second(options, &context, value, arguments);
        }

        // TODO: Pass the full arguments, not just the sequential (.first) ones.
        if (typename builtin_filters_type::filter_type const filter = builtin_filters_type::get(name)) {
            return filter(*this, value, arguments.first, context, options);
        }
        else {
            throw_exception(missing_filter(traits_type::narrow(name)));
        }
    }

    value_type evaluate( match_type     const& match
                       , context_type   const& context
                       , options_type   const& options
              // TODO: , arguments_type const& arguments = arguments_type()
                       ) const {
        return evaluate_expression(match, context, options);
    }

    arguments_type evaluate_arguments( match_type    const& args
                                     , context_type  const& context
                                     , options_type  const& options
                                     ) const {
        arguments_type arguments;
        // TODO: Evaluate the full arguments, not just the sequential (.first) ones.
        BOOST_FOREACH(match_type const& arg, args.nested_results()) {
            arguments.first.push_back(this->evaluate_expression(arg, context, options));
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
            return value_type(none_type()).with_token(literal[0]);
        }
        else if (literal == boolean_literal) {
            match_type const& boolean = detail::unnest(literal);

            if (boolean == true_literal) {
                return value_type(boolean_type(true)).with_token(literal[0]);
            }
            else if (boolean == false_literal) {
                return value_type(boolean_type(false)).with_token(literal[0]);
            }
            else {
                throw_exception(std::logic_error("invalid boolean literal"));
            }
        }
        else if (literal == number_literal) {
            return value_type(traits_type::to_number(string)).with_token(literal[0]);
        }
        else if (literal == string_literal) {
            // Adjust the token by trimming the quotes.
            string_type const token = string_type(literal[0].first + 1, literal[0].second - 1);
            return value_type(extract_string(literal)).with_token(token);
        }
        else if (literal == variable_literal) {
            if (optional<value_type const&> const variable = detail::find_value(string, context)) {
                return value_type(*variable).with_token(literal[0]);
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
            return evaluate_unary(expr, context, options);
        }
        else if (expr == binary_expression) {
            return evaluate_binary(expr, context, options);
        }
        else if (expr == nested_expression) {
            match_type const& nested = expr(this->expression);
            return evaluate_expression(nested, context, options);
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
            throw_exception(std::logic_error("invalid unary operator: " + op));
        }
    }

    value_type evaluate_binary( match_type   const& match
                              , context_type const& context
                              , options_type const& options
                              ) const {
        BOOST_ASSERT(match == binary_expression);
        // First, evaluate the first segment, which is always present and always a chain.
        match_type const& chain = match(this->chain);
        value_type value = evaluate_chain(chain, context, options);
        size_type i = 0;
        string_type op;

        BOOST_FOREACH(match_type const& segment, match.nested_results()) {
            if (!i++) continue; // Skip the first segment (the chain.)
            else if (segment == binary_operator) {
                op = segment.str();
                continue;
            }
            else if (!(segment == expression)) {
                throw_exception(std::logic_error("invalid binary expression"));
            }

            if (op == traits_type::literal("==")) {
                value = value == evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal("!=")) {
                value = value != evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal("<")) {
                value = value < evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal(">")) {
                value = value > evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal("<=")) {
                value = value <= evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal(">=")) {
                value = value >= evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal("and")) {
                value = value ? evaluate_expression(segment, context, options) : value;
            }
            else if (op == traits_type::literal("or")) {
                value = value ? value : evaluate_expression(segment, context, options);
            }
            else if (op == traits_type::literal("in")) {
                value_type const elements = evaluate_expression(segment, context, options);
                value = elements.contains(value);
            }
            else if (boost::algorithm::starts_with(op, traits_type::literal("not"))
                  && boost::algorithm::ends_with(op, traits_type::literal("in"))) {
                value_type const elements = evaluate_expression(segment, context, options);
                value = !elements.contains(value);
            }
            else {
                throw_exception(std::logic_error("invalid binary operator: " + op));
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
            return evaluate(link(this->expression), context, options);
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
        value_type value = evaluate_literal(lit, context, options);

        BOOST_FOREACH(match_type const& link, detail::select_nested(match, this->link)) {
            value_type const attribute = this->evaluate_link(link, context, options);
            value = value.must_get_attribute(attribute);
        }

        return value;
    }

    string_type format_datetime( options_type  const& options
                               , string_type   const& format
                               , datetime_type const& datetime
                               ) const {
        typedef std::map<char_type, string_type>            transliterations_type;
        typedef typename transliterations_type::value_type  transliteration_type;

        static transliterations_type const transliterations = boost::assign::list_of<transliteration_type>
            (char_type('%'), traits_type::literal("%%"))
			(char_type('a'), traits_type::literal(AJG_SYNTH_IF_WINDOWS("", "%P"))) // TODO: Periods; implement on Windows.
            (char_type('A'), traits_type::literal("%p"))
            (char_type('b'), traits_type::literal("%b")) // TODO: Lowercase
            (char_type('B'), traits_type::literal(""))   // "Not implemented" per spec.
            (char_type('c'), traits_type::literal("%Y-%m-%dT%H:%M:%S%z"))
            (char_type('d'), traits_type::literal("%d"))
            (char_type('D'), traits_type::literal("%a"))
            (char_type('e'), traits_type::literal("%z"))    // TODO: Ignored with ptimes
            (char_type('E'), traits_type::literal("%B"))    // TODO: Make locale-aware
            (char_type('f'), traits_type::literal("%l:%M")) // TODO: No leading blank, no zero minutes
            (char_type('F'), traits_type::literal("%B"))
            (char_type('g'), traits_type::literal("%l"))    // TODO: No leading blank
            (char_type('G'), traits_type::literal("%k"))    // TODO: No leading blank
            (char_type('h'), traits_type::literal("%I"))
            (char_type('H'), traits_type::literal("%H"))
            (char_type('i'), traits_type::literal("%M"))
            (char_type('I'), traits_type::literal(""))   // TODO: Implement
            (char_type('j'), traits_type::literal(AJG_SYNTH_IF_WINDOWS("", "%e"))) // TODO: No leading blank; implement on Windows.
            (char_type('l'), traits_type::literal("%A"))
            (char_type('L'), traits_type::literal(""))   // TODO: Implement
            (char_type('m'), traits_type::literal("%m"))
            (char_type('M'), traits_type::literal("%b"))
            (char_type('n'), traits_type::literal("%m")) // TODO: No leading zeros
            (char_type('N'), traits_type::literal("%b")) // TODO: Abbreviations/periods
            (char_type('o'), traits_type::literal("%G"))
            (char_type('O'), traits_type::literal(""))   // TODO: Implement
			(char_type('P'), traits_type::literal(AJG_SYNTH_IF_WINDOWS("", "%r"))) // TODO: Periods, no zero minutes, "midnight"/"noon"; implement on Windows.
            (char_type('r'), traits_type::literal("%a, %d %b %Y %T %z"))
            (char_type('s'), traits_type::literal("%S"))
            (char_type('S'), traits_type::literal(""))   // TODO: Implement
            (char_type('t'), traits_type::literal(""))   // TODO: Implement
            (char_type('T'), traits_type::literal(""))   // TODO: Implement
            (char_type('u'), traits_type::literal("%f")) // TODO: No leading period
            (char_type('U'), traits_type::literal(""))   // TODO: Implement
            (char_type('w'), traits_type::literal("%w"))
			(char_type('W'), traits_type::literal(AJG_SYNTH_IF_WINDOWS("", "%V"))) // TODO: No leading zeros; implement on Windows.
            (char_type('y'), traits_type::literal("%y"))
            (char_type('Y'), traits_type::literal("%Y"))
            (char_type('z'), traits_type::literal("%j")) // TODO: No leading zeros
            (char_type('Z'), traits_type::literal(""))   // TODO: Implement
            ;

        std::basic_ostringstream<char_type> stream;
		typename options_type::formats_type::const_iterator const it = options.formats.find(format);
		string_type const original = it == options.formats.end() ? format : it->second;

        // TODO: This might not be UTF8-safe; consider using a utf8_iterator.
		BOOST_FOREACH(char_type const c, original) {
            stream << find_mapped_value(c, transliterations).get_value_or(string_type(1, c));
        }

        return detail::format_time<string_type>(stream.str(), datetime);
    }

    // TODO: Proper, localizable formatting.
    // TODO: Factor out to detail.
    string_type format_duration( options_type  const& options
                               , duration_type const& duration
                               ) const {
        BOOST_STATIC_CONSTANT(size_type, N = 6);

        static size_type const seconds[N] = { 60 * 60 * 24 * 365
                                            , 60 * 60 * 24 * 30
                                            , 60 * 60 * 24 * 7
                                            , 60 * 60 * 24
                                            , 60 * 60
                                            , 60
                                            };
        static string_type const units[N] = { traits_type::literal("year")
                                            , traits_type::literal("month")
                                            , traits_type::literal("week")
                                            , traits_type::literal("day")
                                            , traits_type::literal("hour")
                                            , traits_type::literal("minute")
                                            };

        if (duration.is_negative()) {
            return pluralize_unit(0, units[N - 1], options);
        }

        string_type result;
        size_type const total = duration.total_seconds();
        size_type count = 0, i = 0;

        for (; i < N; ++i) {
            if ((count = total / seconds[i])) {
                break;
            }
        }

        result += pluralize_unit(count, units[i], options);

        if (i + 1 < N) {
            if ((count = (total - (seconds[i] * count)) / seconds[i + 1])) {
                result += traits_type::literal(", ") + pluralize_unit(count, units[i + 1], options);
            }
        }

        return result;
    }

    /*inline static string_type nonbreaking(string_type const& s) {
        return boost::algorithm::replace_all_copy(s, traits_type::literal(" "), options.nonbreaking_space);
    }*/

    inline static string_type pluralize_unit( size_type    const  n
                                            , string_type  const& s
                                            , options_type const& options
                                            ) {
        string_type const suffix = n == 1 ? string_type() : traits_type::literal("s");
        return traits_type::to_string(n) + options.nonbreaking_space + s + suffix;
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

    // TODO: Parallelize the formatting:
    regex_type tag, text, block, skipper, nothing;
    regex_type identifier, restricted_identifier, unreserved_identifier;
    regex_type unreserved_name, name, names;
    regex_type package, packages;
    regex_type arguments;
    regex_type variables;
    regex_type filter, filters, pipeline;
    regex_type chain, link, subscript_link, attribute_link;
    regex_type unary_operator, binary_operator;
    regex_type unary_expression, binary_expression, nested_expression, expression;
    regex_type none_literal;
    regex_type true_literal, false_literal, boolean_literal;
    regex_type string_literal, number_literal, variable_literal, literal;

    string_regex_type html_namechar;
    string_regex_type html_whitespace;
    string_regex_type html_tag;

  private:

    builtin_tags_type builtin_tags_;

}; // definition

}; // engine

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_ENGINE_HPP_INCLUDED
