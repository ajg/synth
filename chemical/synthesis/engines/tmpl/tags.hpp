
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHEMICAL_SYNTHESIS_ENGINES_TMPL_TAGS_HPP_INCLUDED
#define CHEMICAL_SYNTHESIS_ENGINES_TMPL_TAGS_HPP_INCLUDED

#include <string>

#include <boost/foreach.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <chemical/synthesis/engines/detail.hpp>

namespace chemical {
namespace synthesis {
namespace tmpl {

#define TAG(content) \
    ( as_xpr(engine.tag_open) >> content >> engine.tag_close \
    | as_xpr(engine.alt_open) >> content >> engine.alt_close )

#define NAME(name) (icase(engine.tag_prefix + \
    typename Engine::string_type(text(name))))

#define OPEN_TAG(name) \
    TAG(*_s >> NAME(name) >> !(+_s >> engine.name_attribute) >> *_s)

#define MIDDLE_TAG(name) \
    TAG(*_s >> NAME(name) >> *_s >> !as_xpr(engine.tag_finish))

#define SINGLE_TAG(name) \
   TAG(*_s >> NAME(name) >> +_s >> engine.name_attribute \
       >> *_s >> !as_xpr(engine.tag_finish))

#define CLOSE_TAG(name) \
    TAG(engine.tag_finish >> *_s >> NAME(name) >> *_s)

using detail::text;
using detail::get_nested;
using namespace detail::placeholders;

//
// comment_tag
////////////////////////////////////////////////////////////////////////////////

struct comment_tag {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            using namespace xpressive;
            return OPEN_TAG("COMMENT") >> engine.block
               >> CLOSE_TAG("COMMENT");
        }

        void render( typename Engine::match_type   const& match
                   , typename Engine::this_type    const& engine
                   , typename Engine::context_type const& context
                   , typename Engine::options_type const& options
                   , typename Engine::stream_type&        out
                   ) const {
            // Do nothing.
        }
    };
};

//
// if_tag
////////////////////////////////////////////////////////////////////////////////

struct if_tag {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            using namespace xpressive;
            return OPEN_TAG("IF")   >> engine.block  // B
            >> !(MIDDLE_TAG("ELSE") >> engine.block) // C
               >> CLOSE_TAG("IF");
        }

        void render( typename Engine::match_type   const& match
                   , typename Engine::this_type    const& engine
                   , typename Engine::context_type const& context
                   , typename Engine::options_type const& options
                   , typename Engine::stream_type&        out
                   ) const {
            typename Engine::match_type const& attr  = match(engine.name_attribute);
            typename Engine::match_type const& if_   = get_nested<B>(match);
            typename Engine::match_type const& else_ = get_nested<C>(match);
            bool const cond_ = engine.evaluate(attr, context, options);

                 if (cond_) engine.render_block(out, if_,   context, options);
            else if (else_) engine.render_block(out, else_, context, options);
        }
    };
};

//
// include_tag
////////////////////////////////////////////////////////////////////////////////

struct include_tag {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            using namespace xpressive;
            return SINGLE_TAG("INCLUDE");
        }

        void render( typename Engine::match_type   const& match
                   , typename Engine::this_type    const& engine
                   , typename Engine::context_type const& context
                   , typename Engine::options_type const& options
                   , typename Engine::stream_type&        out
                   ) const {
            typename Engine::match_type const& attr = match(engine.name_attribute);
            typename Engine::string_type const path = engine.extract_attribute(attr);
            engine.render_file(out, path, context, options);
        }
    };
};

//
// loop_tag
////////////////////////////////////////////////////////////////////////////////

template < bool LoopVariables = true
         , bool GlobalVariables = false
         >
struct loop_tag {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            using namespace xpressive;
            return OPEN_TAG("LOOP") >> engine.block
               >> CLOSE_TAG("LOOP");
        }

        inline static int as_int(bool const b) { return b ? 1 : 0; }

        void render( typename Engine::match_type   const& match
                   , typename Engine::this_type    const& engine
                   , typename Engine::context_type const& context
                   , typename Engine::options_type const& options
                   , typename Engine::stream_type&        out
                   ) const {
            typename Engine::match_type const& attr = match(engine.name_attribute);
            typename Engine::match_type const& body = match(engine.block);
            typename Engine::value_type const value =
                engine.evaluate(attr, context, options);
            typename Engine::size_type const length = value.length();
            typename Engine::size_type i = 1;
            typename Engine::context_type copy;
            typedef typename Engine::char_type C;

            if (GlobalVariables) {
                copy = context;
            }

            BOOST_FOREACH(typename Engine::value_type const& item, value) {
                if (LoopVariables) {
                    copy[text("__SIZE__")]    = length;
                    copy[text("__TOTAL__")]   = length;
                    copy[text("__FIRST__")]   = as_int(i == 1);
                    copy[text("__LAST__")]    = as_int(i == length);
                    copy[text("__INNER__")]   = as_int(i != 1 && i != length);
                    copy[text("__OUTER__")]   = as_int(i == 1 || i == length);
                    copy[text("__ODD__")]     = as_int(i % 2 == 1);
                    copy[text("__EVEN__")]    = as_int(i % 2 == 0);
                    copy[text("__COUNTER__")] = i++;
                }

                BOOST_FOREACH(typename Engine::value_type const& pair, item) {
                    typename Engine::string_type const k = pair[0].to_string();
                    typename Engine::value_type const v = pair[1];
                    copy[k] = v;
                }

                engine.render_block(out, body, copy, options);
            }
        }
    };
};

//
// unless_tag
////////////////////////////////////////////////////////////////////////////////

struct unless_tag {
    template <class Engine>
    struct definition {
        typename Engine::regex_type syntax(Engine const& engine) const {
            using namespace xpressive;
            return OPEN_TAG("UNLESS") >> engine.block  // B
            >> !(MIDDLE_TAG("ELSE")   >> engine.block) // C
               >> CLOSE_TAG("UNLESS");
        }

        void render( typename Engine::match_type   const& match
                   , typename Engine::this_type    const& engine
                   , typename Engine::context_type const& context
                   , typename Engine::options_type const& options
                   , typename Engine::stream_type&        out
                   ) const {
            typename Engine::match_type const& attr  = match(engine.name_attribute);
            typename Engine::match_type const& unls_ = get_nested<B>(match);
            typename Engine::match_type const& else_ = get_nested<C>(match);
            bool const cond_ = !engine.evaluate(attr, context, options);

                 if (cond_) engine.render_block(out, unls_, context, options);
            else if (else_) engine.render_block(out, else_, context, options);
        }
    };
};

//
// variable_tag
////////////////////////////////////////////////////////////////////////////////

struct variable_tag {
    template <class Engine>
    struct definition {
        void initialize(Engine const& engine) {
            using namespace xpressive;

            typename Engine::regex_type const equal = *_s >> '=' >> *_s;
            escape_attribute = icase("ESCAPE")  >> equal >> engine.attribute;
            default_attribute = icase("DEFAULT") >> equal >> engine.attribute;
        }

        typename Engine::regex_type syntax(Engine const& engine) const {
            using namespace xpressive;

            typename Engine::regex_type const attribute
                = escape_attribute
                | default_attribute
                | engine.name_attribute
                ;
            return TAG(*_s >> NAME("VAR") >> *(+_s >> attribute) // A
                    >> *_s >> !as_xpr(engine.tag_finish));
        }

        void render( typename Engine::match_type   const& match
                   , typename Engine::this_type    const& engine
                   , typename Engine::context_type const& context
                   , typename Engine::options_type const& options
                   , typename Engine::stream_type&        out
                   ) const {
            typename Engine::value_type result;
            attributes const attrs = parse_attributes(match, engine);

            if (optional<typename Engine::value_type const&> const
                    variable = detail::find_value(attrs.name, context)) {
                result = *variable;
            }
            else if (attrs.default_) {
                result = *attrs.default_;
            }
            else {
                result = engine.default_value;
            }

            if (attrs.escape) {
                switch (*attrs.escape) {
                case none: break; // Do nothing.
                case html: //result = detail::escape_html(result); break;
                case url:  //result = detail::escape_url(result);  break;
                case js:   //result = detail::escape_js(result);   break;
                    abort();
                }
            }

            out << result;
        }

      private:

        enum escape_mode { none, html, url, js };

        struct attributes {
            typename Engine::string_type           name;
            optional<typename Engine::string_type> default_;
            optional<escape_mode>                  escape;
        };

        attributes parse_attributes( typename Engine::match_type const& match
                                   , typename Engine::this_type  const& engine
                                   ) const {
            optional<escape_mode> escape;
            optional<typename Engine::string_type> name;
            optional<typename Engine::string_type> default_;

            BOOST_FOREACH(typename Engine::match_type const& nested, match.nested_results()) {
                typename Engine::match_type const& attr = get_nested<A>(nested);
                typename Engine::match_type const& value = attr(engine.attribute);

                if (attr == engine.name_attribute) {
                    if (name) throw_exception(std::logic_error("duplicate variable name"));
                    else name = engine.extract_attribute(value);
                }
                else if (attr == default_attribute) {
                    if (default_) throw_exception(std::logic_error("duplicate default value"));
                    else default_ = engine.extract_attribute(value);
                }
                else if (attr == escape_attribute) {
                    if (escape) {
                        throw_exception(std::logic_error("duplicate escape mode"));
                    }
                    else {
                        typename Engine::string_type const mode =
                            algorithm::to_lower_copy(value.str());

                             if (mode == text("none")
                              || mode == text("0"))   escape = none;
                        else if (mode == text("html")
                              || mode == text("1"))   escape = html;
                        else if (mode == text("url")) escape = url;
                        else if (mode == text("js"))  escape = js;
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
            return detail::construct<attributes>(*name, default_, escape);
        }

      private:

        typename Engine::regex_type /*const*/ name_attribute;
        typename Engine::regex_type /*const*/ escape_attribute;
        typename Engine::regex_type /*const*/ default_attribute;
    };
};

#undef CLOSE_TAG
#undef SINGLE_TAG
#undef MIDDLE_TAG
#undef OPEN_TAG
#undef NAME
#undef TAG

}}} // namespace chemical::synthesis::tmpl

#endif // CHEMICAL_SYNTHESIS_ENGINES_TMPL_TAGS_HPP_INCLUDED

