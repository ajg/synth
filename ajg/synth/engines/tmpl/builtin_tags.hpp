//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_TMPL_BUILTIN_TAGS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_TMPL_BUILTIN_TAGS_HPP_INCLUDED

#include <map>
#include <string>

#include <boost/foreach.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace tmpl {

#define TAG(content) \
    ( as_xpr(kernel.tag_open) >> content >> kernel.tag_close \
    | as_xpr(kernel.alt_open) >> content >> kernel.alt_close \
    )
#define NAME(name)       (icase(kernel.tag_prefix + traits_type::literal(name)))
#define OPEN_TAG(name)   TAG(*_s >> NAME(name) >> !(+_s >> kernel.name_attribute) >> *_s)
#define MIDDLE_TAG(name) TAG(*_s >> NAME(name) >> *_s >> !as_xpr(kernel.tag_finish))
#define SINGLE_TAG(name) TAG(*_s >> NAME(name) >> +_s >> kernel.name_attribute >> *_s >> !as_xpr(kernel.tag_finish))
#define CLOSE_TAG(name)  TAG(kernel.tag_finish >> *_s >> NAME(name) >> *_s)

namespace {
using detail::get_nested;
using detail::operator ==;
using namespace detail::placeholders;
} // namespace

template <class Kernel>
struct builtin_tags {
  private:

    typedef Kernel                                                              kernel_type;
    typedef typename kernel_type::id_type                                       id_type;
    typedef typename kernel_type::regex_type                                    regex_type;
    typedef typename kernel_type::match_type                                    match_type;
    typedef typename kernel_type::engine_type                                   engine_type;

    typedef typename engine_type::context_type                                  context_type;
    typedef typename engine_type::options_type                                  options_type;
    typedef typename engine_type::value_type                                    value_type;
    typedef typename engine_type::traits_type                                   traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

  public:

    typedef void (*tag_type)( kernel_type  const& kernel
                            , match_type   const& match
                            , context_type const& context
                            , options_type const& options
                            , ostream_type&       ostream
                            );

  private:

 // typedef std::basic_ostringstream<char_type>                                 string_stream_type;
    typedef std::map<id_type, tag_type>                                         tags_type;

  public:

    inline void initialize(kernel_type& kernel) {
        kernel.tag
            = add(kernel, comment_tag::syntax(kernel),  comment_tag::render)
            | add(kernel, if_tag::syntax(kernel),       if_tag::render)
            | add(kernel, include_tag::syntax(kernel),  include_tag::render)
            | add(kernel, loop_tag::syntax(kernel),     loop_tag::render)
            | add(kernel, unless_tag::syntax(kernel),   unless_tag::render)
            | add(kernel, variable_tag::syntax(kernel), variable_tag::render)
            ;
    }

  private:

    inline regex_type const& add(kernel_type& kernel, regex_type const& regex, tag_type const tag) {
        tags_[regex.regex_id()] = tag;
        return regex;
    }

    tags_type tags_;

  public:

//
// get
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline tag_type get(id_type const id) const {
        typename tags_type::const_iterator it = tags_.find(id);
        return it == tags_.end() ? 0 : it->second;
    }


//
// comment_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct comment_tag {
        static regex_type syntax(kernel_type const& kernel) {
            using namespace xpressive;
            return OPEN_TAG("COMMENT") >> kernel.block
               >> CLOSE_TAG("COMMENT");
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            // Do nothing.
        }
    };

//
// if_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct if_tag {
        static regex_type syntax(kernel_type const& kernel) {
            using namespace xpressive;
            return OPEN_TAG("IF")   >> kernel.block  // B
            >> !(MIDDLE_TAG("ELSE") >> kernel.block) // C
               >> CLOSE_TAG("IF");
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type const& attr  = match(kernel.name_attribute);
            match_type const& if_   = get_nested<B>(match);
            match_type const& else_ = get_nested<C>(match);
            bool const cond_ = kernel.evaluate(attr, context, options);

                 if (cond_) kernel.render_block(ostream, if_,   context, options);
            else if (else_) kernel.render_block(ostream, else_, context, options);
        }
    };

//
// include_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct include_tag {
        static regex_type syntax(kernel_type const& kernel) {
            using namespace xpressive;
            return SINGLE_TAG("INCLUDE");
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type const& attr = match(kernel.name_attribute);
            string_type const path = kernel.extract_attribute(attr);
            kernel.render_file(ostream, path, context, options);
        }
    };

//
// loop_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct loop_tag {
        static regex_type syntax(kernel_type const& kernel) {
            using namespace xpressive;
            return OPEN_TAG("LOOP") >> kernel.block
               >> CLOSE_TAG("LOOP");
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type const& attr    = match(kernel.name_attribute);
            match_type const& body    = match(kernel.block);
            value_type const  value   = kernel.evaluate(attr, context, options);
            size_type  const  size    = value.size();
            context_type context_copy = engine_type::global_variables ? context : context_type();
            size_type    index        = 1;

            BOOST_FOREACH(value_type const& item, value) {
                if (engine_type::loop_variables) {
                    context_copy[traits_type::literal("__SIZE__")]    = size;
                    context_copy[traits_type::literal("__TOTAL__")]   = size;
                    context_copy[traits_type::literal("__FIRST__")]   = to_int(index == 1);
                    context_copy[traits_type::literal("__LAST__")]    = to_int(index == size);
                    context_copy[traits_type::literal("__INNER__")]   = to_int(index != 1 && index != size);
                    context_copy[traits_type::literal("__OUTER__")]   = to_int(index == 1 || index == size);
                    context_copy[traits_type::literal("__ODD__")]     = to_int(index % 2 == 1);
                    context_copy[traits_type::literal("__EVEN__")]    = to_int(index % 2 == 0);
                    context_copy[traits_type::literal("__COUNTER__")] = index++;
                }

                BOOST_FOREACH(value_type const& pair, item) {
                    string_type const k = pair[0].to_string();
                    value_type  const v = pair[1];
                    context_copy[k] = v;
                }

                kernel.render_block(ostream, body, context_copy, options);
            }
        }

      private:

        inline static int to_int(bool const b) { return b ? 1 : 0; }
    };

//
// unless_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct unless_tag {
        static regex_type syntax(kernel_type const& kernel) {
            using namespace xpressive;
            return OPEN_TAG("UNLESS") >> kernel.block  // B
            >> !(MIDDLE_TAG("ELSE")   >> kernel.block) // C
               >> CLOSE_TAG("UNLESS");
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type const& attr  = match(kernel.name_attribute);
            match_type const& unls_ = get_nested<B>(match);
            match_type const& else_ = get_nested<C>(match);
            bool const cond_ = !kernel.evaluate(attr, context, options);

                 if (cond_) kernel.render_block(ostream, unls_, context, options);
            else if (else_) kernel.render_block(ostream, else_, context, options);
        }
    };

//
// variable_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct variable_tag {
        static regex_type syntax(kernel_type const& kernel) {
            using namespace xpressive;
            return TAG(*_s >> NAME("VAR") >> *(+_s >> kernel.extended_attribute) // A
                    >> *_s >> !as_xpr(kernel.tag_finish));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            value_type result;
            typename engine_type::attributes const attrs = kernel.parse_attributes(match);

            if (optional<value_type const&> const variable = detail::find_value(attrs.name, context)) {
                result = *variable;
            }
            else if (attrs.default_) {
                result = *attrs.default_;
            }
            else {
                result = kernel.default_value;
            }

            if (attrs.escape) {
                switch (*attrs.escape) {
                case engine_type::attributes::none: break; // Do nothing.
                case engine_type::attributes::html: result = detail::escape_entities(result.to_string()); break;
                case engine_type::attributes::url:  result = detail::uri_encode(result.to_string());      break;
                case engine_type::attributes::js:   AJG_SYNTH_THROW(not_implemented("js escape mode"));

                }
            }

            ostream << result;
        }
    };

#undef CLOSE_TAG
#undef SINGLE_TAG
#undef MIDDLE_TAG
#undef OPEN_TAG
#undef NAME
#undef TAG

}; // builtin_tags

}}} // namespace ajg::synth::tmpl

#endif // AJG_SYNTH_ENGINES_TMPL_BUILTIN_TAGS_HPP_INCLUDED

