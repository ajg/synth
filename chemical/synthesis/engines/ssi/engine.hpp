
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ENGINES_SSI_ENGINE_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ENGINES_SSI_ENGINE_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <numeric>
#include <cstdlib>
#include <algorithm>

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

#include <chemical/synthesis/template.hpp>
#include <chemical/synthesis/engines/detail.hpp>
#include <chemical/synthesis/engines/exceptions.hpp>
#include <chemical/synthesis/engines/base_definition.hpp>
#include <chemical/synthesis/engines/ssi/value.hpp>
#include <chemical/synthesis/engines/ssi/library.hpp>

namespace chemical {
namespace synthesis {
namespace ssi {

using detail::operator ==;

template <class String>
struct options {
    typedef String string_type;

    options()
        : size_format(detail::text("bytes"))
        , time_format(detail::text("%A, %d-%b-%Y %H:%M:%S %Z"))
        , echo_message(detail::text("(none)"))
        , error_message(detail::text("[an error occurred"
              " while processing this directive]")) {}

    string_type size_format;
    string_type time_format;
    string_type echo_message;
    string_type error_message;
};

template < class Library = ssi::default_library
         , class Environment = detail::standard_environment
         , bool ThrowOnErrors = false
         >
struct engine : detail::nonconstructible {

typedef engine engine_type;

template <class BidirectionalIterator>
struct definition : base_definition< BidirectionalIterator
                                   , definition<BidirectionalIterator>
                                   > {
  public:
    // Constants:

    BOOST_STATIC_CONSTANT(bool, throw_on_errors = ThrowOnErrors);

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

    typedef Library                           library_type;
    typedef Environment                       environment_type;
    typedef ssi::value<char_type>             value_type;
    typedef std::vector<string_type>          whitelist_type;
    typedef std::vector<value_type>           array_type;
    typedef std::map<string_type, value_type> context_type;
    typedef library_type                      tags_type;
    typedef options<string_type>              options_type;
    typedef typename detail::define_sequence
        <this_type, tags_type>::type          tag_definitions_type;
    // Define string iterators/regexes specifically. This is useful when
    // they are different from the main iterator_type and regex_type (e.g.
    // when the latter two involve the use of a file_iterator.)
    typedef typename string_type::const_iterator           string_iterator_type;
    typedef xpressive::basic_regex<string_iterator_type>   string_regex_type;
    typedef xpressive::match_results<string_iterator_type> string_match_type;

    struct args_type {
        this_type   const& engine;
        match_type  const& match;
        context_type&      context;
        options_type&      options;
        stream_type&       stream;
    };

  public:

    definition()
        : tag_start (detail::text("<!--#"))
        , tag_end   (detail::text("-->"))
        , environment() {
        using namespace xpressive;
//
// common grammar
////////////////////////////////////////////////////////////////////////////////

        name
            // @see http://www.w3.org/TR/2000/WD-xml-2e-20000814#NT-Name
            = (alpha | '_' | ':') >> *(_w | (set= '_', ':', '-', '.'))
            ;
        quoted_value
            = '"'  >> *~as_xpr('"')  >> '"'
            | '`'  >> *~as_xpr('`')  >> '`'
            | '\'' >> *~as_xpr('\'') >> '\''
            ;
        attribute
            = name >> *_s >> '=' >> *_s >> quoted_value
            ;
        skipper
            = as_xpr(tag_start);
            ;

        this->initialize_grammar();
        fusion::for_each(tags_, detail::construct
            <detail::element_initializer<this_type> >(*this));
        detail::append_tags<this_type, tags_type::size::value>(*this);
    }

  public:

    string_type attribute_string(match_type const& attr) const {
        string_type const string = attr.str();
        BOOST_ASSERT(string.length() >= 2);
        return string.substr(1, string.length() - 2);
    }

    string_type lookup_variable( context_type const& context
                               , options_type const& options
                               , string_type  const& name
                               ) const {
        // First, check the context.
        if (optional<value_type const&> const value
                = detail::find_value(name, context)) {
            return value->to_string();
        }
        // Second, check for magic variables.
        else if (name == detail::text("DOCUMENT_NAME")) {
            throw_exception(not_implemented("DOCUMENT_NAME"));
        }
        else if (name == detail::text("DOCUMENT_URI")) {
            throw_exception(not_implemented("DOCUMENT_URI"));
        }
        else if (name == detail::text("DATE_LOCAL")) {
            return detail::format_current_time(options.time_format);
        }
        else if (name == detail::text("DATE_GMT")) {
            throw_exception(not_implemented("DATE_GMT"));
        }
        else if (name == detail::text("LAST_MODIFIED")) {
            throw_exception(not_implemented("LAST_MODIFIED"));
        }
        // Third, check the environment.
        else if(optional<typename environment_type::mapped_type const>
                       const value = detail::find_mapped_value(
                   this->template convert<char>(name), environment)) {
            return this->template convert<char_type>(*value);
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
                   ) const
    try {
        using namespace detail;
        // If there's only _one_ tag, xpressive will not
        // "nest" the match, so we use it directly instead.
        match_type const& tag = tags_type::size::value == 1 ? match : get_nested<1>(match);
        tag_renderer<this_type, true> const renderer = { *this, stream, tag, context, options };
        find_by_index(*this, tags_, tag_ids_, tag.regex_id(), renderer);
    }
    catch (std::exception const&) {
        if (throw_on_errors) throw;
        stream << options.error_message;
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

    regex_type directive(string_type const& name) const {
        using namespace xpressive;
        return tag_start >> *_s >> name
            >> (regex_type() = *(+_s >> attribute)) >> *_s >> tag_end;
    }
/*
  private:

    static inline void set_undefined_variable( context_type& context
                                    , typename context_type::key_type    const& key
                                    , typename context_type::mapped_type const& value
                                    ) {
        if (context.find(key) == context.end()) {
            context.insert(typename context_type::value_type(key, value));
        }
    }*/

  public:

    string_type const tag_start;
    string_type const tag_end;

  public:

    regex_type tag, text, block, skipper;
    regex_type name, attribute, quoted_value;
    environment_type const environment;
    options_type const default_options;

  private:

    whitelist_type whitelist_;
    tag_definitions_type  tags_;
    std::vector<id_type>  tag_ids_;

    template <class Engine, typename Engine::tags_type::size::value_type Size>
    friend struct detail::append_tags;

}; // definition

}; // engine

}}} // namespace chemical::synthesis::ssi

#endif // CHEMICAL_SYNTHESIS_ENGINES_SSI_ENGINE_HPP_INCLUDED

