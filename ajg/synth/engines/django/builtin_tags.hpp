//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_BUILTIN_TAGS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_BUILTIN_TAGS_HPP_INCLUDED

#include <map>
#include <string>
#include <locale>
#include <sstream>
#include <iterator>
#include <stdexcept>

#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/adapters/map.hpp>
#include <ajg/synth/adapters/bool.hpp>
#include <ajg/synth/adapters/string.hpp>
#include <ajg/synth/adapters/vector.hpp>
#include <ajg/synth/adapters/numeric.hpp>
#include <ajg/synth/adapters/utility.hpp>
#include <ajg/synth/adapters/variant.hpp>
#include <ajg/synth/engines/django/formatter.hpp>

namespace ajg {
namespace synth {

struct missing_variable;
struct missing_attribute;

namespace django {
namespace {
using detail::operator ==;
using boost::xpressive::_s;
using boost::xpressive::as_xpr;
using boost::xpressive::s1;
using boost::xpressive::s2;
using boost::xpressive::s3;
using boost::xpressive::s4;
boost::xpressive::mark_tag const id(9);
} // namespace

template <class Engine>
struct builtin_tags {
  public:

    typedef Engine                                                              engine_type;
    typedef typename engine_type::options_type                                  options_type;
    typedef typename options_type::traits_type                                  traits_type;
    typedef typename options_type::boolean_type                                 boolean_type;
    typedef typename options_type::char_type                                    char_type;
    typedef typename options_type::size_type                                    size_type;
    typedef typename options_type::number_type                                  number_type;
    typedef typename options_type::datetime_type                                datetime_type;
    typedef typename options_type::duration_type                                duration_type;
    typedef typename options_type::string_type                                  string_type;
    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::range_type                                   range_type;
    typedef typename options_type::sequence_type                                sequence_type;
    typedef typename options_type::arguments_type                               arguments_type;
    typedef typename options_type::context_type                                 context_type;

    typedef typename engine_type::id_type                                       id_type;
    typedef typename engine_type::regex_type                                    regex_type;
    typedef typename engine_type::match_type                                    match_type;
    typedef typename engine_type::stream_type                                   out_type; // TODO: Rename ostream_type.

    typedef void (*tag_type)( engine_type  const& engine
                            , match_type   const& match
                            , context_type const& context
                            , options_type const& options
                            , out_type&           out
                            );

  private:

    typedef std::basic_ostringstream<char_type>                                 string_stream_type;
    typedef std::map<id_type, tag_type>                                         tags_type;
    typedef formatter<options_type>                                             formatter_type;

  public:

    inline void initialize(engine_type& engine) {
        engine.tag
            = add(engine, autoescape_tag::syntax(engine),  autoescape_tag::render)
            | add(engine, block_tag::syntax(engine),       block_tag::render)
            | add(engine, comment_tag::syntax(engine),     comment_tag::render)
            | add(engine, csrf_token_tag::syntax(engine),  csrf_token_tag::render)
            | add(engine, cycle_tag::syntax(engine),       cycle_tag::render)
            | add(engine, debug_tag::syntax(engine),       debug_tag::render)
            | add(engine, extends_tag::syntax(engine),     extends_tag::render)
            | add(engine, filter_tag::syntax(engine),      filter_tag::render)
            | add(engine, firstof_tag::syntax(engine),     firstof_tag::render)
            | add(engine, for_tag::syntax(engine),         for_tag::render)
            | add(engine, for_empty_tag::syntax(engine),   for_empty_tag::render)
            | add(engine, if_tag::syntax(engine),          if_tag::render)
            | add(engine, ifchanged_tag::syntax(engine),   ifchanged_tag::render)
            | add(engine, ifequal_tag::syntax(engine),     ifequal_tag::render)
            | add(engine, ifnotequal_tag::syntax(engine),  ifnotequal_tag::render)
            | add(engine, include_tag::syntax(engine),     include_tag::render)
            | add(engine, load_tag::syntax(engine),        load_tag::render)
            | add(engine, load_from_tag::syntax(engine),   load_from_tag::render)
            | add(engine, now_tag::syntax(engine),         now_tag::render)
            | add(engine, regroup_tag::syntax(engine),     regroup_tag::render)
            | add(engine, spaceless_tag::syntax(engine),   spaceless_tag::render)
            | add(engine, ssi_tag::syntax(engine),         ssi_tag::render)
            | add(engine, templatetag_tag::syntax(engine), templatetag_tag::render)
            | add(engine, url_tag::syntax(engine),         url_tag::render)
            | add(engine, url_as_tag::syntax(engine),      url_as_tag::render)
            | add(engine, variable_tag::syntax(engine),    variable_tag::render)
            | add(engine, verbatim_tag::syntax(engine),    verbatim_tag::render)
            | add(engine, widthratio_tag::syntax(engine),  widthratio_tag::render)
            | add(engine, with_tag::syntax(engine),        with_tag::render)
            | add(engine, library_tag::syntax(engine),     library_tag::render)
            ;
    }

  private:

    inline regex_type const& add(engine_type& engine, regex_type const& regex, tag_type const tag) {
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

// TODO: Replace with function.
#define AJG_TAG(content) engine.block_open >> *_s >> content >> *_s >> engine.block_close

//
// autoescape_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct autoescape_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("autoescape") >> engine.name) >> engine.block
                >> AJG_TAG(engine.reserved("endautoescape"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            string_type const& setting = match(engine.name)[id].str();
            match_type  const& block   = match(engine.block);
            boolean_type autoescape = true;

                 if (setting == traits_type::literal("on"))  autoescape = true;
            else if (setting == traits_type::literal("off")) autoescape = false;
            else throw_exception(std::invalid_argument("setting"));

            options_type options_copy = options; // NOTE: Don't make the copy const.
            options_copy.autoescape = autoescape;
            engine.render_block(out, block, context, options_copy);
        }
    };

//
// block_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct block_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("block") >> (s1 = engine.name)) >> engine.block
                >> AJG_TAG(engine.reserved("endblock") >> !(s2 = engine.name));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type  const& block = match(engine.block);
            string_type const& name  = match(s1)[id].str();
            string_type const& close = match(s2)[id].str();

            if (!close.empty() && name != close) {
                std::string const original = traits_type::narrow(name);
                std::string const message  = "mismatched endblock tag for " + original;
                throw_exception(std::logic_error(message));
            }

            if (options.blocks_) { // We're being inherited from.
                if (optional<string_type const&> const overriden =
                    detail::find_value(name, *options.blocks_)) {
                    out << *overriden;
                }
                else {
                    string_stream_type stream;
                    engine.render_block(stream, block, context, options);
                    string_type const result = stream.str();
                    (*options.blocks_)[name] = result;
                    out << result;
                }
            }
            else { // We're being rendered directly.
                engine.render_block(out, block, context, options);
            }
        }
    };

//
// comment_tag
//     TODO: Split into comment_short_tag and comment_long_tag.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct comment_tag {
        static regex_type syntax(engine_type& engine) {
            namespace x = boost::xpressive;
            return // Short form; assert no closing token or newlines.
                   engine.comment_open >> *(~x::before(engine.comment_close | x::_n) >> x::_)
                >> engine.comment_close
                   // Long form
                |  AJG_TAG(engine.reserved("comment")) >> engine.block
                >> AJG_TAG(engine.reserved("endcomment"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            // Do nothing.
        }
    };

//
// cycle_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct cycle_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("cycle") >> engine.arguments >> !(engine.keyword("as") >> engine.name)) >> engine.block;
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            size_type   const  position = match.position();
            match_type  const& args     = match(engine.arguments);
            match_type  const& block    = match(engine.block);
            string_type const& name     = match(engine.name)[id].str();
            size_type   const  total    = args.nested_results().size();
            size_type   const  current  = detail::find_mapped_value(position, options.cycles_).get_value_or(0);

            match_type const& arg   = *detail::advance_to(args.nested_results().begin(), current);
            value_type const  value = engine.evaluate(arg, context, options);
            const_cast<options_type&>(options).cycles_[position] = (current + 1) % total;
            out << value;

            if (name.empty()) {
                // E.g. cycle foo
                engine.render_block(out, block, context, options);
            }
            else {
                // E.g. cycle foo as bar
                context_type context_copy = context;
                context_copy[name] = value;
                engine.render_block(out, block, context_copy, options);
            }
        }
    };

//
// csrf_token_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct csrf_token_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("csrf_token"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {

            if (optional<value_type const&> const token = detail::find_value(traits_type::literal("csrf_token"), context)) {
                string_type const& s = detail::escape_entities(token->to_string());

                if (s != traits_type::literal("NOTPROVIDED")) {
                    out << "<div style='display:none'>";
                    out << "<input type='hidden' name='csrfmiddlewaretoken' value='" << s << "' />";
                    out << "</div>";
                }
            }
        }
    };

//
// debug_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct debug_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("debug"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            out << "<h1>Context:</h1>" << std::endl;

            BOOST_FOREACH(typename context_type::value_type const& p, context) {
                out << "    " << value_type(p.first).escape()
                    << " = "  << value_type(p.second).escape() << "<br />" << std::endl;
            }
        }
    };

//
// extends_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct extends_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("extends") >> engine.string_literal) >> engine.block;
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            typedef typename options_type::blocks_type    blocks_type;
            typedef typename blocks_type::value_type block_type;

            match_type  const& string   = match(engine.string_literal);
            match_type  const& body     = match(engine.block);
            string_type const  filepath = engine.extract_string(string);

            std::basic_ostream<char_type> null_stream(0);
            blocks_type blocks, supers;
            options_type options_copy = options;
            context_type context_copy = context;

            options_copy.blocks_ = &blocks; // Set it.

            // First, we render the parent template as if it were
            // stand-alone, so that we can make block.super
            // available to the derived template. We don't care
            // about non-block content, so it is discarded.
            engine.render_file(null_stream, filepath, context, options_copy);
            string_type const suffix = traits_type::literal("_super");

            BOOST_FOREACH(block_type const& block, blocks) {
                context_copy[block.first + suffix] = block.second;
            }

            // We only care about the supers; any other
            // modifications to the options are discarded.
            options_copy = options; // Reset it.
            options_copy.blocks_ = &supers;

            // Second, we "extract" the blocks that the derived
            // template will be overriding, while at the same time
            // making the parent template's versions available
            // for the derivee to use as block.super. The derivee's
            // non-block content is irrelevant so it is discarded.
            engine.render_block(null_stream, body, context_copy, options_copy);

            // We only care about the blocks; any other
            // modifications to the options are discarded.
            options_copy = options; // Reset it.
            options_copy.blocks_ = &supers; //blocks;

            // Finally, we render the parent template with the
            // potentially overriden blocks already rendered.
            engine.render_file(out, filepath, context, options_copy);
        }
    };

//
// firstof_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct firstof_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("firstof") >> engine.arguments >> !engine.string_literal);
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            BOOST_FOREACH(match_type const& var, match(engine.arguments).nested_results()) {
                try {
                    if (value_type const value = engine.evaluate(var, context, options)) {
                        out << value;
                        break;
                    }
                }
                catch (missing_variable const&) {
                    // Missing variables count as 'False' ones.
                }
            }

            // Use the fallback, if there is one.
            if (match_type const& fallback = match(engine.string_literal)) {
                out << engine.extract_string(fallback);
            }
        }
    };

//
// filter_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct filter_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("filter") >> engine.filters) >> engine.block
                >> AJG_TAG(engine.reserved("endfilter"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            string_stream_type stream;
            engine.render_block(stream, match(engine.block), context, options);
            out << engine.apply_filters(stream.str(), match(engine.filters), context, options);
        }
    };

//
// for_tag
//     TODO: for ... in ... _reversed_, using BOOST_FOREACH_REVERSE.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct for_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("for") >> engine.variable_names >> engine.keyword("in") >> engine.value) >> engine.block
              >> !(AJG_TAG(engine.reserved("empty")) >> engine.block)
                >> AJG_TAG(engine.reserved("endfor"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type  const& vars  = match(engine.variable_names);
            match_type  const& for_  = match(engine.block, 0);
            match_type  const& empty = match(engine.block, 1);
            value_type  const& value = engine.evaluate(match(engine.value), context, options);

            typename value_type::const_iterator it(value.begin()), end(value.end());
            typename options_type::names_type const& variables = engine.extract_names(vars);

            if (it == end) {
                if (empty) { // for ... empty ... endfor case.
                    engine.render_block(out, empty, context, options);
                }
                return;
            }

            context_type context_copy = context;
            size_type const n = variables.size();
            BOOST_ASSERT(n > 0);

            for (; it != end; ++it) {
                if (n == 1) { // e.g. for x in ...
                    context_copy[variables[0]] = boost::ref(*it);
                }
                else {
                    size_type i = 0;
                    BOOST_FOREACH(value_type const& var, *it) { // e.g. for x, y, z in ...
                        if (i >= n) break;
                        context_copy[variables[i++]] = var; // TODO: boost::ref?
                    }

                    while (i < n) { // Overwrite the remaining vars in the context.
                        context_copy[variables[i++]] = options.default_value;
                    }
                }

                engine.render_block(out, for_, context_copy, options);
            }
        }
    };

//
// for_empty_tag
//     NOTE: This is a no-op, since the empty clause is already handled by for_tag.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct for_empty_tag {
        static regex_type syntax(engine_type& engine) { return engine.nothing; }
        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {}
    };

//
// if_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct if_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("if") >> engine.value) >> engine.block
              >> !(AJG_TAG(engine.reserved("else")) >> engine.block)
                >> AJG_TAG(engine.reserved("endif"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type   const& if_   = match(engine.block, 0);
            match_type   const& else_ = match(engine.block, 1);
            boolean_type const  cond_ = engine.evaluate(match(engine.value), context, options);

                 if (cond_) engine.render_block(out, if_,   context, options);
            else if (else_) engine.render_block(out, else_, context, options);
        }
    };

//
// ifchanged_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct ifchanged_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("ifchanged") >> engine.arguments) >> engine.block
              >> !(AJG_TAG(engine.reserved("else"))     >> engine.block)
              >>   AJG_TAG(engine.reserved("endifchanged"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type const& args  = match(engine.arguments);
            match_type const& if_   = match(engine.block, 0);
            match_type const& else_ = match(engine.block, 1);

            size_type const position = match.position();
            optional<value_type const&> const value = detail::find_value(position, options.changes_);

            // This is the case with no variables (compare contents).
            if (args.nested_results().empty()) {
                string_stream_type stream;
                engine.render_block(stream, if_, context, options);
                string_type const result = stream.str();

                if (value && *value == value_type/*ref*/(result)) {
                    if (else_) {
                        engine.render_block(out, else_, context, options);
                    }
                }
                else {
                    const_cast<options_type&>(options).changes_[position] = result;
                    out << result;
                }
            }
            // Here, we compare variables.
            else {
                // NOTE: The key is a string (rather than an int) presumably in case variables are repeated.
                std::map<string_type, value_type> values;

                BOOST_FOREACH(match_type const& arg, args.nested_results()) {
                    string_type const s = boost::algorithm::trim_copy(arg.str());
                    values[s] = engine.evaluate(arg, context, options);
                }

                if (value && *value == value_type/*ref*/(values)) {
                    if (else_) {
                        engine.render_block(out, else_, context, options);
                    }
                }
                else {
                    const_cast<options_type&>(options).changes_[position] = values;
                    engine.render_block(out, if_, context, options);
                }
            }
        }
    };

//
// ifequal_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct ifequal_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("ifequal") >> engine.value >> engine.value) >> engine.block
              >> !(AJG_TAG(engine.reserved("else")) >> engine.block)
              >>   AJG_TAG(engine.reserved("endifequal"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type   const& left  = match(engine.value, 0);
            match_type   const& right = match(engine.value, 1);
            match_type   const& if_   = match(engine.block, 0);
            match_type   const& else_ = match(engine.block, 1);
            boolean_type const  cond_ =
                engine.evaluate(left, context, options) ==
                engine.evaluate(right, context, options);

                 if (cond_) engine.render_block(out, if_,   context, options);
            else if (else_) engine.render_block(out, else_, context, options);
        }
    };

//
// ifnotequal_tag
// TODO: DRY this and ifequal_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct ifnotequal_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("ifnotequal") >> engine.value >> engine.value) >> engine.block
              >> !(AJG_TAG(engine.reserved("else")) >> engine.block)
              >>   AJG_TAG(engine.reserved("endifnotequal"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type   const& left  = match(engine.value, 0);
            match_type   const& right = match(engine.value, 1);
            match_type   const& if_   = match(engine.block, 0);
            match_type   const& else_ = match(engine.block, 1);
            boolean_type const  cond_ =
                engine.evaluate(left, context, options) !=
                engine.evaluate(right, context, options);

                 if (cond_) engine.render_block(out, if_,   context, options);
            else if (else_) engine.render_block(out, else_, context, options);
        }
    };

//
// include_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct include_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("include") >> engine.string_literal);
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            string_type const path = engine.extract_string(match(engine.string_literal));
            engine.render_file(out, path, context, options);
        }
    };

//
// load_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct load_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("load") >> engine.packages) >> engine.block;
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type const& packages = match(engine.packages);
            match_type const& block    = match(engine.block);
            options_type options_copy  = options;
            context_type context_copy  = context;

            BOOST_FOREACH(match_type const& package, detail::select_nested(packages, engine.package)) {
                string_type const& library = package[id].str();
                engine.load_library(context_copy, options_copy, library);
            }

            engine.render_block(out, block, context_copy, options_copy);
        }
    };

//
// load_from_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct load_from_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("load") >> engine.names >> engine.keyword("from") >> engine.package) >> engine.block;
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type  const& names   = match(engine.names);
            match_type  const& block   = match(engine.block);
            string_type const& library = match(engine.package)[id].str();
            std::vector<string_type> components;

            BOOST_FOREACH(match_type const& name, detail::select_nested(names, engine.name)) {
                components.push_back(name[id].str());
            }

            options_type options_copy = options;
            context_type context_copy = context;
            engine.load_library(context_copy, options_copy, library, &components);
            engine.render_block(out, block, context_copy, options_copy);
        }
    };

//
// now_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct now_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("now") >> engine.string_literal);
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            string_type const format = engine.extract_string(match(engine.string_literal));
            out << formatter_type::format_datetime(options, format, detail::utc_now());
        }
    };

//
// regroup_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct regroup_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("regroup") >> engine.value
                          >> engine.keyword("by") >> engine.package
                          >> engine.keyword("as") >> engine.name) >> engine.block;
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type  const& expr  = match(engine.value);
            string_type const& attrs = match(engine.package)[id].str();
            string_type const& name  = match(engine.name)[id].str();
            match_type  const& block = match(engine.block);

            value_type        values;
            entries_type entries;
            try {
                values = engine.evaluate(expr, context, options);
            }
            // Fail silently in these cases:
            catch (missing_variable  const&) { goto done; }
            catch (missing_attribute const&) { goto done; }
            entries = regroup(values, attrs);

          done:
            context_type context_copy = context;
            context_copy[name] = entries;
            engine.render_block(out, block, context_copy, options);
        }

        typedef std::map<string_type, value_type>   entry_type;
        typedef std::vector<entry_type>             entries_type;

        inline static entries_type regroup(value_type const& values, string_type const& attrs) {
            static string_type const grouper_name = traits_type::literal("grouper");
            static string_type const list_name    = traits_type::literal("list");
            entries_type entries;

            BOOST_FOREACH(typename value_type::group_type const& group, values.group_by(attrs)) {
                value_type const key   = group.first;
                value_type const value = group.second;
                entry_type entry;
                entry[grouper_name] = key;
                entry[list_name]    = value;
                entries.push_back(entry);
            }

            return entries;
        }
    };

//
// spaceless_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct spaceless_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("spaceless")) >> engine.block
                >> AJG_TAG(engine.reserved("endspaceless"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            // TODO: Reuse engine.html_tag if possible.
            static typename engine_type::string_regex_type const tag = '<' >> *~(as_xpr('>')) >> '>';
            static typename engine_type::string_regex_type const gap = (s1 = tag) >> +_s >> (s2 = tag);

            string_stream_type stream;
            std::ostreambuf_iterator<char_type> output(out);

            match_type const& body = match(engine.block);
            engine.render_block(stream, body, context, options);
            // TODO: Use detail::bidirectional_istream_iterator to feed directly to regex_replace.
            string_type const string = stream.str();
            regex_replace(output, string.begin(), string.end(), gap, traits_type::literal("$1$2"));
        }
    };

//
// ssi_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct ssi_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("ssi") >> (s1 = '/' >> +~_s) >> !(+_s >> (s2 = "parsed")) >> *_s);
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            string_type  const path   = match[s1].str();
            boolean_type const parsed = match[s2].matched;

            if (parsed) {
                engine.render_file(out, path, context, options);
            }
            else {
                string_type line;
                std::string const path_ = traits_type::narrow(path);
                std::basic_ifstream<char_type> file(path_.c_str());
                while (std::getline(file, line)) out << line;
            }
        }
    };

//
// templatetag_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct templatetag_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("templatetag") >> engine.name);
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            string_type const name = match(engine.name)[id].str();

            if (optional<string_type const> const& marker = detail::find_mapped_value(name, engine.markers)) {
                out << *marker;
            }
            else {
                out << options.default_value;
            }
        }
    };

//
// url_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct url_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("url") >> engine.value >> engine.arguments);
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type const& expr = match(engine.value);
            match_type const& args = match(engine.arguments);

            value_type     const view      = engine.evaluate(expr, context, options);
            arguments_type const arguments = engine.evaluate_arguments(args, context, options);

            if (optional<string_type> const& url = engine.get_view_url(view, arguments, context, options)) {
                out << *url;
            }
            else {
                throw_exception(std::runtime_error("view not found"));
            }
        }
    };

//
// url_as_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct url_as_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("url") >> engine.value >> engine.arguments
                        >> engine.keyword("as") >> engine.name) >> engine.block;
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type  const& expr  = match(engine.value);
            match_type  const& args  = match(engine.arguments);
            match_type  const& block = match(engine.block);
            string_type const& name  = match(engine.name)[id].str();

            value_type     const view      = engine.evaluate(expr, context, options);
            arguments_type const arguments = engine.evaluate_arguments(args, context, options);
            string_type    const url       = engine.get_view_url(view, arguments, context, options)
                                                   .get_value_or(string_type());

            context_type context_copy = context;
            context_copy[name] = url;
            engine.render_block(out, block, context_copy, options);
        }
    };

//
// variable_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct variable_tag {
        static regex_type syntax(engine_type& engine) {
            return engine.variable_open >> *_s >> engine.value >> engine.variable_close;
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            value_type const& value = engine.evaluate(match(engine.value), context, options);
            boolean_type const safe = !options.autoescape || value.safe();
            safe ? out << value : out << value.escape();
        }
    };

//
// verbatim_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct verbatim_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("verbatim")) >> engine.block
                >> AJG_TAG(engine.reserved("endverbatim"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            out << match(engine.block).str();
        }
    };

//
// widthratio_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct widthratio_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("widthratio") >> engine.value >> engine.value >> engine.value);
        }

        template <class T>
        inline static T round(T const r) {
            return (r > 0.0) ? std::floor(r + 0.5) : std::ceil(r - 0.5);
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type const& value = match(engine.value, 0);
            match_type const& limit = match(engine.value, 1);
            match_type const& width = match(engine.value, 2);

            number_type const ratio
                = engine.evaluate(value, context, options).to_number()
                / engine.evaluate(limit, context, options).to_number()
                * engine.evaluate(width, context, options).to_number();

            out << round(ratio);
        }
    };

//
// with_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct with_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.reserved("with") >> engine.value >> engine.keyword("as") >> engine.name) >> engine.block
                >> AJG_TAG(engine.reserved("endwith"));
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            match_type  const& value = match(engine.value);
            match_type  const& body  = match(engine.block);
            string_type const& name  = match(engine.name)[id].str();

            context_type context_copy = context;
            context_copy[name] = engine.evaluate(value, context, options);
            engine.render_block(out, body, context_copy, options);
        }
    };

//
// library_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct library_tag {
        static regex_type syntax(engine_type& engine) {
            return AJG_TAG(engine.unreserved_name >> engine.arguments) >> engine.block;
        }

        static void render( engine_type  const& engine
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , out_type&           out
                          ) {
            typedef typename options_type::tag_type tag_type;

            string_type const& name = match(engine.unreserved_name)[id].str();
            match_type  const& args = match(engine.arguments);
            match_type  const& body = match(engine.block);
            tag_type    const& tag  = get_library_tag(name, context, options);

            arguments_type arguments    = engine.evaluate_arguments(args, context, options);
            context_type   context_copy = context;
            options_type   options_copy = options;

            if (value_type const& value = tag(options_copy, &context_copy, arguments)) {
                out << value;
            }
            engine.render_block(out, body, context_copy, options_copy);
        }

      private:

        inline static typename options_type::tag_type const& get_library_tag
                ( string_type  const& name
                , context_type const& context
                , options_type const& options
                ) {
            typename options_type::tags_type::const_iterator it = options.loaded_tags.find(name);

            if (it != options.loaded_tags.end()) {
                return it->second;
            }
            throw_exception(missing_tag(traits_type::narrow(name)));
        }
    };

#undef AJG_TAG

}; // builtin_tags

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_BUILTIN_TAGS_HPP_INCLUDED
