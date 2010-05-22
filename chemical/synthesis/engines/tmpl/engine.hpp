
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ENGINES_TMPL_ENGINE_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ENGINES_TMPL_ENGINE_HPP_INCLUDED

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

#include <chemical/synthesis/template.hpp>
#include <chemical/synthesis/engines/detail.hpp>
#include <chemical/synthesis/engines/exceptions.hpp>
#include <chemical/synthesis/engines/base_definition.hpp>
#include <chemical/synthesis/engines/tmpl/value.hpp>
#include <chemical/synthesis/engines/tmpl/library.hpp>

namespace chemical {
namespace synthesis {
namespace tmpl {

enum tag_mode
    { xml
    , html
    , loose
    };

using detail::operator ==;

template < class Library = tmpl::default_library
         , bool CaseSensitive = false
         , bool ShortcutSyntax = true
         // TODO: Implement tag_mode.
         , tag_mode TagMode = loose
         >
struct engine : detail::nonconstructible {

typedef engine engine_type;

template <class BidirectionalIterator>
struct definition : base_definition< BidirectionalIterator
                                   , definition<BidirectionalIterator>
                                   > {
  public:
    // Constants:

    BOOST_STATIC_CONSTANT(bool, case_sensitive = CaseSensitive);
    BOOST_STATIC_CONSTANT(bool, shortcut_syntax = ShortcutSyntax);
    BOOST_STATIC_CONSTANT(tmpl::tag_mode, tag_mode = TagMode);

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

    typedef typename mpl::if_c
        < CaseSensitive, std::less<string_type>
        , detail::insensitive_less<string_type> >::type  less_type;
    typedef Library                                      library_type;
    typedef tmpl::value<char_type>                       value_type;
    typedef mpl::void_                                   options_type;
    typedef library_type                                 tags_type;
    typedef std::map<string_type, value_type, less_type> context_type;
    typedef std::vector<value_type>                      array_type;
    typedef typename detail::define_sequence
        <this_type, tags_type>::type                     tag_definitions_type;

  public:

    definition()
        : tag_open      (detail::text("<"))
        , tag_close     (detail::text(">"))
        , tag_finish    (detail::text("/"))
        , tag_prefix    (detail::text("TMPL_"))
        , tag_attribute (detail::text("NAME"))
        , alt_open      (detail::text("<!--"))
        , alt_close     (detail::text("-->"))
        , default_value (detail::text("")) {
        using namespace xpressive;
//
// common grammar
////////////////////////////////////////////////////////////////////////////////

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
        regex_type const prefix
        // We want to skip essentially anything that is not a tmpl tag or comment.
            = *_s >> !as_xpr(tag_finish) >> *_s >> icase(tag_prefix)
            ;
        skipper
            = tag_open >> prefix >> +(~before(tag_close) >> _) >> tag_close
            | alt_open >> prefix >> +(~before(alt_close) >> _) >> alt_close
            ;

        initialize_grammar();
        fusion::for_each(tags_, detail::construct
            <detail::element_initializer<this_type> >(*this));
        detail::append_tags<this_type, tags_type::size::value>(*this);
    }

  public:

    string_type attribute_string(match_type const& attr) const {
        if (attr == attribute) {
            match_type const& attr_ = detail::get_nested<1>(attr);
            return attribute_string(attr_);
        }
        else if (attr == name_attribute) {
            match_type const& attr_ = attr(attribute);
            return attribute_string(attr_);
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
        string_type const name = attribute_string(attr);

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
        typedef file_template<char_type, engine_type> file_template_type;
        std::string const filepath_ = convert<char>(filepath);
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
        find_by_index(*this, tags_, tag_ids_, tag.regex_id(), renderer);
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

  public:

    string_type const tag_open;
    string_type const tag_close;
    string_type const tag_finish;
    string_type const tag_prefix;
    string_type const tag_attribute;
    string_type const alt_open;
    string_type const alt_close;

  public:
  
    regex_type tag, text, block, skipper;
    regex_type name, attribute, name_attribute;
    regex_type plain_attribute, quoted_attribute;
    value_type const default_value;

  private:

    tag_definitions_type  tags_;
    std::vector<id_type>  tag_ids_;

    template <class Engine, typename Engine::tags_type::size::value_type Size>
    friend struct detail::append_tags;

}; // definition

}; // engine

}}} // namespace chemical::synthesis::tmpl

#endif // CHEMICAL_SYNTHESIS_ENGINES_TMPL_ENGINE_HPP_INCLUDED
