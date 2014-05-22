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
#include <ajg/synth/detail/text.hpp>
#include <ajg/synth/engines/base_engine.hpp>
#include <ajg/synth/engines/django/value.hpp>
#include <ajg/synth/engines/django/options.hpp>
#include <ajg/synth/engines/django/builtin_tags.hpp>
#include <ajg/synth/engines/django/builtin_filters.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace django {

template <class Traits, class Options = options<value<Traits> > >
struct engine : base_engine<Options> {
  public:

    typedef engine                                                              engine_type;
    typedef Traits                                                              traits_type;

    typedef typename traits_type::none_type                                     none_type;
    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;
    typedef typename traits_type::symbols_type                                  symbols_type;

    typedef typename engine_type::value_type                                    value_type;
    typedef typename engine_type::options_type                                  options_type;

    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::names_type                                   names_type;
    typedef typename options_type::arguments_type                               arguments_type;

    typedef typename value_type::behavior_type                                  behavior_type;
    typedef typename value_type::sequence_type                                  sequence_type;

  private:

    template <class K> friend struct django::builtin_tags;
    template <class K> friend struct django::builtin_filters;

  public:

    template <class Iterator>
    struct kernel;

}; // engine


template <class Options>
struct state {
  public:

    typedef Options                                                             options_type;

  private:

    typedef typename options_type::loader_type                                  loader_type;
    typedef typename options_type::loaders_type                                 loaders_type;
    typedef typename options_type::tag_type                                     tag_type;
    typedef typename options_type::filter_type                                  filter_type;
    typedef typename options_type::library_type                                 library_type;
    typedef typename options_type::renderer_type                                renderer_type;
    typedef typename options_type::segment_type                                 segment_type;
    typedef typename options_type::entry_type                                   entry_type;
    typedef typename options_type::tags_type                                    tags_type;
    typedef typename options_type::filters_type                                 filters_type;
    typedef typename options_type::traits_type                                  traits_type;
    typedef typename options_type::libraries_type                               libraries_type;
    typedef typename options_type::renderers_type                               renderers_type;
    typedef typename options_type::segments_type                                segments_type;
    typedef typename options_type::entries_type                                 entries_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::names_type                                    names_type;
    typedef typename traits_type::symbols_type                                  symbols_type;
    typedef detail::text<string_type>                                           text;

  public:

    // inline explicit state() {}

    inline explicit state(options_type const& options)
        : loaders_(options.loaders), loaded_libraries_(options.libraries) {}

  public:

    /*
    inline filter_type get_filter(string_type const& name) const {
        if (boost::optional<filter_type> const& filter = detail::find(name, this->loaded_filters_)) {
            return *filter;
        }
        AJG_SYNTH_THROW(missing_filter(text::narrow(name)));
    }

    inline tag_type get_tag(string_type const& name) const {
        if (boost::optional<tag_type> const& tag = detail::find(name, this->loaded_tags_)) {
            return *tag;
        }
        AJG_SYNTH_THROW(missing_tag(text::narrow(name)));
    }
    */

    inline boost::optional<filter_type> get_filter(string_type const& name) const {
        return detail::find(name, this->loaded_filters_);
    }

    inline boost::optional<tag_type> get_tag(string_type const& name) const {
        return detail::find(name, this->loaded_tags_);
    }

    inline boost::optional<renderer_type> get_renderer(size_type const position) const {
        SHOW(position);
        return detail::find(position, this->parsed_renderers_);
    }

    inline void set_renderer(size_type const position, renderer_type const& renderer) {
        SHOW(position);
        this->parsed_renderers_[position] = renderer;
    }

    void load_library(string_type const& library_name, names_type const& names = names_type()) {
        SHOW(library_name);
        library_type library = this->loaded_libraries_[library_name];

        if (!library) {
            BOOST_FOREACH(loader_type const& loader, this->loaders_) {
                if ((library = loader->load_library(library_name))) {
                    this->loaded_libraries_[library_name] = library;
                    break;
                }
            }
        }

        if (!library) {
            AJG_SYNTH_THROW(missing_library(text::narrow(library_name)));
        }
        else if (!names.empty()) {
            BOOST_FOREACH(string_type const& name, names) {
                if (name.empty()) {
                    AJG_SYNTH_THROW(std::invalid_argument("empty component name"));
                }
                tag_type    const& tag    = library->get_tag(name);
                filter_type const& filter = library->get_filter(name);

                if (!tag.first && !filter) {
                    AJG_SYNTH_THROW(missing_key(text::narrow(name)));
                }
                if (tag.first) {
                    this->loaded_tags_[name] = tag;
                }
                if (filter) {
                    this->loaded_filters_[name] = filter;
                }
            }
        }
        else {
            BOOST_FOREACH(string_type const& name, library->list_tags()) {
                // if (tag_type const& tag = library->get_tag(name)) {
                tag_type const& tag = library->get_tag(name); if (tag.first) {
                    this->loaded_tags_[name] = tag;
                }
                else {
                    AJG_SYNTH_THROW(missing_tag(text::narrow(name)));
                }
            }
            BOOST_FOREACH(string_type const& name, library->list_filters()) {
                if (filter_type const& filter = library->get_filter(name)) {
                    this->loaded_filters_[name] = filter;
                }
                else {
                    AJG_SYNTH_THROW(missing_filter(text::narrow(name)));
                }
            }
        }
    }

  private:

    template <class K> friend struct django::builtin_tags;
    template <class K> friend struct django::builtin_filters;

  private:

    loaders_type   loaders_;
    tags_type      loaded_tags_;
    filters_type   loaded_filters_;
    libraries_type loaded_libraries_;
    renderers_type parsed_renderers_;

    std::vector<string_type> library_tag_args_;
    entries_type             library_tag_entries_;
    boolean_type             library_tag_continue_;
};

template <class Traits, class Options>
template <class Iterator>
struct engine<Traits, Options>::kernel : base_engine<Options>::AJG_SYNTH_TEMPLATE kernel<Iterator, state<Options> > {
  public:

    typedef kernel                                                              kernel_type;
    typedef Iterator                                                            iterator_type;
    typedef engine                                                              engine_type;
    typedef typename kernel_type::result_type                                   result_type;

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
        restricted_identifier
            = identifier[ x::check(not_in(keywords_)) ]
            ;
        unreserved_identifier
            = restricted_identifier[ x::check(not_in(reserved_)) ]
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
            = !(x::set = '-','+') >> +_d >> !('.' >> +_d) >> !('e' >> +_d)
            ;
        string_literal
            = '"'  >> *~as_xpr('"')  >> '"'
            | '\'' >> *~as_xpr('\'') >> '\''
            ;
        super_literal
            = word("block.super")
            ;
        variable_literal
            = restricted_identifier
            ;
        literal
            = none_literal
            | boolean_literal
            | number_literal
            | string_literal
            | super_literal
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
        this->skipper
            = block_open
            | block_close
            | comment_open
            | comment_close
            | variable_open
            | variable_close
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

    struct not_in {
        symbols_type const& symbols;
        explicit not_in(symbols_type const& symbols) : symbols(symbols) {}

        boolean_type operator ()(typename match_type::value_type const& match) const {
            return this->symbols.find(match.str()) == this->symbols.end();
        }
    };

  private:

    using kernel_type::base_type::is;

  public: // TODO: Make protected, and make builtin_tags/builtin_filters friends.

    inline regex_type marker  (string_type const& s, string_type const& name) { return as_xpr((this->markers[name] = s)); }
    inline regex_type word    (string_type const s) { return as_xpr(s) >> _b; }
    inline regex_type word    (char const* const s) { return as_xpr(s) >> _b; }
    inline regex_type op      (char const* const s) { return this->word(*this->keywords_.insert(text::literal(s)).first); }
    inline regex_type keyword (char const* const s) { return this->word(*this->keywords_.insert(text::literal(s)).first) >> *_s; }
    inline regex_type reserved(char const* const s) { return this->word(*this->reserved_.insert(text::literal(s)).first) >> *_s; }

    sequence_type split_argument( result_type  const& // result
                                , value_type   const& argument
                                , context_type const& context
                                , options_type const& options
                                , char_type    const  delimiter
                                ) const {
        typedef typename string_type::const_iterator                                string_iterator_type;
        typedef kernel<string_iterator_type>                                        string_kernel_type;
        typedef typename string_kernel_type::match_type                             string_match_type;
        typedef typename string_kernel_type::result_type                            string_result_type;

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
        string_match_type match;
        sequence_type sequence;

        BOOST_FOREACH(string_type const& t, tokenizer) {
            if (std::distance(t.begin(), t.end()) == 0) {
                sequence.push_back(value_type(none_type()));
            }
            else if (x::regex_match(t.begin(), t.end(), match, string_kernel.chain)) {
                try {
                    string_result_type const result(options);
                    sequence.push_back(string_kernel.evaluate_chain(result, match, context, options));
                }
                catch (missing_variable const& e) {
                    string_type const string(t.begin(), t.end());

                    if (text::narrow(string) != e.name) {
                        throw;
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

    path_type extract_path(match_type const& match) const {
        return traits_type::to_path(this->extract_string(match));
    }

    string_type extract_string(match_type const& match) const {
        BOOST_ASSERT(is(match, this->string_literal));
        return detail::text<string_type>::unquote(match.str());
    }

    names_type extract_names(match_type const& match) const {
        names_type names;
        BOOST_FOREACH(match_type const& name, this->select_nested(match, this->name)) {
            names.push_back(name[id].str());
        }
        return names;
    }

    void render( ostream_type&       ostream
               , result_type  const& result
               , context_type const& context
               , options_type const& options
               ) const {
        this->render_block(ostream, result, this->get_match(result), context, options);
    }

    void render_path( ostream_type&       ostream
                    , result_type  const& result
                    , path_type    const& path
                    , context_type const& context
                    , options_type const& options
                    ) const {
        templates::path_template<engine_type> const t(path, options.directories, options);
        return t.render_to_stream(ostream, context, options);
    }

    void render_plain( ostream_type&       ostream
                     , result_type  const& result
                     , match_type   const& plain
                     , context_type const& context
                     , options_type const& options
                     ) const {
        ostream << plain.str();
    }

    void render_block( ostream_type&       ostream
                     , result_type  const& result
                     , match_type   const& block
                     , context_type const& context
                     , options_type const& options
                     ) const {
        BOOST_FOREACH(match_type const& nested, block.nested_results()) {
            this->render_match(ostream, result, nested, context, options);
        }
    }

    void render_tag( ostream_type&       ostream
                   , result_type  const& result
                   , match_type   const& match
                   , context_type const& context
                   , options_type const& options
                   ) const {
        match_type const& m  = this->unnest(match);
        id_type    const  id = m.regex_id();

        if (typename builtin_tags_type::tag_type const tag = builtin_tags_.get(id)) {
            tag(*this, result, m, context, options, ostream);
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("missing built-in tag"));
        }
    }

    void render_match( ostream_type&       ostream
                     , result_type  const& result
                     , match_type   const& match
                     , context_type const& context
                     , options_type const& options
                     ) const {
             if (is(match, this->plain)) this->render_plain(ostream, result, match, context, options);
        else if (is(match, this->block)) this->render_block(ostream, result, match, context, options);
        else if (is(match, this->tag))   this->render_tag(ostream, result, match, context, options);
        else AJG_SYNTH_THROW(std::logic_error("invalid template state"));
    }

    value_type apply_filters( value_type   const& value
                            , result_type  const& result
                            , match_type   const& match
                            , context_type const& context
                            , options_type const& options
                            ) const {
        value_type v = value;

        BOOST_FOREACH(match_type const& filter, this->select_nested(match, this->filter)) {
            BOOST_ASSERT(is(filter, this->filter));
            string_type const& name  = filter(this->name)[id].str();
            match_type  const& chain = filter(this->chain);

            arguments_type arguments;
            if (chain) {
                arguments.first.push_back(this->evaluate_chain(result, chain, context, options));
            }
            v = this->apply_filter(v, result, name, arguments, context, options);
        }

        return v;
    }

    value_type apply_filter( value_type     const& value
                           , result_type    const& result
                           , string_type    const& name
                           , arguments_type const& arguments
                           , context_type   const& context
                           , options_type   const& options
                           ) const {
        // Let library filters override built-in ones.
        if (boost::optional<typename options_type::filter_type> const& filter = this->get_state(result).get_filter(name)) {
            return (*filter)(value, arguments, const_cast<context_type&>(context), const_cast<options_type&>(options));
        }
        else if (typename builtin_filters_type::filter_type const filter = builtin_filters_type::get(name)) {
            return filter(*this, result, value, arguments, context, options);
        }
        else {
            AJG_SYNTH_THROW(missing_filter(text::narrow(name)));
        }
    }

    value_type evaluate( result_type  const& result
                       , match_type   const& match
                       , context_type const& context
                       , options_type const& options
                       ) const {

        try {
            value_type const& value = this->evaluate_expression(result, match(this->expression), context, options);
            return this->apply_filters(value, result, match, context, options);
        }
        catch (missing_variable  const&) { return options.default_value; }
        catch (missing_attribute const&) { return options.default_value; }
    }

    arguments_type evaluate_arguments( result_type   const& result
                                     , match_type    const& match
                                     , context_type  const& context
                                     , options_type  const& options
                                     ) const {
        arguments_type arguments;
        BOOST_FOREACH(match_type const& arg, this->select_nested(match, this->argument)) {
            value_type const& value = this->evaluate(result, arg(this->value), context, options);
            if (match_type const& name = arg(this->restricted_identifier)) {
                arguments.second[name.str()] = value; // Keyword argument.
            }
            else {
                arguments.first.push_back(value); // Positional argument.
            }
        }
        return arguments;
    }

    value_type evaluate_literal( result_type  const& result
                               , match_type   const& match
                               , context_type const& context
                               , options_type const& options
                               ) const {
        BOOST_ASSERT(is(match, this->literal));
        match_type  const& literal = this->unnest(match);
        string_type const  string  = match.str();
        string_type const  token   = literal[0];

        if (is(literal, this->none_literal)) {
            return value_type(none_type()).token(token);
        }
        else if (is(literal, this->boolean_literal)) {
            match_type const& boolean = this->unnest(literal);

            if (is(boolean, this->true_literal)) {
                return value_type(boolean_type(true)).token(token);
            }
            else if (is(boolean, this->false_literal)) {
                return value_type(boolean_type(false)).token(token);
            }
            else {
                AJG_SYNTH_THROW(std::logic_error("invalid boolean literal"));
            }
        }
        else if (is(literal, this->number_literal)) {
            double const d = (std::atof)(text::narrow(string).c_str());
            return value_type(static_cast<floating_type>(d)).token(token);
        }
        else if (is(literal, this->string_literal)) {
            return value_type(extract_string(literal)).token(token);
        }
        else if (is(literal, this->super_literal)) {
            return options.get_base_block();
        }
        else if (is(literal, this->variable_literal)) {
            if (optional<value_type> const& variable = detail::find(string, context)) {
                return variable->copy().token(token);
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

    value_type evaluate_expression( result_type  const& result
                                  , match_type   const& match
                                  , context_type const& context
                                  , options_type const& options
                                  ) const {
        match_type const& expr = this->unnest(match);

        if (is(expr, this->unary_expression)) {
            return this->evaluate_unary(result, expr, context, options);
        }
        else if (is(expr, this->binary_expression)) {
            return this->evaluate_binary(result, expr, context, options);
        }
        else if (is(expr, this->nested_expression)) {
            match_type const& nested = expr(this->expression);
            return this->evaluate_expression(result, nested, context, options);
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("invalid expression"));
        }
    }

    value_type evaluate_unary( result_type  const& result
                             , match_type   const& match
                             , context_type const& context
                             , options_type const& options
                             ) const {
        BOOST_ASSERT(is(match, this->unary_expression));
        string_type const& op      = match(unary_operator).str();
        match_type  const& operand = match(expression);

        if (op == text::literal("not")) {
            return !evaluate_expression(result, operand, context, options);
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("invalid unary operator"));
        }
    }

    value_type evaluate_binary( result_type  const& result
                              , match_type   const& match
                              , context_type const& context
                              , options_type const& options
                              ) const {
        BOOST_ASSERT(is(match, this->binary_expression));
        match_type const& chain = match(this->chain);
        value_type value = this->evaluate_chain(result, chain, context, options);
        string_type op;

        BOOST_FOREACH(match_type const& segment, detail::drop(match.nested_results(), 1)) {
            if (is(segment, this->binary_operator)) {
                op = segment.str();
            }
            else if (!(is(segment, this->expression))) {
                AJG_SYNTH_THROW(std::logic_error("invalid binary expression"));
            }
            else if (op == text::literal("==")) {
                value = value == this->evaluate_expression(result, segment, context, options);
            }
            else if (op == text::literal("!=")) {
                value = value != this->evaluate_expression(result, segment, context, options);
            }
            else if (op == text::literal("<")) {
                value = value < this->evaluate_expression(result, segment, context, options);
            }
            else if (op == text::literal(">")) {
                value = value > this->evaluate_expression(result, segment, context, options);
            }
            else if (op == text::literal("<=")) {
                value = value <= this->evaluate_expression(result, segment, context, options);
            }
            else if (op == text::literal(">=")) {
                value = value >= this->evaluate_expression(result, segment, context, options);
            }
            else if (op == text::literal("and")) {
                value = value ? this->evaluate_expression(result, segment, context, options) : value;
            }
            else if (op == text::literal("or")) {
                value = value ? value : this->evaluate_expression(result, segment, context, options);
            }
            else if (op == text::literal("in")) {
                value_type const elements = this->evaluate_expression(result, segment, context, options);
                value = elements.contains(value);
            }
            else if (text::begins_with(op, text::literal("not"))
                  && text::ends_with(op, text::literal("in"))) {
                value_type const elements = this->evaluate_expression(result, segment, context, options);
                value = !elements.contains(value);
            }
            else {
                AJG_SYNTH_THROW(std::logic_error("invalid binary operator"));
            }
        }

        return value;
    }

    value_type evaluate_link( result_type  const& result
                            , match_type   const& match
                            , context_type const& context
                            , options_type const& options
                            ) const {
        match_type const& link = this->unnest(match);

        if (is(link, this->subscript_link)) { // i.e. value[attribute]
            return this->evaluate(result, link(this->expression), context, options);
        }
        else if (is(link, this->attribute_link)) { // i.e. value.attribute
            return string_type(link(this->identifier).str());
        }
        else {
            AJG_SYNTH_THROW(std::logic_error("invalid link"));
        }
    }

    value_type evaluate_chain( result_type  const& result
                             , match_type   const& match
                             , context_type const& context
                             , options_type const& options
                             ) const {
        BOOST_ASSERT(is(match, this->chain));
        match_type const& lit = match(this->literal);
        value_type value = this->evaluate_literal(result, lit, context, options);

        BOOST_FOREACH(match_type const& link, this->select_nested(match, this->link)) {
            value_type const attribute = this->evaluate_link(result, link, context, options);
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
        return boost::none;
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
    regex_type number_literal;
    regex_type string_literal;
    regex_type super_literal;
    regex_type variable_literal;
    regex_type literal;
    regex_type polyadic_tag; // monadic_tag, dyadic_tag; // library_tag;

    string_regex_type html_namechar;
    string_regex_type html_whitespace;
    string_regex_type html_tag;

  private:

    builtin_tags_type builtin_tags_;

}; // kernel

}}}} // namespace ajg::synth::engines::django

#endif // AJG_SYNTH_ENGINES_DJANGO_ENGINE_HPP_INCLUDED
