//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_TMPL_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_TMPL_ENGINE_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <numeric>
#include <algorithm>

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

#include <ajg/synth/templates.hpp>
#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/engines/exceptions.hpp>
#include <ajg/synth/engines/base_engine.hpp>
#include <ajg/synth/engines/tmpl/value.hpp>
#include <ajg/synth/engines/tmpl/builtin_tags.hpp>

namespace ajg {
namespace synth {
namespace tmpl {

enum tag_mode
    { xml
    , html
    , loose
    };

using detail::operator ==;

template < bool CaseSensitive   = false
         , bool ShortcutSyntax  = true
         , bool LoopVariables   = true
         , bool GlobalVariables = false
         , tag_mode TagMode     = loose // TODO: Implement.
         >
struct engine : base_engine {

template <class BidirectionalIterator>
struct definition : base_engine::definition<BidirectionalIterator, definition<BidirectionalIterator> > {
  public:

    BOOST_STATIC_CONSTANT(bool, case_sensitive   = CaseSensitive);
    BOOST_STATIC_CONSTANT(bool, shortcut_syntax  = ShortcutSyntax);
    BOOST_STATIC_CONSTANT(bool, loop_variables   = LoopVariables);
    BOOST_STATIC_CONSTANT(bool, global_variables = GlobalVariables);
    BOOST_STATIC_CONSTANT(tmpl::tag_mode, tag_mode = TagMode);

  public:

    typedef definition                                                  this_type;
    typedef base_engine::definition<BidirectionalIterator, this_type>   base_type;

    typedef typename base_type::id_type         id_type;
    typedef typename base_type::boolean_type    boolean_type;
    typedef typename base_type::size_type       size_type;
    typedef typename base_type::char_type       char_type;
    typedef typename base_type::match_type      match_type;
    typedef typename base_type::regex_type      regex_type;
    typedef typename base_type::frame_type      frame_type;
    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stream_type     stream_type;
    typedef typename base_type::iterator_type   iterator_type;
    typedef typename base_type::definition_type definition_type;

    typedef typename mpl::if_c
        < CaseSensitive
        , std::less<string_type>
        , detail::insensitive_less<string_type>
        >::type                                          less_type;
    typedef builtin_tags<this_type>                      builtin_tags_type;
    typedef tmpl::value<char_type>                       value_type;
    typedef mpl::void_                                   options_type;
    typedef std::map<string_type, value_type, less_type> context_type;
    typedef std::vector<value_type>                      sequence_type;
    typedef typename value_type::traits_type             traits_type;

  public:

    definition()
        : tag_open      (traits_type::literal("<"))
        , tag_close     (traits_type::literal(">"))
        , tag_finish    (traits_type::literal("/"))
        , tag_prefix    (traits_type::literal("TMPL_"))
        , tag_attribute (traits_type::literal("NAME"))
        , alt_open      (traits_type::literal("<!--"))
        , alt_close     (traits_type::literal("-->"))
        , default_value (traits_type::literal("")) {
        using namespace xpressive;
//
// common grammar
////////////////////////////////////////////////////////////////////////////////////////////////////

        name
            // @see http://www.w3.org/TR/2000/WD-xml-2e-20000814#NT-Name
            = (alpha | '_' | ':') >> *(_w | (set= '_', ':', '-', '.'))
            ;
        quoted_attribute
            = '"'  >> *~as_xpr('"')  >> '"'
            | '\'' >> *~as_xpr('\'') >> '\''
            ;
        plain_attribute
            = name
            ;
        attribute
            = quoted_attribute
            | plain_attribute
            ;
        regex_type const tag_attribute_equals
            = icase(tag_attribute) >> *_s >> '=' >> *_s
            ;
        shortcut_syntax
            ? name_attribute = !tag_attribute_equals >> attribute
            : name_attribute = tag_attribute_equals >> attribute
            ;
        escape_attribute
            = icase("ESCAPE")  >> *_s >> '=' >> *_s >> attribute
            ;
        default_attribute
            = icase("DEFAULT") >> *_s >> '=' >> *_s >> attribute
            ;
        extended_attribute
            = escape_attribute
            | default_attribute
            | name_attribute
            ;
        regex_type const prefix
        // We want to skip essentially anything that is not a tmpl tag or comment.
            = *_s >> !as_xpr(tag_finish) >> *_s >> icase(tag_prefix)
            ;
        skipper
            = tag_open >> prefix >> +(~before(tag_close) >> _) >> tag_close
            | alt_open >> prefix >> +(~before(alt_close) >> _) >> alt_close
            ;

        this->initialize_grammar();
        builtin_tags_.initialize(*this);
    }

  public:

    string_type extract_attribute(match_type const& attr) const {
        if (attr == attribute) {
            match_type const& attr_ = detail::get_nested<1>(attr);
            return extract_attribute(attr_);
        }
        else if (attr == name_attribute) {
            match_type const& attr_ = attr(attribute);
            return extract_attribute(attr_);
        }
        else if (attr == quoted_attribute) {
            // TODO: Escape sequences, etc.
            // Handles "string" or 'string'.
            string_type const string = attr.str();
            return string.substr(1, string.size() - 2);
        }
        else if (attr == plain_attribute) {
            return attr.str();
        }
        else if (!attr) {
            throw_exception(std::logic_error("missing attribute"));
        }
        else {
            throw_exception(std::logic_error("invalid attribute"));
        }
    }

    value_type evaluate_attribute( match_type   const& attr
                                 , context_type const& context
                                 , options_type const& options
                                 ) const {
        string_type const name = extract_attribute(attr);

        if (optional<value_type const&> const
                variable = detail::find_value(name, context)) {
            return *variable;
        }
        else {
            return default_value;
        }
    }

    value_type evaluate( match_type   const& match
                       , context_type const& context
                       , options_type const& options
                       ) const {
        match_type const& attr = match(attribute);
        return evaluate_attribute(attr, context, options);
    }

    void render( stream_type&        stream
               , frame_type   const& frame
               , context_type const& context
               , options_type const& options
               ) const {
        render_block(stream, frame, context, options);
    }

    void render_file( stream_type&        stream
                    , string_type  const& filepath
                    , context_type const& context
                    , options_type const& options
                    ) const {
        typedef file_template<char_type, engine> file_template_type;
        file_template_type(filepath).render(stream, context, options);
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

    struct attributes {
        enum escape_mode { none, html, url, js };

        string_type           name;
        optional<string_type> default_;
        optional<escape_mode> escape;
    };

    // TODO: Throw synth exceptions when possible.
    attributes parse_attributes(match_type const& match) const {
        optional<typename attributes::escape_mode> escape;
        optional<string_type> name;
        optional<string_type> default_;

        BOOST_FOREACH(match_type const& nested, match.nested_results()) {
            match_type const& attr  = get_nested<A>(nested);
            match_type const& value = attr(this->attribute);

            if (attr == name_attribute) {
                if (name) throw_exception(std::logic_error("duplicate variable name"));
                else name = this->extract_attribute(value);
            }
            else if (attr == default_attribute) {
                if (default_) throw_exception(std::logic_error("duplicate default value"));
                else default_ = this->extract_attribute(value);
            }
            else if (attr == escape_attribute) {
                if (escape) {
                    throw_exception(std::logic_error("duplicate escape mode"));
                }
                else {
                    string_type const mode = boost::algorithm::to_lower_copy(value.str());

                         if (mode == traits_type::literal("none")
                          || mode == traits_type::literal("0"))   escape = attributes::none;
                    else if (mode == traits_type::literal("html")
                          || mode == traits_type::literal("1"))   escape = attributes::html;
                    else if (mode == traits_type::literal("url")) escape = attributes::url;
                    else if (mode == traits_type::literal("js"))  escape = attributes::js;
                    else {
                        throw_exception(std::invalid_argument("invalid escape mode"));
                    }
                }
            }
            else {
                throw_exception(std::invalid_argument("invalid attribute"));
            }
        }

        if (!name) throw_exception(std::logic_error("missing variable name"));
        attributes const attrs = {*name, default_, escape};
        return attrs;
    }

  public:

    string_type const tag_open;
    string_type const tag_close;
    string_type const tag_finish;
    string_type const tag_prefix;
    string_type const tag_attribute;
    string_type const alt_open;
    string_type const alt_close;

  public:

    regex_type tag;
    regex_type text;
    regex_type block;
    regex_type name;
    regex_type skipper;
    regex_type attribute;
    regex_type plain_attribute;
    regex_type quoted_attribute;
    regex_type name_attribute;
    regex_type escape_attribute;
    regex_type default_attribute;
    regex_type extended_attribute;
    value_type const default_value;

  private:


  private:

    builtin_tags_type builtin_tags_;

}; // definition

}; // engine

}}} // namespace ajg::synth::tmpl

#endif // AJG_SYNTH_ENGINES_TMPL_ENGINE_HPP_INCLUDED
