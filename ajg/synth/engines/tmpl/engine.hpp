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
#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/engines/base_engine.hpp>
#include <ajg/synth/engines/tmpl/value.hpp>
#include <ajg/synth/engines/tmpl/options.hpp>
#include <ajg/synth/engines/tmpl/builtin_tags.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace tmpl {

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

    typedef tmpl::value<traits_type>                                            value_type;
    typedef options<value_type>                                                 options_type;
    typedef typename mpl::if_c< options_type::case_sensitive
                              , std::less<string_type>
                              , detail::insensitive_less<string_type>
                              >::type                                           less_type;
    typedef std::map<string_type, value_type, less_type>                        context_type;

  private:

    template <class K> friend struct tmpl::builtin_tags;

    struct attributes {
        enum escape_mode { none, html, url, js };

        string_type           name;
        optional<string_type> default_;
        optional<escape_mode> escape;
    };

  public:

    template <class Iterator>
    struct kernel;

}; // engine

template <class Traits>
template <class Iterator>
struct engine<Traits>::kernel : base_engine<traits_type>::AJG_SYNTH_TEMPLATE kernel<Iterator>{
  public:

    typedef kernel                                                              kernel_type;
    typedef Iterator                                                            iterator_type;

  protected:

    typedef builtin_tags<kernel_type>                                           builtin_tags_type;
    typedef typename kernel_type::id_type                                       id_type;
    typedef typename kernel_type::regex_type                                    regex_type;
    typedef typename kernel_type::match_type                                    match_type;

  public:

    typedef match_type                                                          frame_type;
    typedef engine                                                              engine_type;

  private:

    template <class K> friend struct tmpl::builtin_tags;

  public:

    kernel()
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
        options_type::shortcut_syntax
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
        this->skipper
            = tag_open >> prefix >> +(~before(tag_close) >> _) >> tag_close
            | alt_open >> prefix >> +(~before(alt_close) >> _) >> alt_close
            ;

        this->initialize_grammar();
        builtin_tags_.initialize(*this);
    }

  public:

    path_type extract_path(match_type const& attr) const {
        return traits_type::to_path(this->extract_attribute(attr));
    }

    string_type extract_attribute(match_type const& attr) const {
        if (is(attr, this->attribute)) {
            match_type const& attr_ = detail::unnest(attr);
            return extract_attribute(attr_);
        }
        else if (is(attr, this->name_attribute)) {
            match_type const& attr_ = attr(attribute);
            return extract_attribute(attr_);
        }
        else if (is(attr, this->quoted_attribute)) {
            // TODO: Escape sequences, etc.
            // Handles "string" or 'string'.
            string_type const string = attr.str();
            return string.substr(1, string.size() - 2);
        }
        else if (is(attr, this->plain_attribute)) {
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

        if (optional<value_type> const& variable = detail::find(name, context)) {
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

    void render( ostream_type&       ostream
               , frame_type   const& frame
               , context_type const& context
               , options_type const& options
               ) const {
        render_block(ostream, frame, context, options);
    }

    void render_path( ostream_type&       ostream
                    , path_type    const& path
                    , context_type const& context
                    , options_type const& options
                    ) const {
        templates::path_template<engine_type> const t(path);
        return t.render_to_stream(ostream, context, options);
    }

    void render_text( ostream_type&       ostream
                    , match_type   const& text
                    , context_type const& context
                    , options_type const& options
                    ) const {
        ostream << text.str();
    }

    void render_block( ostream_type&       ostream
                     , match_type   const& block
                     , context_type const& context
                     , options_type const& options
                     ) const {
        BOOST_FOREACH(match_type const& nested, block.nested_results()) {
            render_match(ostream, nested, context, options);
        }
    }

    void render_tag( ostream_type&       ostream
                   , match_type   const& match
                   , context_type const& context
                   , options_type const& options
                   ) const {
        match_type const& match_ = detail::unnest(match);
        id_type    const  id     = match_.regex_id();

        if (typename builtin_tags_type::tag_type const tag = builtin_tags_.get(id)) {
            tag(*this, match_, context, options, ostream);
        }
        else {
            throw_exception(std::logic_error("missing built-in tag"));
        }
    }

    void render_match( ostream_type&       ostream
                     , match_type   const& match
                     , context_type const& context
                     , options_type const& options
                     ) const {
             if (is(match, this->text))  render_text(ostream, match, context, options);
        else if (is(match, this->block)) render_block(ostream, match, context, options);
        else if (is(match, this->tag))   render_tag(ostream, match, context, options);
        else throw_exception(std::logic_error("invalid template state"));
    }

    // TODO: Throw synth exceptions when possible.
    attributes parse_attributes(match_type const& match) const {
        optional<typename attributes::escape_mode> escape;
        optional<string_type> name;
        optional<string_type> default_;

        BOOST_FOREACH(match_type const& nested, match.nested_results()) {
            match_type const& attr  = detail::unnest(nested);
            match_type const& value = attr(this->attribute);

            if (is(attr, this->name_attribute)) {
                if (name) throw_exception(std::logic_error("duplicate variable name"));
                else name = this->extract_attribute(value);
            }
            else if (is(attr, this->default_attribute)) {
                if (default_) throw_exception(std::logic_error("duplicate default value"));
                else default_ = this->extract_attribute(value);
            }
            else if (is(attr, this->escape_attribute)) {
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

    // TODO: Move these out of the kernel.
    string_type const tag_open;
    string_type const tag_close;
    string_type const tag_finish;
    string_type const tag_prefix;
    string_type const tag_attribute;
    string_type const alt_open;
    string_type const alt_close;
    value_type  const default_value;

  public:

    regex_type name;
    regex_type attribute;
    regex_type plain_attribute;
    regex_type quoted_attribute;
    regex_type name_attribute;
    regex_type escape_attribute;
    regex_type default_attribute;
    regex_type extended_attribute;

  private:

    builtin_tags_type builtin_tags_;

}; // kernel

}}}} // namespace ajg::synth::engines::tmpl

#endif // AJG_SYNTH_ENGINES_TMPL_ENGINE_HPP_INCLUDED
