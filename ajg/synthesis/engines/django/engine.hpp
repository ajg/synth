//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_ENGINES_DJANGO_ENGINE_HPP_INCLUDED
#define AJG_SYNTHESIS_ENGINES_DJANGO_ENGINE_HPP_INCLUDED

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

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>

#include <ajg/synthesis/template.hpp>
#include <ajg/synthesis/engines/detail.hpp>
#include <ajg/synthesis/engines/exceptions.hpp>
#include <ajg/synthesis/engines/base_definition.hpp>
#include <ajg/synthesis/engines/django/value.hpp>
#include <ajg/synthesis/engines/django/library.hpp>

namespace ajg {
namespace synthesis {
namespace django {

//
// options
////////////////////////////////////////////////////////////////////////////////

template <class Iterator, class Value>
struct options {
    typedef Iterator                           iterator_type;
    typedef Value                              value_type;
    typedef typename iterator_type::value_type char_type;
    typedef std::basic_string<char_type>       string_type;
    typedef typename string_type::size_type    size_type;

    bool autoescape;
    std::map<string_type, string_type>* blocks;
    std::map<iterator_type, size_type>  cycles;
    std::map<iterator_type, value_type> registry;

    options() : autoescape(true), blocks(0) {}
};

using detail::operator ==;

template <class Library = django::default_library>
struct engine : detail::nonconstructible {

typedef engine engine_type;

template <class BidirectionalIterator>
struct definition : base_definition< BidirectionalIterator
                                   , definition<BidirectionalIterator>
                                   > {
  private:

    template <class Sequence>
    struct define_sequence;

  public:

    typedef definition this_type;
    typedef base_definition< BidirectionalIterator
                           , this_type> base_type;

    typedef typename base_type::id_type         id_type;
    typedef typename base_type::size_type       size_type;
    typedef typename base_type::char_type       char_type;
    typedef typename base_type::match_type      match_type;
    typedef typename base_type::regex_type      regex_type;
    typedef typename base_type::frame_type      frame_type;
    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stream_type     stream_type;
    typedef typename base_type::iterator_type   iterator_type;
    typedef typename base_type::definition_type definition_type;

    typedef Library                            library_type;
    typedef typename library_type::first       tags_type;
    typedef typename library_type::second      filters_type;
    typedef django::value<iterator_type>       value_type;
    typedef options<iterator_type, value_type> options_type;
    typedef std::map<string_type, value_type>  context_type;
    typedef std::vector<value_type>            array_type;

    typedef detail::indexable_sequence<this_type, tags_type,
        id_type, detail::create_definitions_extended>      tag_sequence_type;
    typedef detail::indexable_sequence<this_type, filters_type,
        string_type, detail::create_definitions_extended>  filter_sequence_type;

  public:

    definition()
        : newline        (detail::text("\n"))
        , brace_open     (detail::text("{"))
        , brace_close    (detail::text("}"))
        , block_open     (detail::text("{%"))
        , block_close    (detail::text("%}"))
        , comment_open   (detail::text("{#"))
        , comment_close  (detail::text("#}"))
        , variable_open  (detail::text("{{"))
        , variable_close (detail::text("}}"))
        , default_value  (detail::text("")) {
        using namespace xpressive;
//
// common grammar
////////////////////////////////////////////////////////////////////////////////

        identifier
            = (alpha | '_') >> *_w
            ;
        none_literal
            = as_xpr("None")
            ;
        boolean_literal
            = as_xpr("True")
            | as_xpr("False")
            ;
        number_literal
            = !(set= '-','+') >> +_d // integral part
                >> !('.' >> +_d)     // floating part
                >> !('e' >> +_d)     // exponent part
            ;
        string_literal
            = '"'  >> *~as_xpr('"')  >> '"'
            | '\'' >> *~as_xpr('\'') >> '\''
            ;
        variable_literal
            = identifier
            ;
        literal
            = none_literal
            | boolean_literal
            | number_literal
            | string_literal
            | variable_literal
            ;
        attribution
            = '.' >> *_s >> identifier
            ;
        subscription
            = '[' >> *_s >> xpressive::ref(expression) >> *_s >> ']'
            ;
        expression
            = literal >> *(*_s >> (attribution | subscription))
            ;
        filter
            = identifier >> !(':' >> *_s >> expression)
            ;
        pipe
            = filter >> *(*_s >> '|' >> *_s >> filter)
            ;
        skipper
            = as_xpr( block_open) | comment_open  | variable_open
                    | block_close | comment_close | variable_close
            ;

        this->initialize_grammar();
        fusion::for_each(tags_.definition, detail::construct
            <detail::element_initializer<this_type> >(*this));
        fusion::for_each(filters_.definition,
            detail::construct<append_filter>(*this));
        detail::index_sequence<this_type, tag_sequence_type,
            &this_type::tags_, tag_sequence_type::size>(*this);
    }

  public:

    value_type apply_filter( value_type   const& value
                           , string_type  const& name
                           , array_type   const& args
                           , context_type const& context
                           , options_type const& options
                           ) const {
        process_filter const processor = { *this, value, name, args, context, options };
        return detail::find_by_index(*this, filters_.definition, filters_.index, name, processor);
    }

    template <char_type Delimiter>
    array_type split_argument( value_type   const& argument
                             , context_type const& context
                             , options_type const& options
                             ) const {
        typedef char_separator<char_type> separator_type;
        typedef typename value_type::token_type token_type;
        typedef tokenizer<separator_type, iterator_type, token_type> tokenizer_type;

        BOOST_ASSERT(argument.is_literal());
        token_type const& source = argument.token();
        static char_type const delimiter[2] = { Delimiter, 0 };
        separator_type const separator(delimiter, 0, keep_empty_tokens);
        tokenizer_type const tokenizer(source.first, source.second, separator);

        array_type args;
        match_type match;

        BOOST_FOREACH(token_type const& token, tokenizer) {
            if (!std::distance(token.first, token.second)) {
                args.push_back(value_type());
            }
            else if (xpressive::regex_match(token.first,
                     token.second, match, expression)) {
                try {
                    args.push_back(evaluate(match, context, options));
                }
                catch (missing_variable const& e) {
                    string_type const string(token.first, token.second);

                    if (this->template convert<char>(string) != e.name) {
                        throw_exception(e);
                    }

                    // A missing variable means an embedded
                    // argument was meant as a string literal.
                    value_type value = string;
                    value.token(match[0]);
                    args.push_back(value);
                }
            }
        }

        return args;
    }

    template <class T>
    string_type extract_string(T const& from) const {
        // TODO: Escape sequences, etc.
        // Handles "string" or 'string'.
        string_type const string = from.str();
        return string.substr(1, string.size() - 2);
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
        typedef file_template<char_type, engine_type> file_template_type;
        std::string const filepath_ = this->template convert<char>(filepath);
        file_template_type(filepath_).render(stream, context, options);
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
        using namespace detail;
        // If there's only _one_ tag, xpressive will not
        // "nest" the match, so we use it directly instead.
        match_type const& tag = tags_type::size::value == 1 ? match : get_nested<1>(match);
        tag_renderer<this_type> const renderer = { *this, stream, tag, context, options };
        find_by_index(*this, tags_.definition, tags_.index, tag.regex_id(), renderer);
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

    value_type apply_pipe( value_type   const& value
                         , match_type   const& pipe
                         , context_type const& context
                         , options_type const& options
                         ) const {
        value_type result = value;

        BOOST_FOREACH(match_type const& filter, pipe.nested_results()) {
            match_type const& name = detail::get_nested<1>(filter);
            match_type const& arg  = detail::get_nested<2>(filter);

            array_type const args = !arg ? array_type()
                : array_type(1, evaluate(arg, context, options));
            result = apply_filter(result, name.str(), args, context, options);
        }

        return result;
    }

    value_type evaluate_literal( match_type   const& match
                               , context_type const& context
                               , options_type const& options
                               ) const {
        value_type value;
        BOOST_ASSERT(match == this->literal);
        string_type const string = match.str();
        match_type const& literal = detail::get_nested<1>(match);

        if (literal == none_literal) {
            value = value_type();
            value.token(literal[0]);
        }
        else if (literal == boolean_literal) {
            value = typename value_type::boolean_type
                (string == detail::text("True"));
            value.token(literal[0]);
        }
        else if (literal == number_literal) {
            value = lexical_cast<typename
                value_type::number_type>(string);
            value.token(literal[0]);
        }
        else if (literal == string_literal) {
            value = extract_string(literal);
            // Adjust the token by trimming the quotes.
            value.token(std::make_pair(literal[0].first + 1,
                                       literal[0].second - 1));
        }
        else if (literal == variable_literal) {
            if (optional<value_type const&> const
                    variable = detail::find_value(string, context)) {
                value = *variable;
                value.token(literal[0]);
            }
            else {
                throw_exception(missing_variable(
                    this->template convert<char>(string)));
            }
        }
        else {
            throw_exception(std::logic_error("invalid literal"));
        }

        return value;
    }

    value_type evaluate( match_type   const& match
                       , context_type const& context
                       , options_type const& options
                       ) const {
        // Handle singly-nested expressions.
        /*if (match == expression) {
            match_type const& nested = detail::get_nested<1>(match);
            return evaluate(nested, context, options);
        }*/

        // First, evaluate the first segment, which is
        // always present, and which is always a literal.
        match_type const& literal = detail::get_nested<1>(match);
        value_type value = evaluate_literal(literal, context, options);
        size_type i = 0;

        BOOST_FOREACH( match_type const& segment
                     , match.nested_results()
                     ) {
            if (!i++) continue; // Skip the first segment (the literal.)
            value_type attribute;
            match_type const& nested = detail::get_nested<1>(segment);

            if (segment == subscription) { // i.e. value [ attribute ]
                attribute = evaluate(nested, context, options);
            }
            else if (segment == attribution) { // i.e. value.attribute
                attribute = nested.str();
            }
            else {
                throw_exception(std::logic_error("invalid expression"));
            }

            typename value_type::const_iterator const
                it = value.find_attribute(attribute);

            if (it == value.end()) {
                std::string const name = this->template
                    convert<char>(attribute.to_string());
                throw_exception(missing_attribute(name));
            }

            value = *it;
        }

        return value;
    }

  private:

    struct process_filter {
        this_type    const& self;
        value_type   const& value_;
        string_type  const& name_;
        array_type   const& args_;
        context_type const& context_;
        options_type const& options_;

        typedef value_type result_type;

        template <class Filter>
        value_type operator ()(Filter const& filter) const {
            options_type& options = const_cast<options_type&>(options_);
            return filter.process(value_, self, name_, context_, args_, options);
        }
    };

    struct append_filter {
        this_type& self;

        template <class Filter>
        void operator()(Filter const& filter) const {
            self.filters_.index.push_back(filter.name());
        }
    };

  public:

    string_type const newline;
    string_type const brace_open;
    string_type const brace_close;
    string_type const block_open;
    string_type const block_close;
    string_type const comment_open;
    string_type const comment_close;
    string_type const variable_open;
    string_type const variable_close;

  public:

    regex_type tag, text, block, skipper;
    regex_type identifier, filter, pipe;
    regex_type subscription, attribution, expression;
    regex_type string_literal, number_literal;
    regex_type none_literal, boolean_literal;
    regex_type variable_literal, literal;
    value_type const default_value;

  private:

    tag_sequence_type    tags_;
    filter_sequence_type filters_;

}; // definition

}; // engine

}}} // namespace ajg::synthesis::django

#endif // AJG_SYNTHESIS_ENGINES_DJANGO_ENGINE_HPP_INCLUDED
