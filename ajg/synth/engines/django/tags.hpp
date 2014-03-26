//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_TAGS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_TAGS_HPP_INCLUDED

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

namespace ajg {
namespace synth {

//
// forward declaration of engine exceptions
////////////////////////////////////////////////////////////////////////////////////////////////////

struct missing_variable;
struct missing_attribute;

namespace django {

using detail::text;
using detail::operator ==;
using boost::xpressive::_s;
using boost::xpressive::s1;
using boost::xpressive::s2;
using boost::xpressive::s3;
using boost::xpressive::s4;
boost::xpressive::mark_tag const id(9);

#define TAG(content) \
    engine.block_open >> *_s >> content >> *_s >> engine.block_close

//
// autoescape_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct autoescape_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("autoescape") >> engine.name) >> engine.block
                >> TAG(engine.reserved("endautoescape"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            String const& setting = match(engine.name)[id].str();
            Match  const& block   = match(engine.block);
            bool autoescape = true;

            if      (setting == detail::text("on"))  autoescape = true;
            else if (setting == detail::text("off")) autoescape = false;
            else throw_exception(std::invalid_argument("setting"));

            Options options_copy = options; // NOTE: Don't make the copy const.
            options_copy.autoescape = autoescape;
            engine.render_block(out, block, context, options_copy);
        }
    };
};

//
// block_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct block_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("block") >> (s1 = engine.name)) >> engine.block
                >> TAG(engine.reserved("endblock") >> !(s2 = engine.name));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options&       options
                   , typename Engine::stream_type& out) const {
            Match  const& block = match(engine.block);
            String const& name  = match(s1)[id].str();
            String const& close = match(s2)[id].str();

            if (!close.empty() && name != close) {
                std::string const original = engine.template transcode<char>(name);
                std::string const message  = "mismatched endblock tag for " + original;
                throw_exception(std::logic_error(message));
            }

            if (options.blocks_) { // We're being inherited from.
                if (optional<String const&> const overriden =
                    detail::find_value(name, *options.blocks_)) {
                    out << *overriden;
                }
                else {
                    std::basic_ostringstream<Char> stream;
                    engine.render_block(stream, block, context, options);
                    String const result = stream.str();
                    (*options.blocks_)[name] = result;
                    out << result;
                }
            }
            else { // We're being rendered directly.
                engine.render_block(out, block, context, options);
            }
        }
    };
};

//
// comment_tag
//     TODO: Split into comment_short_tag and comment_long_tag.
////////////////////////////////////////////////////////////////////////////////////////////////////

struct comment_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            namespace x = boost::xpressive;
            return // Short form; assert no closing token or newlines.
                   engine.comment_open >> *(~x::before(engine.comment_close | x::_n) >> x::_)
                >> engine.comment_close
                   // Long form
                |  TAG(engine.reserved("comment")) >> engine.block
                >> TAG(engine.reserved("endcomment"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            // Do nothing.
        }
    };
};

//
// cycle_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct cycle_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("cycle") >> engine.arguments >> !(engine.keyword("as") >> engine.name)) >> engine.block;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options&       options
                   , typename Engine::stream_type& out) const {
            typename Engine::size_type const position = match.position();
            Match const& args  = match(engine.arguments);
            Match const& name  = match(engine.name);
            Match const& block = match(engine.block);
            Size const total   = args.nested_results().size();
            Size const current = options.cycles_[position];

            Match const& arg   = *detail::advance(args.nested_results(), current);
            Value const  value = engine.evaluate(arg, context, options);
            options.cycles_[position] = (current + 1) % total;
            out << value;

            if (!name) {
                // E.g. cycle foo
                engine.render_block(out, block, context, options);
            }
            else {
                // E.g. cycle foo as bar
                Context context_copy = context;
                context_copy[name[id].str()] = value;
                engine.render_block(out, block, context_copy, options);
            }
        }
    };
};

//
// csrf_token_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct csrf_token_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("csrf_token"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {

            if (optional<Value const&> const token = detail::find_value(text("csrf_token"), context)) {
                String const& s = detail::escape_entities(token->to_string());

                if (s != text("NOTPROVIDED")) {
                    out << "<div style='display:none'>";
                    out << "<input type='hidden' name='csrfmiddlewaretoken' value='" << s << "' />";
                    out << "</div>";
                }
            }
        }
    };
};

//
// debug_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct debug_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("debug"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
          /*
          #ifdef NDEBUG
            out << "NDEBUG is defined." << std::endl;
          #else
            out << "NDEBUG is not defined." << std::endl;
          #endif
          #ifdef _DEBUG
            out << "_DEBUG is defined." << std::endl;
          #else
            out << "_DEBUG is not defined." << std::endl;
          #endif
          */
            out << "\nContext:\n" << std::endl;

            BOOST_FOREACH(typename Context::value_type const& p, context) {
                out << "    " << Value(p.first).escape()
                    << " = "  << Value(p.second).escape() << std::endl;
            }
        }
    };
};

//
// extends_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct extends_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("extends") >> engine.string_literal) >> engine.block;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            typedef typename Options::blocks_type    blocks_type;
            typedef typename blocks_type::value_type block_type;

            Match  const& string   = match(engine.string_literal);
            Match  const& body     = match(engine.block);
            String const  filepath = engine.extract_string(string);

            std::basic_ostream<Char> null_stream(0);
            blocks_type blocks, supers;
            Options options_copy = options;
            Context context_copy = context;

            options_copy.blocks_ = &blocks; // Set it.

            // First, we render the parent template as if it were
            // stand-alone, so that we can make block.super
            // available to the derived template. We don't care
            // about non-block content, so it is discarded.
            engine.render_file(null_stream, filepath, context, options_copy);
            String const suffix = text("_super");

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
};

//
// firstof_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct firstof_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("firstof") >> engine.arguments >> !engine.string_literal);
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            BOOST_FOREACH(Match const& var, match(engine.arguments).nested_results()) {
                try {
                    if (Value const value = engine.evaluate(var, context, options)) {
                        out << value;
                    }
                }
                catch (missing_variable const&) {
                    // Missing variables count as 'False' ones.
                }
            }

            // Use the fallback, if there is one.
            if (Match const& fallback = match(engine.string_literal)) {
                out << engine.extract_string(fallback);
            }
        }
    };
};

//
// filter_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct filter_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("filter") >> engine.pipeline) >> engine.block
                >> TAG(engine.reserved("endfilter"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& filters = match(engine.pipeline);
            Match const& body    = match(engine.block);

            std::basic_ostringstream<Char> stream;
            engine.render_block(stream, body, context, options);
            out << engine.apply_filters(stream.str(), filters, context, options);
        }
    };
};

//
// for_tag
// TODO: for ... in ... _reversed_, using BOOST_FOREACH_REVERSE.
////////////////////////////////////////////////////////////////////////////////////////////////////

struct for_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("for") >> engine.variables >> engine.keyword("in") >> engine.expression) >> engine.block
              >> !(TAG(engine.reserved("empty")) >> engine.block)
                >> TAG(engine.reserved("endfor"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match  const& vars   = match(engine.variables);
            Match  const& expr   = match(engine.expression);
            Match  const& for_   = match(engine.block, 0);
            Match  const& empty  = match(engine.block, 1);
            String const& first  = vars(engine.name, 0)[id].str();
            String const& second = vars(engine.name, 1)[id].str();
            Value  const& value  = engine.evaluate(expr, context, options);

            typename Value::const_iterator it(value.begin()), end(value.end());

            if (it != end) { // Not empty.
                Context context_copy = context;
                uintmax_t i = 0;

                for (; it != end; ++it, ++i) {
                    if (second.empty()) { // e.g. for i in ...
                        context_copy[first] = boost::ref(*it);
                    }
                    else { // e.g. for k, v in ...
                        // context_copy[first] = i;
                        // context_copy[second] = boost::ref(*it);

                        Value const item = *it;

                        if (item.length() < 2) {
                            throw_exception(std::out_of_range("item"));
                        }
                        context_copy[first]  = boost::ref(item[0]);
                        context_copy[second] = boost::ref(item[1]);
                    }
                    engine.render_block(out, for_, context_copy, options);
                }
            }
            // for ... empty ... endfor case.
            else if (empty) {
                engine.render_block(out, empty, context, options);
            }
        }
    };
};

//
// for_empty_tag
// NOTE: This is a no-op, since the empty clause is already handled by for_tag.
////////////////////////////////////////////////////////////////////////////////////////////////////

struct for_empty_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const { return engine.nothing; }
        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {}
    };
};

//
// if_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct if_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("if") >> engine.expression) >> engine.block
              >> !(TAG(engine.reserved("else")) >> engine.block)
                >> TAG(engine.reserved("endif"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& expr  = match(engine.expression);
            Match const& if_   = match(engine.block, 0);
            Match const& else_ = match(engine.block, 1);
            bool  const  cond_ = engine.evaluate(expr, context, options);

                 if (cond_) engine.render_block(out, if_,   context, options);
            else if (else_) engine.render_block(out, else_, context, options);
        }
    };
};

//
// ifchanged_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ifchanged_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("ifchanged") >> engine.arguments) >> engine.block
              >> !(TAG(engine.reserved("else"))     >> engine.block)
              >>   TAG(engine.reserved("endifchanged"));
        }

        // template <class Parameters>
        // void render(Parameters const& params, Stream& out) const {

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options&       options
                   , typename Engine::stream_type& out) const {

            Match const& args  = match(engine.arguments);
            Match const& if_   = match(engine.block, 0);
            Match const& else_ = match(engine.block, 1);

            typename Engine::size_type const position = match.position();
            optional<Value const&> const value = detail::find_value(position, options.changes_);

            // This is the case with no variables (compare contents).
            if (args.nested_results().empty()) {
                std::basic_ostringstream<Char> stream;
                engine.render_block(stream, if_, context, options);
                String const result = stream.str();

                if (value && *value == Value/*ref*/(result)) {
                    if (else_) {
                        engine.render_block(out, else_, context, options);
                    }
                }
                else {
                    options.changes_[position] = result;
                    out << result;
                }
            }
            // Here, we compare variables.
            else {
                // NOTE: The key is a string (rather than an int) presumably in case variables are repeated.
                std::map<String, Value> values;

                BOOST_FOREACH(Match const& arg, args.nested_results()) {
                    String const s = algorithm::trim_copy(arg.str());
                    values[s] = engine.evaluate(arg, context, options);
                }

                if (value && *value == Value/*ref*/(values)) {
                    if (else_) {
                        engine.render_block(out, else_, context, options);
                    }
                }
                else {
                    options.changes_[position] = values;
                    engine.render_block(out, if_, context, options);
                }
            }
        }
    };
};

//
// ifequal_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ifequal_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("ifequal") >> engine.expression >> engine.expression) >> engine.block
              >> !(TAG(engine.reserved("else")) >> engine.block)
              >>   TAG(engine.reserved("endifequal"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            AJG_DUMP(bool(match));
            AJG_DUMP(match.str());
            Match const& left  = match(engine.expression, 0);
            Match const& right = match(engine.expression, 1);
            Match const& if_   = match(engine.block, 0);
            Match const& else_ = match(engine.block, 1);
            AJG_DUMP(bool(left));
            AJG_DUMP(bool(right));
            AJG_DUMP(bool(if_));
            AJG_DUMP(bool(else_));
            AJG_DUMP(left.str());
            AJG_DUMP(right.str());
            AJG_DUMP(if_.str());
            AJG_DUMP(else_.str());
            bool  const  cond_ = engine.evaluate(left, context, options)
                              == engine.evaluate(right, context, options);

                 if (cond_) engine.render_block(out, if_,   context, options);
            else if (else_) engine.render_block(out, else_, context, options);
        }
    };
};

//
// ifnotequal_tag
// TODO: DRY this and ifequal_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ifnotequal_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("ifnotequal") >> engine.expression >> engine.expression) >> engine.block
              >> !(TAG(engine.reserved("else")) >> engine.block)
              >>   TAG(engine.reserved("endifnotequal"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& left  = match(engine.expression, 0);
            Match const& right = match(engine.expression, 1);
            Match const& if_   = match(engine.block, 0);
            Match const& else_ = match(engine.block, 1);
            bool  const  cond_ = engine.evaluate(left, context, options)
                              != engine.evaluate(right, context, options);

                 if (cond_) engine.render_block(out, if_,   context, options);
            else if (else_) engine.render_block(out, else_, context, options);
        }
    };
};

//
// include_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct include_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("include") >> engine.string_literal);
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            String const path = engine.extract_string(match(engine.string_literal));
            engine.render_file(out, path, context, options);
        }
    };
};

//
// load_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct load_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("load") >> engine.packages) >> engine.block;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& packages = match(engine.packages);
            Match const& block    = match(engine.block);
            Options options_copy = options;
            Context context_copy = context;

            BOOST_FOREACH(Match const& package, packages.nested_results()) {
                String const library = package[id].str();
                engine.load_library(context_copy, options_copy, library);
            }

            engine.render_block(out, block, context_copy, options_copy);
        }
    };
};

//
// load_from_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct load_from_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("load") >> engine.names >> engine.keyword("from") >> engine.package) >> engine.block;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match  const& package = match(engine.package);
            Match  const& block   = match(engine.block);
            String const  library = package[id].str();
            std::vector<String> names;

            BOOST_FOREACH(Match const& name, match(engine.names).nested_results()) {
                names.push_back(name[id].str());
            }

            Options options_copy = options;
            Context context_copy = context;
            engine.load_library(context_copy, options_copy, library, &names);
            engine.render_block(out, block, context_copy, options_copy);
        }
    };
};

//
// now_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct now_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("now") >> engine.string_literal);
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            String const format = engine.extract_string(match(engine.string_literal));
            out << engine.format_datetime(options, format, detail::utc_now());
        }
    };
};

//
// regroup_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct regroup_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("regroup") >> engine.expression
                                              >> engine.keyword("by")
                                              >> engine.package
                                              >> engine.keyword("as")
                                              >> engine.name) >> engine.block;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options&       options
                   , typename Engine::stream_type& out) const {
            Match  const& expr  = match(engine.expression);
            String const& attrs = match(engine.package[id]).str();
            String const& name  = match(engine.name[id]).str();
            Match  const& block = match(engine.block);

            Value        values;
            entries_type entries;
            try {
                values = engine.evaluate(expr, context, options);
            }
            // Fail silently in these cases:
            catch (missing_variable  const&) { goto done; }
            catch (missing_attribute const&) { goto done; }
            entries = regroup(values, attrs);

          done:
            Context context_copy = context;
            context_copy[name] = entries;
            engine.render_block(out, block, context_copy, options);
        }

        typedef std::map<String, Value>         entry_type;
        typedef std::vector<entry_type>         entries_type;

        inline static entries_type regroup(Value const& values, String const& attrs) {
            static String const grouper_(detail::text("grouper")),
                                list_   (detail::text("list"));
            entries_type entries;

            BOOST_FOREACH(typename Value::group_type const& group, values.group_by(attrs)) {
                Value const key   = group.first;
                Value const value = group.second;
                entry_type entry;
                entry[grouper_] = key;
                entry[list_]    = value;
                entries.push_back(entry);
            }

            return entries;
        }
    };
};

//
// spaceless_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct spaceless_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        definition() {
            namespace x = boost::xpressive;
            typename Engine::string_regex_type const
                tag_ = '<' >> *~(x::as_xpr('>')) >> '>';
                gap_ = (s1 = tag_) >> +_s >> (s2 = tag_);
        }

        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("spaceless")) >> engine.block
                >> TAG(engine.reserved("endspaceless"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {

            std::basic_ostringstream<Char> stream;
            std::ostreambuf_iterator<Char> output(out);

            Match const& body = match(engine.block);
            engine.render_block(stream, body, context, options);
            // TODO: Figure out how to feed stream directly to regex_replace.
            //       std::istreambuf_iterator is almost good enough for that, but
            //       it is sadly not Bidirectional and thus lacks operator --.
            String const string = stream.str();
            regex_replace(output, string.begin(),
                string.end(), gap_, text("$1$2"));
        }

      private:

        typename Engine::string_regex_type gap_;
    };
};

//
// ssi_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ssi_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("ssi") >> (s1 = '/' >> +~_s) >> !(+_s >> (s2 = "parsed")) >> *_s);
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            String const path = match[s1].str();
            bool const parsed = match[s2].matched;

            if (parsed) {
                engine.render_file(out, path, context, options);
            }
            else {
                String line;
                std::string const path_ =
                    engine.template transcode<char>(path);
                std::basic_ifstream<Char> file(path_.c_str());
                while (std::getline(file, line)) out << line;
            }
        }
    };
};

//
// templatetag_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct templatetag_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("templatetag") >> engine.name);
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            String const name = match(engine.name[id]).str();

            if (optional<String const> const& marker = detail::find_mapped_value(name, engine.markers)) {
                out << *marker;
            }
            else {
                out << options.default_value;
            }
        }
    };
};

//
// url_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct url_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
      private:

        typedef Context     context_type;
        typedef Engine      engine_type;
        typedef Match       match_type;
        typedef Options     options_type;
        typedef Regex       regex_type;
        typedef String      string_type;
        typedef Value       value_type;

        typedef typename engine_type::arguments_type    arguments_type;
        typedef typename engine_type::stream_type       stream_type;

      public:

        regex_type syntax(engine_type& engine) const {
            return TAG(engine.reserved("url") >> engine.expression >> engine.arguments);
        }

        void render( match_type   const& match
                   , engine_type  const& engine
                   , context_type const& context
                   , options_type&       options
                   , stream_type&        out
                   ) const {
            match_type const& expr = match(engine.expression);
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
};

//
// url_as_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct url_as_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
      private:

        typedef Context     context_type;
        typedef Engine      engine_type;
        typedef Match       match_type;
        typedef Options     options_type;
        typedef Regex       regex_type;
        typedef String      string_type;
        typedef Value       value_type;

        typedef typename engine_type::arguments_type    arguments_type;
        typedef typename engine_type::stream_type       output_type;

      public:

        regex_type syntax(engine_type& engine) const {
            return TAG(engine.reserved("url") >> engine.expression >> engine.arguments
                       >> engine.keyword("as") >> engine.name) >> engine.block;
        }

        void render( match_type   const& match
                   , engine_type  const& engine
                   , context_type const& context
                   , options_type&       options
                   , output_type&        output
                   ) const {
            match_type const& expr  = match(engine.expression);
            match_type const& args  = match(engine.arguments);
            match_type const& block = match(engine.block);
            match_type const& name  = match(engine.name);

            value_type     const view      = engine.evaluate(expr, context, options);
            arguments_type const arguments = engine.evaluate_arguments(args, context, options);
            string_type    const url       = engine.get_view_url(view, arguments, context, options)
                                                   .get_value_or(string_type());

            context_type context_copy = context;
            context_copy[name[id].str()] = url;
            engine.render_block(output, block, context_copy, options);
        }
    };
};

//
// variable_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct variable_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return engine.variable_open >> *_s >> engine.expression >> engine.filters
                >> engine.variable_close;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& expr    = match(engine.expression);
            Match const& filters = match(engine.filters);
            Value value;

            try {
                value = engine.evaluate(expr, context, options);
                value = engine.apply_filters(value, filters, context, options);
            }
            catch (missing_variable  const&) { value = options.default_value; }
            catch (missing_attribute const&) { value = options.default_value; }

            bool const safe = !options.autoescape || value.safe();
            safe ? out << value : out << value.escape();
        }
    };
};

//
// verbatim_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct verbatim_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("verbatim")) >> engine.block
                >> TAG(engine.reserved("endverbatim"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            out << match(engine.block).str();
        }
    };
};

//
// widthratio_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct widthratio_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("widthratio") >> engine.expression >> engine.expression >> engine.expression);
        }

        template <class T>
        inline static T round(T const r) {
            return (r > 0.0) ? std::floor(r + 0.5) : std::ceil(r - 0.5);
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& value = match(engine.expression, 0);
            Match const& limit = match(engine.expression, 1);
            Match const& width = match(engine.expression, 2);

            typename Value::number_type const ratio
                = engine.evaluate(value, context, options).count()
                / engine.evaluate(limit, context, options).count()
                * engine.evaluate(width, context, options).count();

            out << round(ratio);
        }
    };
};

//
// with_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct with_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            return TAG(engine.reserved("with") >> engine.expression >> engine.keyword("as") >> engine.name) >> engine.block
                >> TAG(engine.reserved("endwith"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& expr  = match(engine.expression);
            Match const& name  = match(engine.name);
            Match const& body  = match(engine.block);

            Context context_copy = context;
            context_copy[name[id].str()] = engine.evaluate(expr, context, options);
            engine.render_block(out, body, context_copy, options);
        }
    };
};

//
// library_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct library_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
      private:

        typedef Context     context_type;
        typedef Engine      engine_type;
        typedef Match       match_type;
        typedef Options     options_type;
        typedef Regex       regex_type;
        typedef String      string_type;
        typedef Value       value_type;

        typedef typename engine_type::arguments_type    arguments_type;
        typedef typename engine_type::stream_type       output_type;
        typedef typename options_type::tag_type         tag_type;

      public:

        regex_type syntax(engine_type& engine) const {
            return TAG(engine.unreserved_name >> engine.arguments) >> engine.block;
        }

        void render( match_type   const& match
                   , engine_type  const& engine
                   , context_type const& context
                   , options_type&       options
                   , output_type&        output
                   ) const {
            string_type const& name = match(engine.unreserved_name)[id].str();
            match_type  const& args = match(engine.arguments);
            match_type  const& body = match(engine.block);

            arguments_type arguments    = engine.evaluate_arguments(args, context, options);
            context_type   context_copy = context;
            options_type   options_copy = options;

            if (optional<tag_type const> const& tag = detail::find_mapped_value(name, options_copy.loaded_tags)) {
                if (value_type const& value = (*tag)(options_copy, &context_copy, arguments)) {
                    output << value;
                }
                engine.render_block(output, body, context_copy, options_copy);
            }
            else {
                throw_exception(missing_tag(engine.template transcode<char>(name)));
            }
        }
    };
};

#undef TAG

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_TAGS_HPP_INCLUDED
