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
using detail::get_nested;
using detail::operator ==;
using namespace detail::placeholders;

#define TAG(content) \
    engine.block_open >> *xpressive::_s >> content >> *xpressive::_s >> engine.block_close

//
// autoescape_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct autoescape_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            using namespace xpressive;
            return TAG(engine.name("autoescape") >> *_s >> (s1 = as_xpr("on") | "off")) >> engine.block
                >> TAG(engine.name("endautoescape"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            bool const flag = match[1].str() == text("on");
            Match const& block = match(engine.block);

            Options options_copy = options; // NOTE: Don't make the copy const.
            options_copy.autoescape = flag;
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
            using namespace xpressive;
            return TAG(engine.name("block") >> *_s >> (s1 = engine.identifier)) >> engine.block
                >> TAG(engine.name("endblock") >> !(*_s >> (s2 = engine.identifier)));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options&       options
                   , typename Engine::stream_type& out) const {
            Match const& block = match(engine.block);
            String const name  = match[1].str();

            if (match[2] && name != match[2].str()) {
                std::string const name_ = engine.template transcode<char>(name);
                std::string const message = "mismatched endblock tag for " + name_;
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
// csrf_token_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

struct csrf_token_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine& engine) const {
            using namespace xpressive;
            return TAG(engine.name("csrf_token"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {

            if (optional<Value const&> const token = detail::find_value(text("csrf_token"), context)) {
                if (token->to_string() != text("NOTPROVIDED")) {
                    out << "<div style='display:none'>";
                    out <<     "<input ";
                    out <<          "type='hidden' ";
                    out <<          "name='csrfmiddlewaretoken' ";
                    out <<          "value='" << *token;
                    out <<      "' />";
                    out << "</div>";
                }
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
            using namespace xpressive;
            return // Short form
                   engine.comment_open
                       // Assert no closing token or newlines.
                       >> *(~before(engine.comment_close
                                   | as_xpr('\n')) >> _)
                >> engine.comment_close
                   // Long form
                |  TAG(engine.name("comment")) >> engine.block
                >> TAG(engine.name("endcomment"));
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
            using namespace xpressive;
            return TAG(engine.name("cycle") >> engine.arguments >> !(*_s >> engine.keyword("as") >> *_s >> engine.identifier)) >> engine.block;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options&       options
                   , typename Engine::stream_type& out) const {
            typename Engine::size_type const position = match.position();
            Match const& args  = match(engine.arguments);
            Match const& ident = match(engine.identifier);
            Match const& block = match(engine.block);
            Size const total   = args.nested_results().size();
            Size const current = options.cycles_[position];

            Match const& arg = *detail::advance(args.nested_results(), current);
            Value const value = engine.evaluate(arg, context, options);
            options.cycles_[position] = (current + 1) % total;
            out << value;

            if (!ident) {
                // E.g. cycle foo
                engine.render_block(out, block, context, options);
            }
            else {
                // E.g. cycle foo as bar
                String const name = ident.str();
                Context context_copy = context;
                context_copy[name] = value;
                engine.render_block(out, block, context_copy, options);
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
            return TAG(engine.name("debug"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
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
            using namespace xpressive;
            return TAG(engine.name("extends") >> *_s >> engine.string_literal) >> engine.block;
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
            using namespace xpressive;
            return TAG(engine.name("firstof") >> engine.arguments >> *_s >> !engine.string_literal);
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
            using namespace xpressive;
            return TAG(engine.name("filter") >> *_s >> engine.pipe) >> engine.block
                >> TAG(engine.name("endfilter"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& pipe = match(engine.pipe);
            Match const& body = match(engine.block);

            std::basic_ostringstream<Char> stream;
            engine.render_block(stream, body, context, options);
            out << engine.apply_pipe(stream.str(), pipe, context, options);
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
            using namespace xpressive;
            Regex const variables = (s1 = engine.identifier)
                >> !(*_s >> ',' >> *_s >> (s2 = engine.identifier))
                ;
            return TAG(engine.name("for") >> *_s >> variables >> *_s >> engine.keyword("in") >> *_s >> engine.expression) >> engine.block // _, B, _, D, E
              >> !(TAG(engine.name("empty")) >> engine.block)                                                                             // _, G
                >> TAG(engine.name("endfor"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match  const& vars   = get_nested<B>(match);
            Match  const& expr   = get_nested<D>(match);
            Match  const& for_   = get_nested<E>(match);
            Match  const& empty  = get_nested<G>(match);
            String const& first  = vars[1].str();
            String const& second = vars[2].str();
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
                        context_copy[first] = boost::ref(item[0]);
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
            using namespace xpressive;
            return TAG(engine.name("if") >> engine.expression) >> engine.block // _, B, C
              >> !(TAG(engine.name("else")) >> engine.block)                   // _, E
                >> TAG(engine.name("endif"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& expr  = get_nested<B>(match);
            Match const& if_   = get_nested<C>(match);
            Match const& else_ = get_nested<E>(match);
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
            using namespace xpressive;
            return TAG(engine.name("ifchanged") >> engine.arguments) >> engine.block // _, B, C
              >> !(TAG(engine.name("else"))     >> engine.block)                     // _, E
              >>   TAG(engine.name("endifchanged"));
        }

        // template <class Parameters>
        // void render(Parameters const& params, Stream& out) const {

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options&       options
                   , typename Engine::stream_type& out) const {

            Match const& vars  = get_nested<B>(match);
            Match const& if_   = get_nested<C>(match);
            Match const& else_ = get_nested<E>(match);

            typename Engine::size_type const position = match.position();
            optional<Value const&> const value = detail::find_value(position, options.changes_);

            // This is the case with no variables (compare contents).
            if (vars.nested_results().empty()) {
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

                BOOST_FOREACH(Match const& var, vars.nested_results()) {
                    String const name = var.str();
                    values[name] = engine.evaluate(var, context, options);
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
            using namespace xpressive;
            return TAG(engine.name("ifequal") >> *_s >> engine.expression >> *_s >> engine.expression) >> engine.block // _, B, C, D
              >> !(TAG(engine.name("else")) >> engine.block)                                                           // _, F
              >>   TAG(engine.name("endifequal"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& left  = get_nested<B>(match);
            Match const& right = get_nested<C>(match);
            Match const& if_   = get_nested<D>(match);
            Match const& else_ = get_nested<F>(match);
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
            using namespace xpressive;
            return TAG(engine.name("ifnotequal") >> *_s >> engine.expression >> *_s >> engine.expression) >> engine.block // _, B, C, D
              >> !(TAG(engine.name("else")) >> engine.block)                                                              // _, F
              >>   TAG(engine.name("endifnotequal"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& left  = get_nested<B>(match);
            Match const& right = get_nested<C>(match);
            Match const& if_   = get_nested<D>(match);
            Match const& else_ = get_nested<F>(match);
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
            using namespace xpressive;
            return TAG(engine.name("include") >> *_s >> engine.string_literal);
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
            using namespace xpressive;
            Regex const packages = +(*_s >> engine.package);

            return TAG(engine.name("load") >> packages)
                   >> engine.block;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& packages = match.nested_results().front();
            Match const& block    = match(engine.block);
            Options options_copy = options;
            Context context_copy = context;

            BOOST_FOREACH(Match const& package, packages.nested_results()) {
                String const library = package.str();
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
            using namespace xpressive;
            Regex const identifiers = +(*_s >> engine.identifier);
            return TAG(engine.name("load") >> identifiers >> *_s >> engine.keyword("from") >> *_s >> engine.package)
                   >> engine.block;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& identifiers = match.nested_results().front();
            Match const& package     = match(engine.package);
            Match const& block       = match(engine.block);
            String const library = package.str();
            std::vector<String> names;

            BOOST_FOREACH(Match const& identifier, identifiers.nested_results()) {
                names.push_back(identifier.str());
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
            using namespace xpressive;
            return TAG(engine.name("now") >> *_s >> engine.string_literal);
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
            using namespace xpressive;
            return TAG(engine.name("regroup") >> *_s >> engine.expression
                                              >> *_s >> engine.keyword("by")
                                              >> *_s >> engine.package
                                              >> *_s >> engine.keyword("as")
                                              >> *_s >> engine.identifier)
                   >> engine.block;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options&       options
                   , typename Engine::stream_type& out) const {
            Match  const& expr  = match(engine.expression);
            String const& attrs = match(engine.package).str();
            String const& name  = match(engine.identifier).str();
            Match  const& block = match(engine.block);

            Value        values;
            entries_type entries;
            try {
                values = engine.evaluate(expr, context, options);
            }
            // Fail silently in these cases:
            catch (missing_variable const&) { goto done; }
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
            using namespace xpressive;

            typename Engine::string_regex_type const
                tag_ = '<' >> *~(as_xpr('>')) >> '>';
                gap_ = (s1 = tag_) >> +_s >> (s2 = tag_);
        }

        Regex syntax(Engine& engine) const {
            using namespace xpressive;
            return TAG(engine.name("spaceless")) >> engine.block
                >> TAG(engine.name("endspaceless"));
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
            using namespace xpressive;
            return TAG(engine.name("ssi") >> *_s >> (s1 = '/' >> +~_s) // absolute path
                                        >> !(+_s >> (s2 = "parsed")));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            String const path = match[xpressive::s1].str();
            bool const parsed = match[xpressive::s2].matched;

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
            using namespace xpressive;
            return TAG(engine.name("templatetag") >> *_s >> engine.identifier);
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            String const name = match(engine.identifier).str();
            Engine const& e = engine;

                 if (name == text("openbrace"))     out << e.brace_open;
            else if (name == text("closebrace"))    out << e.brace_close;
            else if (name == text("openblock"))     out << e.block_open;
            else if (name == text("closeblock"))    out << e.block_close;
            else if (name == text("opencomment"))   out << e.comment_open;
            else if (name == text("closecomment"))  out << e.comment_close;
            else if (name == text("openvariable"))  out << e.variable_open;
            else if (name == text("closevariable")) out << e.variable_close;
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
            using namespace xpressive;
            return TAG(engine.name("url") >> *_s >> engine.expression >> engine.arguments);
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
            using namespace xpressive;
            return TAG(engine.name("url") >> *_s >> engine.expression >> engine.arguments
                                          >> *_s >> engine.keyword("as") >> *_s >> engine.identifier)
                   >> engine.block;
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
            match_type const& ident = match(engine.identifier);

            value_type     const view      = engine.evaluate(expr, context, options);
            arguments_type const arguments = engine.evaluate_arguments(args, context, options);

            string_type const url  = engine.get_view_url(view, arguments, context, options).get_value_or(String());
            string_type const name = ident.str();

            context_type context_copy = context;
            context_copy[name] = url;
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
            using namespace xpressive;
            return engine.variable_open >> *_s >> engine.expression >> !(*_s >> '|' >> *_s >> engine.pipe) >> *_s
                >> engine.variable_close;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& expr = match(engine.expression);
            Match const& pipe = match(engine.pipe);
            Value value;

            try {
                value = engine.evaluate(expr, context, options);
                if (pipe) value = engine.apply_pipe(value, pipe, context, options);
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
            using namespace xpressive;
            return TAG(engine.name("verbatim")) >> engine.block
                >> TAG(engine.name("endverbatim"));
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
            using namespace xpressive;
            return TAG(engine.name("widthratio") // _
                >> *_s >> engine.expression      // B
                >> *_s >> engine.expression      // C
                >> *_s >> engine.expression      // D
            );
        }

        template <class T>
        inline static T round(T const r) {
            return (r > 0.0) ? std::floor(r + 0.5) : std::ceil(r - 0.5);
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& value = get_nested<B>(match);
            Match const& limit = get_nested<C>(match);
            Match const& width = get_nested<D>(match);

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
            using namespace xpressive;
            return TAG(engine.name("with") >> *_s >> engine.expression >> *_s
                           >> engine.keyword("as") >> *_s >> engine.identifier) >> engine.block
                >> TAG(engine.name("endwith"));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& expr  = match(engine.expression);
            Match const& name  = match(engine.identifier);
            Match const& body  = match(engine.block);

            Context context_copy = context;
            context_copy[name.str()] = engine.evaluate(expr, context, options);
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
            string_type const& name = match(engine.unreserved_name).str();
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
