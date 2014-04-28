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

#include <boost/algorithm/string/trim.hpp>

#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/adapters/map.hpp>
#include <ajg/synth/adapters/bool.hpp>
#include <ajg/synth/adapters/string.hpp>
#include <ajg/synth/adapters/vector.hpp>
#include <ajg/synth/adapters/numeric.hpp>
#include <ajg/synth/adapters/utility.hpp>
#include <ajg/synth/adapters/variant.hpp>
#include <ajg/synth/detail/filesystem.hpp>
#include <ajg/synth/detail/transformer.hpp>
#include <ajg/synth/engines/django/formatter.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace django {
namespace {
x::mark_tag const id(9);
}

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

    typedef typename value_type::behavior_type                                  behavior_type;
    typedef typename value_type::range_type                                     range_type;
    typedef typename value_type::sequence_type                                  sequence_type;
    typedef typename value_type::mapping_type                                   mapping_type;
    typedef typename value_type::traits_type                                    traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef detail::transformer<string_type>                                    transform;

  public:

    typedef void (*tag_type)( kernel_type  const& kernel
                            , match_type   const& match
                            , context_type const& context
                            , options_type const& options
                            , ostream_type&       ostream
                            );

  private:

    typedef std::map<id_type, tag_type>                                         tags_type;
    typedef std::basic_ostringstream<char_type>                                 string_stream_type;
    typedef formatter<options_type>                                             formatter_type;
    typedef typename options_type::arguments_type                               arguments_type;
    typedef typename arguments_type::second_type::value_type                    named_argument_type;

  public:

    inline void initialize(kernel_type& kernel) {
        kernel.tag
            = add(kernel, autoescape_tag::syntax(kernel),        autoescape_tag::render)
            | add(kernel, block_tag::syntax(kernel),             block_tag::render)
            | add(kernel, comment_tag::syntax(kernel),           comment_tag::render)
            | add(kernel, csrf_token_tag::syntax(kernel),        csrf_token_tag::render)
            | add(kernel, cycle_tag::syntax(kernel),             cycle_tag::render)
            | add(kernel, cycle_as_tag::syntax(kernel),          cycle_as_tag::render)
            | add(kernel, cycle_as_silent_tag::syntax(kernel),   cycle_as_silent_tag::render)
            | add(kernel, debug_tag::syntax(kernel),             debug_tag::render)
            | add(kernel, extends_tag::syntax(kernel),           extends_tag::render)
            | add(kernel, filter_tag::syntax(kernel),            filter_tag::render)
            | add(kernel, firstof_tag::syntax(kernel),           firstof_tag::render)
            | add(kernel, for_tag::syntax(kernel),               for_tag::render)
            | add(kernel, for_empty_tag::syntax(kernel),         for_empty_tag::render)
            | add(kernel, if_tag::syntax(kernel),                if_tag::render)
            | add(kernel, ifchanged_tag::syntax(kernel),         ifchanged_tag::render)
            | add(kernel, ifequal_tag::syntax(kernel),           ifequal_tag::render)
            | add(kernel, ifnotequal_tag::syntax(kernel),        ifnotequal_tag::render)
            | add(kernel, include_tag::syntax(kernel),           include_tag::render)
            | add(kernel, include_with_tag::syntax(kernel),      include_with_tag::render)
            | add(kernel, include_with_only_tag::syntax(kernel), include_with_only_tag::render)
            | add(kernel, load_tag::syntax(kernel),              load_tag::render)
            | add(kernel, load_from_tag::syntax(kernel),         load_from_tag::render)
            | add(kernel, now_tag::syntax(kernel),               now_tag::render)
            | add(kernel, regroup_tag::syntax(kernel),           regroup_tag::render)
            | add(kernel, spaceless_tag::syntax(kernel),         spaceless_tag::render)
            | add(kernel, ssi_tag::syntax(kernel),               ssi_tag::render)
            | add(kernel, templatetag_tag::syntax(kernel),       templatetag_tag::render)
            | add(kernel, url_tag::syntax(kernel),               url_tag::render)
            | add(kernel, url_as_tag::syntax(kernel),            url_as_tag::render)
            | add(kernel, variable_tag::syntax(kernel),          variable_tag::render)
            | add(kernel, verbatim_tag::syntax(kernel),          verbatim_tag::render)
            | add(kernel, widthratio_tag::syntax(kernel),        widthratio_tag::render)
            | add(kernel, with_tag::syntax(kernel),              with_tag::render)
            | add(kernel, library_tag::syntax(kernel),           library_tag::render)
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

// TODO[c++11]: Replace with function.
#define TAG(content) kernel.block_open >> *_s >> content >> *_s >> kernel.block_close

//
// autoescape_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct autoescape_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("autoescape") >> kernel.name) >> kernel.block
                >> TAG(kernel.reserved("endautoescape"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            string_type const& setting = match(kernel.name)[id].str();
            match_type  const& block   = match(kernel.block);
            boolean_type autoescape = true;

                 if (setting == traits_type::literal("on"))  autoescape = true;
            else if (setting == traits_type::literal("off")) autoescape = false;
            else throw_exception(std::invalid_argument("setting"));

            options_type options_copy = options; // NOTE: Don't make the copy const.
            options_copy.autoescape = autoescape;
            kernel.render_block(ostream, block, context, options_copy);
        }
    };

//
// block_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct block_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("block") >> kernel.name) >> kernel.block
                >> TAG(kernel.reserved("endblock") >> !kernel.name);
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type  const& block = match(kernel.block);
            match_type  const& open  = match(kernel.name, 0);
            match_type  const& close = match(kernel.name, 1);
            string_type const  name  = open[id].str();

            if (close && name != close[id].str()) {
                std::string const original = traits_type::narrow(name);
                throw_exception(std::invalid_argument("mismatched endblock tag for " + original));
            }

            if (options.top_level()) { // The block is being rendered directly.
                kernel.render_block(ostream, block, context, options);
                return;
            } // Else, the block is being derived from instead:

            if (optional<string_type> const& derived = options.get_block(name)) {
                ostream << *derived; // The base template is being rendered with derived blocks.
            }

            string_stream_type ss;
            { // TODO: Either make an options copy or make this strongly exception safe:
                string_type const previous_block = options.base_block_;
                const_cast<options_type&>(options).base_block_ = name;
                kernel.render_block(ss, block, context, options);
                const_cast<options_type&>(options).base_block_ = previous_block;
            }
            (*options.blocks_)[name] = ss.str();
        }
    };

//
// comment_tag
//     TODO: Split into comment_short_tag and comment_long_tag.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct comment_tag {
        static regex_type syntax(kernel_type& kernel) {
            // Short form (without a closing token or newlines) or long form, below.
            return kernel.comment_open >> *(~x::before(kernel.comment_close | _n) >> _) >> kernel.comment_close
                | TAG(kernel.reserved("comment")) >> kernel.block >> TAG(kernel.reserved("endcomment"));
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
// csrf_token_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct csrf_token_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("csrf_token"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {

            if (optional<value_type> const& token = detail::find(traits_type::literal("csrf_token"), context)) {
                string_type const& s = transform::escape_entities(token->to_string());

                if (s != traits_type::literal("NOTPROVIDED")) {
                    ostream << "<div style='display:none'>";
                    ostream << "<input type='hidden' name='csrfmiddlewaretoken' value='" << s << "' />";
                    ostream << "</div>";
                }
            }
        }
    };

//
// cycle_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct cycle_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("cycle") >> kernel.values >>
                !(kernel.keyword("as") >> kernel.name >> !(s1 = kernel.keyword("silent")))) >> kernel.block;
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type  const& vals     = match(kernel.values);
            match_type  const& block    = match(kernel.block);
            match_type  const& name     = match(kernel.name);
            size_type   const  position = match.position();
            size_type   const  total    = vals.nested_results().size();
            size_type   const  current  = detail::find(position, options.cycles_).get_value_or(0);

            match_type const& val   = *detail::advance_to(vals.nested_results().begin(), current);
            value_type const  value = kernel.evaluate(val, context, options);
            const_cast<options_type&>(options).cycles_[position] = (current + 1) % total;

            if (!name) { // e.g. cycle foo
                ostream << value;
                kernel.render_block(ostream, block, context, options);
            }
            else { // e.g. cycle foo as bar {silent}
                boolean_type const silent = match[s1].matched;
                if (!silent) ostream << value;

                context_type context_copy = context;
                context_copy[name[id].str()] = value;
                kernel.render_block(ostream, block, context_copy, options);
            }
        }
    };

//
// cycle_as_tag
//     NOTE: This is a no-op, since the as clause is already handled by cycle_tag.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct cycle_as_tag {
        static regex_type syntax(kernel_type& kernel) { return kernel.nothing; }
        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {}
    };

//
// cycle_as_silent_tag
//     NOTE: This is a no-op, since the as ... silent clause is already handled by cycle_tag.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct cycle_as_silent_tag {
        static regex_type syntax(kernel_type& kernel) { return kernel.nothing; }
        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {}
    };

//
// debug_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct debug_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("debug"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            ostream << "<h1>Context:</h1>" << std::endl;

            BOOST_FOREACH(typename context_type::value_type const& p, context) {
                ostream << "    " << value_type(p.first).escape()
                    << " = "  << value_type(p.second).escape() << "<br />" << std::endl;
            }
        }
    };

//
// extends_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct extends_tag {
        static regex_type syntax(kernel_type& kernel) {
            // TODO: Handle non-string literals bases, which should be treated not as paths but as
            //       template objects.
            return TAG(kernel.reserved("extends") >> kernel.string_literal) >> kernel.block;
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type const& body = match(kernel.block);
            path_type  const  path = kernel.extract_path(match(kernel.string_literal));

            std::basic_ostream<char_type> null_stream(0);
            typename options_type::blocks_type blocks;

            options_type options_copy = options;
            options_copy.blocks_ = &blocks;

            // First, render the base template as if it were stand-alone, so that block.super is
            // available to the derived template; non-block content is discarded.
            kernel.render_path(null_stream, path, context, options_copy);

            options_copy = options; // Discard non-block modifications to options.
            options_copy.blocks_ = &blocks;

            // Second, render any blocks in the derived template, while making the base template's
            // versions available to the derivee as block.super; non-block content is discarded.
            kernel.render_block(null_stream, body, context, options_copy);

            options_copy = options; // Discard non-block modifications to options.
            options_copy.blocks_ = &blocks;

            // Third, render the base template again with any (possibly) overridden blocks.
            // TODO: Parse and generate the result once and reuse it or have render_path do caching.
            kernel.render_path(ostream, path, context, options_copy);
        }
    };

//
// firstof_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct firstof_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("firstof") >> kernel.values >> !kernel.string_literal);
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type const& vals = match(kernel.values);

            BOOST_FOREACH(match_type const& val, kernel.select_nested(vals, kernel.value)) {
                try {
                    if (value_type const value = kernel.evaluate(val, context, options)) {
                        ostream << value;
                        break;
                    }
                }
                catch (missing_variable const&) {
                    // Missing variables count as 'False' ones.
                }
            }

            // Use the fallback, if there is one.
            if (match_type const& fallback = match(kernel.string_literal)) {
                ostream << kernel.extract_string(fallback);
            }
        }
    };

//
// filter_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct filter_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("filter") >> kernel.filters) >> kernel.block
                >> TAG(kernel.reserved("endfilter"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            string_stream_type ss;
            kernel.render_block(ss, match(kernel.block), context, options);
            ostream << kernel.apply_filters(ss.str(), match(kernel.filters), context, options);
        }
    };

//
// for_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct for_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("for") >> kernel.variable_names >> kernel.keyword("in")
                              >> kernel.value >> !(s1 = kernel.keyword("reversed"))) >> kernel.block
              >> !(TAG(kernel.reserved("empty")) >> kernel.block)
                >> TAG(kernel.reserved("endfor"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type   const& vars     = match(kernel.variable_names);
            match_type   const& for_     = match(kernel.block, 0);
            match_type   const& empty    = match(kernel.block, 1);
            boolean_type const  reversed = match[s1].matched;
            value_type          value    = kernel.evaluate(match(kernel.value), context, options);

            if (reversed) {
                value = value.reverse();
            }
        #if AJG_SYNTH_IS_COMPILER_GCC
            else {
                // FIXME: This extra copy shouldn't be necessary but it fixes the odd invalid reads in Travis.
                value = value.to_sequence();
            }
        #endif

            typename value_type::const_iterator it(value.begin()), end(value.end());
            typename options_type::names_type const& variables = kernel.extract_names(vars);

            if (it == end) {
                if (empty) { // for ... empty ... endfor case.
                    kernel.render_block(ostream, empty, context, options);
                }
                return;
            }

            context_type context_copy = context;
            size_type const n = variables.size();
            BOOST_ASSERT(n > 0);

            for (; it != end; ++it) {
                if (n == 1) { // e.g. for x in ...
                    context_copy[variables[0]] = /* XXX: boost::ref */(*it);
                }
                else {
                    size_type i = 0;
                    BOOST_FOREACH(value_type const& var, *it) { // e.g. for x, y, z in ...
                        if (i >= n) break;
                        context_copy[variables[i++]] = /* XXX: boost::ref */(var);
                    }

                    while (i < n) { // Overwrite the remaining vars in the context.
                        context_copy[variables[i++]] = options.default_value;
                    }
                }

                kernel.render_block(ostream, for_, context_copy, options);
            }
        }
    };

//
// for_empty_tag
//     NOTE: This is a no-op, since the empty clause is already handled by for_tag.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct for_empty_tag {
        static regex_type syntax(kernel_type& kernel) { return kernel.nothing; }
        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {}
    };

//
// if_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct if_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("if") >> kernel.value) >> kernel.block
              >> !(TAG(kernel.reserved("else")) >> kernel.block)
                >> TAG(kernel.reserved("endif"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type   const& if_   = match(kernel.block, 0);
            match_type   const& else_ = match(kernel.block, 1);
            boolean_type const  cond_ = kernel.evaluate(match(kernel.value), context, options);

                 if (cond_) kernel.render_block(ostream, if_,   context, options);
            else if (else_) kernel.render_block(ostream, else_, context, options);
        }
    };

//
// ifchanged_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct ifchanged_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("ifchanged") >> !kernel.values) >> kernel.block
              >> !(TAG(kernel.reserved("else"))     >> kernel.block)
              >>   TAG(kernel.reserved("endifchanged"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type const& if_   = match(kernel.block, 0);
            match_type const& else_ = match(kernel.block, 1);

            size_type const position = match.position();
            optional<value_type> const value = detail::find(position, options.changes_);

            if (match_type const& vals = match(kernel.values)) { // Compare variables.
                // NOTE: The key is a string (rather than e.g. an int) presumably in case variables are repeated.
                mapping_type values;

                BOOST_FOREACH(match_type const& val, kernel.select_nested(vals, kernel.value)) {
                    string_type const s = boost::algorithm::trim_copy(val.str());
                    values[s] = kernel.evaluate(val, context, options);
                }

                if (value && value->template as<mapping_type>() == values) {
                    if (else_) {
                        kernel.render_block(ostream, else_, context, options);
                    }
                }
                else {
                    const_cast<options_type&>(options).changes_[position] = values;
                    kernel.render_block(ostream, if_, context, options);
                }
            }
            else { // No variables, compare contents.
                string_stream_type ss;
                kernel.render_block(ss, if_, context, options);
                string_type const result = ss.str();

                if (value && value->template as<string_type>() == result) {
                    if (else_) {
                        kernel.render_block(ostream, else_, context, options);
                    }
                }
                else {
                    const_cast<options_type&>(options).changes_[position] = result;
                    ostream << result;
                }
            }
        }
    };

//
// ifequal_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct ifequal_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("ifequal") >> kernel.value >> kernel.value) >> kernel.block
              >> !(TAG(kernel.reserved("else")) >> kernel.block)
              >>   TAG(kernel.reserved("endifequal"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type   const& left  = match(kernel.value, 0);
            match_type   const& right = match(kernel.value, 1);
            match_type   const& if_   = match(kernel.block, 0);
            match_type   const& else_ = match(kernel.block, 1);
            boolean_type const  cond_ = kernel.evaluate(left, context, options) ==
                                        kernel.evaluate(right, context, options);

                 if (cond_) kernel.render_block(ostream, if_,   context, options);
            else if (else_) kernel.render_block(ostream, else_, context, options);
        }
    };

//
// ifnotequal_tag
// TODO: DRY this and ifequal_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct ifnotequal_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("ifnotequal") >> kernel.value >> kernel.value) >> kernel.block
              >> !(TAG(kernel.reserved("else")) >> kernel.block)
              >>   TAG(kernel.reserved("endifnotequal"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type   const& left  = match(kernel.value, 0);
            match_type   const& right = match(kernel.value, 1);
            match_type   const& if_   = match(kernel.block, 0);
            match_type   const& else_ = match(kernel.block, 1);
            boolean_type const  cond_ = kernel.evaluate(left, context, options) !=
                                        kernel.evaluate(right, context, options);

                 if (cond_) kernel.render_block(ostream, if_,   context, options);
            else if (else_) kernel.render_block(ostream, else_, context, options);
        }
    };

//
// include_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct include_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("include") >> kernel.string_literal >> *_s >>
                !(kernel.keyword("with") >> kernel.arguments >> !(s1 = kernel.keyword("only"))));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            path_type const path = kernel.extract_path(match(kernel.string_literal));

            if (match_type const& args = match(kernel.arguments)) {
                boolean_type const only = match[s1].matched;

                arguments_type const& arguments = kernel.evaluate_arguments(args, context, options);
                if (!arguments.first.empty()) {
                    AJG_SYNTH_THROW(std::invalid_argument("positional argument"));
                }

                context_type context_copy = only ? context_type() : context;
                BOOST_FOREACH(named_argument_type const& argument, arguments.second) {
                    context_copy[argument.first] = argument.second;
                }
                kernel.render_path(ostream, path, context_copy, options);
            }
            else {
                kernel.render_path(ostream, path, context, options);
            }
        }
    };

//
// include_with_tag
//     NOTE: This is a no-op, since the with clause is already handled by include_tag.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct include_with_tag {
        static regex_type syntax(kernel_type& kernel) { return kernel.nothing; }
        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {}
    };

//
// include_with_only_tag
//     NOTE: This is a no-op, since the with ... only clause is already handled by include_tag.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct include_with_only_tag {
        static regex_type syntax(kernel_type& kernel) { return kernel.nothing; }
        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {}
    };

//
// load_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct load_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("load") >> kernel.packages) >> kernel.block;
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type const& packages = match(kernel.packages);
            match_type const& block    = match(kernel.block);
            options_type options_copy  = options;
            context_type context_copy  = context;

            BOOST_FOREACH(match_type const& package, kernel.select_nested(packages, kernel.package)) {
                string_type const& library = package[id].str();
                kernel.load_library(context_copy, options_copy, library);
            }

            kernel.render_block(ostream, block, context_copy, options_copy);
        }
    };

//
// load_from_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct load_from_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("load") >> kernel.names >> kernel.keyword("from") >> kernel.package) >> kernel.block;
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type  const& names   = match(kernel.names);
            match_type  const& block   = match(kernel.block);
            string_type const& library = match(kernel.package)[id].str();
            std::vector<string_type> components;

            BOOST_FOREACH(match_type const& name, kernel.select_nested(names, kernel.name)) {
                components.push_back(name[id].str());
            }

            options_type options_copy = options;
            context_type context_copy = context;
            kernel.load_library(context_copy, options_copy, library, &components);
            kernel.render_block(ostream, block, context_copy, options_copy);
        }
    };

//
// now_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct now_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("now") >> kernel.string_literal);
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            string_type const format = kernel.extract_string(match(kernel.string_literal));
            ostream << formatter_type::format_datetime(options, format, traits_type::utc_datetime());
        }
    };

//
// regroup_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct regroup_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("regroup") >> kernel.value
                          >> kernel.keyword("by") >> kernel.package
                          >> kernel.keyword("as") >> kernel.name) >> kernel.block;
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type  const& expr  = match(kernel.value);
            string_type const& attrs = match(kernel.package)[id].str();
            string_type const& name  = match(kernel.name)[id].str();
            match_type  const& block = match(kernel.block);

            value_type        values;
            entries_type entries;
            try {
                values = kernel.evaluate(expr, context, options);
            }
            // Fail silently in these cases:
            catch (missing_variable  const&) { goto done; }
            catch (missing_attribute const&) { goto done; }
            entries = regroup(values, attrs);

          done:
            context_type context_copy = context;
            context_copy[name] = entries;
            kernel.render_block(ostream, block, context_copy, options);
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
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("spaceless")) >> kernel.block
                >> TAG(kernel.reserved("endspaceless"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            static typename kernel_type::string_regex_type const tag = '<' >> *~(as_xpr('>')) >> '>';
            static typename kernel_type::string_regex_type const gap = (s1 = tag) >> +_s >> (s2 = tag);

            string_stream_type ss;
            std::ostreambuf_iterator<char_type> it(ostream);

            match_type const& body = match(kernel.block);
            kernel.render_block(ss, body, context, options);
            // TODO: Use bidirectional_input_stream to feed directly to regex_replace.
            string_type const string = ss.str();
            x::regex_replace(it, string.begin(), string.end(), gap, traits_type::literal("$1$2"));
        }
    };

//
// ssi_tag:
//     TODO: Take into account `ALLOWED_INCLUDE_ROOTS`.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct ssi_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("ssi") >> kernel.value >> !(s1 = kernel.keyword("parsed")));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            value_type   const value  = kernel.evaluate(match(kernel.value), context, options);
            path_type    const path   = traits_type::to_path(value.to_string());
            boolean_type const parsed = match[s1].matched;

            if (!detail::is_absolute(path)) {
                throw_exception(std::invalid_argument("relative path"));
            }

            if (parsed) {
                kernel.render_path(ostream, path, context, options);
            }
            else {
                string_type line;
                std::string const path_ = traits_type::narrow(path);
                std::basic_ifstream<char_type> file(path_.c_str());
                while (std::getline(file, line)) ostream << line << kernel.newline;
            }
        }
    };

//
// templatetag_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct templatetag_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("templatetag") >> kernel.name);
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            string_type const name = match(kernel.name)[id].str();

            if (optional<string_type> const& marker = detail::find(name, kernel.markers)) {
                ostream << *marker;
            }
            else {
                ostream << options.default_value;
            }
        }
    };

//
// url_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct url_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("url") >> kernel.value >> kernel.arguments);
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type const& expr = match(kernel.value);
            match_type const& args = match(kernel.arguments);

            value_type     const view      = kernel.evaluate(expr, context, options);
            arguments_type const arguments = kernel.evaluate_arguments(args, context, options);

            if (optional<string_type> const& url = kernel.get_view_url(view, arguments, context, options)) {
                ostream << *url;
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
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("url") >> kernel.value >> kernel.arguments
                        >> kernel.keyword("as") >> kernel.name) >> kernel.block;
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type  const& expr  = match(kernel.value);
            match_type  const& args  = match(kernel.arguments);
            match_type  const& block = match(kernel.block);
            string_type const& name  = match(kernel.name)[id].str();

            value_type     const view      = kernel.evaluate(expr, context, options);
            arguments_type const arguments = kernel.evaluate_arguments(args, context, options);
            string_type    const url       = kernel.get_view_url(view, arguments, context, options)
                                                   .get_value_or(string_type());

            context_type context_copy = context;
            context_copy[name] = url;
            kernel.render_block(ostream, block, context_copy, options);
        }
    };

//
// variable_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct variable_tag {
        static regex_type syntax(kernel_type& kernel) {
            return kernel.variable_open >> *_s >> kernel.value >> kernel.variable_close;
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            value_type const& value = kernel.evaluate(match(kernel.value), context, options);
            boolean_type const safe = !options.autoescape || value.safe();
            safe ? ostream << value : ostream << value.escape();
        }
    };

//
// verbatim_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct verbatim_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("verbatim")) >> kernel.block
                >> TAG(kernel.reserved("endverbatim"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            ostream << match(kernel.block).str();
        }
    };

//
// widthratio_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct widthratio_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("widthratio") >> kernel.value >> kernel.value >> kernel.value);
        }

        template <class T>
        inline static T round(T const r) {
            return (r > 0.0) ? std::floor(r + 0.5) : std::ceil(r - 0.5);
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type const& value = match(kernel.value, 0);
            match_type const& limit = match(kernel.value, 1);
            match_type const& width = match(kernel.value, 2);

            number_type const ratio
                = kernel.evaluate(value, context, options).to_number()
                / kernel.evaluate(limit, context, options).to_number()
                * kernel.evaluate(width, context, options).to_number();

            ostream << round(ratio);
        }
    };

//
// with_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct with_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.reserved("with") >> kernel.value >> kernel.keyword("as") >> kernel.name) >> kernel.block
                >> TAG(kernel.reserved("endwith"));
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            match_type  const& value = match(kernel.value);
            match_type  const& body  = match(kernel.block);
            string_type const& name  = match(kernel.name)[id].str();

            context_type context_copy = context;
            context_copy[name] = kernel.evaluate(value, context, options);
            kernel.render_block(ostream, body, context_copy, options);
        }
    };

//
// library_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct library_tag {
        static regex_type syntax(kernel_type& kernel) {
            return TAG(kernel.unreserved_name >> kernel.arguments) >> kernel.block;
        }

        static void render( kernel_type  const& kernel
                          , match_type   const& match
                          , context_type const& context
                          , options_type const& options
                          , ostream_type&       ostream
                          ) {
            typedef typename options_type::tag_type tag_type;

            string_type const& name = match(kernel.unreserved_name)[id].str();
            match_type  const& args = match(kernel.arguments);
            match_type  const& body = match(kernel.block);
            tag_type    const& tag  = get_library_tag(name, context, options);

            arguments_type arguments    = kernel.evaluate_arguments(args, context, options);
            context_type   context_copy = context;
            options_type   options_copy = options;

            if (value_type const& value = tag(options_copy, context_copy, arguments)) {
                ostream << value;
            }
            kernel.render_block(ostream, body, context_copy, options_copy);
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

#undef TAG

}; // builtin_tags

}}}} // namespace ajg::synth::engines::django

#endif // AJG_SYNTH_ENGINES_DJANGO_BUILTIN_TAGS_HPP_INCLUDED
