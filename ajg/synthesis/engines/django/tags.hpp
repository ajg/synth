//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTHESIS_ENGINES_DJANGO_TAGS_HPP_INCLUDED
#define AJG_SYNTHESIS_ENGINES_DJANGO_TAGS_HPP_INCLUDED

#include <string>
#include <locale>
#include <sstream>
#include <iterator>
#include <stdexcept>

#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include <boost/xpressive/xpressive.hpp>

#include <ajg/synthesis/engines/detail.hpp>
#include <ajg/synthesis/adapters/map.hpp>
#include <ajg/synthesis/adapters/bool.hpp>
#include <ajg/synthesis/adapters/string.hpp>
#include <ajg/synthesis/adapters/vector.hpp>
#include <ajg/synthesis/adapters/numeric.hpp>
#include <ajg/synthesis/adapters/utility.hpp>
#include <ajg/synthesis/adapters/variant.hpp>

namespace ajg {
namespace synthesis {

//
// forward declaration of engine exceptions
////////////////////////////////////////////////////////////////////////////////

struct missing_variable;
struct missing_attribute;

namespace django {

using detail::text;
using detail::get_nested;
using detail::operator ==;
using namespace detail::placeholders;

#define TAG(content) \
    engine.block_open >> *_s >> content >> *_s >> engine.block_close

//
// autoescape_tag
////////////////////////////////////////////////////////////////////////////////

struct autoescape_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("autoescape" >> +_s >> (s1 = as_xpr("on") | "off"))
                >> engine.block // A
                >> TAG("endautoescape");
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            bool const flag = match[1].str() == text("on");
            Match const& block = get_nested<A>(match);

            // Don't make copy const!
            Options copy = options;
            copy.autoescape = flag;
            engine.render_block(out, block, context, copy);
        }
    };
};

//
// block_tag
////////////////////////////////////////////////////////////////////////////////

struct block_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("block" >> +_s >> engine.identifier)        // A
                >> engine.block                                    // B
                >> TAG("endblock" >> !(+_s >> engine.identifier)); // C
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options&       options
                   , typename Engine::stream_type& out) const {
            Match const& name1 = get_nested<A>(match);
            Match const& block = get_nested<B>(match);
            Match const& name2 = get_nested<C>(match);
            String const name = name1.str();

            if (name2 && name != name2.str()) {
                std::string const name_ = engine.template convert<char>(name);
                std::string const message = "mismatched endblock tag for " + name_;
                throw_exception(std::logic_error(message));
            }

            if (options.blocks) { // We're being inherited from.
                if (optional<String const&> const overriden =
                    detail::find_value(name, *options.blocks)) {
                    out << *overriden;
                }
                else {
                    std::basic_ostringstream<Char> stream;
                    engine.render_block(stream, block, context, options);
                    String const result = stream.str();
                    (*options.blocks)[name] = result;
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
////////////////////////////////////////////////////////////////////////////////

struct csrf_token_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("csrf_token");
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {

            if (optional<Value const&> const token
                     = detail::find_value(text("csrf_token"), context)) {
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
////////////////////////////////////////////////////////////////////////////////

struct comment_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return // Short form
                   engine.comment_open
                       // Assert no closing token or newlines.
                       >> *(~before(engine.comment_close
                                   | as_xpr('\n')) >> _)
                >> engine.comment_close
                   // Long form
                |  TAG("comment") >> engine.block
                >> TAG("endcomment");
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
////////////////////////////////////////////////////////////////////////////////

struct cycle_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        struct not_as {
        bool operator ()(typename Match::value_type const& match) const {
            return match.str() != lexical_cast<String>("as");
        }};

        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            Regex const expressions = +(+_s >> engine.expression[ check(not_as()) ]);
            return TAG("cycle" >> expressions
                       >> !(+_s >> "as" >> +_s >> engine.identifier))
                   >> engine.block;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options&       options
                   , typename Engine::stream_type& out) const {
            typename Engine::iterator_type const it = match[0].first;
            // Alternative style to get_nested<X>(match):
            Match const& exprs = match.nested_results().front();
            Match const& ident = match(engine.identifier);
            Match const& block = match(engine.block);
            Size const total   = exprs.nested_results().size();
            Size const current = options.cycles[it];

            Match const& expr = *detail::advance(exprs.nested_results(), current);
            Value const value = engine.evaluate(expr, context, options);
            options.cycles[it] = (current + 1) % total;
            out << value;

            if (!ident) {
                // E.g. cycle foo
                engine.render_block(out, block, context, options);
            }
            else {
                // E.g. cycle foo as bar
                String const name = ident.str();
                Context copy = context;
                copy[name] = value;
                engine.render_block(out, block, copy, options);
            }
        }
    };
};

//
// debug_tag
////////////////////////////////////////////////////////////////////////////////

struct debug_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("debug");
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
////////////////////////////////////////////////////////////////////////////////

struct extends_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("extends" >> +_s
                    >> engine.string_literal) // A
                >> engine.block;              // B
        }

        /*struct blocks_type :  {


        };*/

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& string = get_nested<A>(match);
            Match const& body   = get_nested<B>(match);
            String const filepath = engine.extract_string(string);
            std::basic_ostream<Char> null_stream(0);
            std::map<String, String> blocks, supers;
            typedef std::pair<String, String> block_type;

            Options copy = options;
            copy.blocks = &blocks;

            Context __xxx__ = context;

            // First, we render the parent template as if it were
            // stand-alone, so that we can make block.super
            // available to the derived template. We don't care
            // about non-block content, so it is discarded.
            engine.render_file(null_stream, filepath, context, copy);
            String const suffix = text("_super");

            BOOST_FOREACH(block_type const& block, blocks) {
                __xxx__[block.first + suffix] = block.second;
            }

            // We only care about the supers; any other
            // modifications to the options are discarded.
            copy = options; // Reset copy.
            copy.blocks = &supers;

            // Second, we "extract" the blocks that the derived
            // template will be overriding, while at the same time
            // making the parent template's versions available
            // for the derivee to use as block.super. The derivee's
            // non-block content is irrelevant so it is discarded.
            engine.render_block(null_stream, body, __xxx__, copy);

            // We only care about the blocks; any other
            // modifications to the options are discarded.
            copy = options; // Reset copy.
            copy.blocks = &supers; //blocks;

            // Finally, we render the parent template with the
            // potentially overriden blocks already rendered.
            engine.render_file(out, filepath, context, copy);
        }
    };
};

//
// firstof_tag
////////////////////////////////////////////////////////////////////////////////

struct firstof_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("firstof"
                >> (Regex() = +(+_s >> engine.expression))  // A
                    >> *_s >> !engine.string_literal        // B
            );
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            BOOST_FOREACH(Match const& var, get_nested<A>(match).nested_results()) {
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
            if (Match const& fallback = get_nested<B>(match)) {
                out << engine.extract_string(fallback);
            }
        }
    };
};

//
// filter_tag
////////////////////////////////////////////////////////////////////////////////

struct filter_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("filter" >> +_s >> engine.pipe) // A
                    >> engine.block                    // B
                >> TAG("endfilter");
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& pipe = get_nested<A>(match);
            Match const& body = get_nested<B>(match);

            std::basic_ostringstream<Char> stream;
            engine.render_block(stream, body, context, options);
            out << engine.apply_pipe(stream.str(), pipe, context, options);
        }
    };
};

//
// for_tag
// TODO: for ... in ... _reversed_, using BOOST_FOREACH_REVERSE.
////////////////////////////////////////////////////////////////////////////////

struct for_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            Regex const variables = engine.identifier        // 1
                >> !(*_s >> ',' >> *_s >> engine.identifier) // 2
                ;
            return TAG("for" >> +_s >> variables                  // A
                       >> +_s >> "in" >> +_s >> engine.expression // B
                   ) >> engine.block                              // C
              >> !(TAG("empty") >> engine.block)                  // D
                >> TAG("endfor");
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match  const& vars   = get_nested<A>(match);
            Match  const& expr   = get_nested<B>(match);
            Match  const& for_   = get_nested<C>(match);
            Match  const& empty  = get_nested<D>(match);
            String const& first  = get_nested<1>(vars).str();
            String const& second = get_nested<2>(vars).str();
            Value  const& value  = engine.evaluate(expr, context, options);

            typename Value::const_iterator it(value.begin()), end(value.end());

            if (it != end) { // Not empty.
                Context copy = context;
                uintmax_t i = 0;

                for (; it != end; ++it, ++i) {
                    if (second.empty()) { // e.g. for i in ...
                        copy[first] = boost::ref(*it);
                    }
                    else { // e.g. for k, v in ...
                        copy[first] = i;
                        copy[second] = boost::ref(*it);
                    }
                    engine.render_block(out, for_, copy, options);
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
////////////////////////////////////////////////////////////////////////////////

struct for_empty_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {}
        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {}
    };
};

//
// if_tag
////////////////////////////////////////////////////////////////////////////////

struct if_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex boolean_expression, and_expression,
              or_expression, not_expression;

        void initialize(Engine const& engine) {
            using namespace xpressive;

            not_expression // not A
                = "not" >> +_s >> engine.expression
                ;
            and_expression // A and B
                = engine.expression >> +_s >> "and" >> +_s >> engine.expression
                ;
            or_expression // A or B
                = engine.expression >> +_s >> "or" >> +_s >> engine.expression
                ;
            boolean_expression // A
                = not_expression
                | and_expression
                | or_expression
                | engine.expression
                ;
        }

        bool evaluate( Match   const& expr,    Engine  const& engine
                     , Context const& context, Options const& options) const {
            if (expr == boolean_expression) {
                // Simply recurse down one 'nesting' level.
                return evaluate(get_nested<A>(expr), engine, context, options);
            }
            else if (expr == not_expression) {
                return !engine.evaluate(get_nested<A>(expr), context, options);
            }
            else if (expr == and_expression) {
                return engine.evaluate(get_nested<A>(expr), context, options)
                    && engine.evaluate(get_nested<B>(expr), context, options);
            }
            else if (expr == or_expression) {
                return engine.evaluate(get_nested<A>(expr), context, options)
                    || engine.evaluate(get_nested<B>(expr), context, options);
            }
            else {
                return engine.evaluate(expr, context, options);
            }
        }

        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("if" >> +_s >> boolean_expression) // A
                       >> engine.block                    // B
              >> !(TAG("else") >> engine.block)           // C
                >> TAG("endif");
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& expr  = get_nested<A>(match);
            Match const& if_   = get_nested<B>(match);
            Match const& else_ = get_nested<C>(match);
            bool  const  cond_ = evaluate(expr, engine, context, options);

                 if (cond_) engine.render_block(out, if_,   context, options);
            else if (else_) engine.render_block(out, else_, context, options);
        }
    };
};

//
// ifchanged_tag
////////////////////////////////////////////////////////////////////////////////

struct ifchanged_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("ifchanged"
                >> *(+_s >> engine.expression)  // A
                       ) >> engine.block        // B
              >> !(TAG("else") >> engine.block) // C
              >>   TAG("endifchanged");
        }

        // template <class Parameters>
        // void render(Parameters const& params, Stream& out) const {

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options&       options
                   , typename Engine::stream_type& out) const {

            Match const& vars  = get_nested<A>(match);
            Match const& if_   = get_nested<B>(match);
            Match const& else_ = get_nested<C>(match);

            typename Engine::iterator_type const key(match[0].first);
            optional<Value const&> const value = detail::find_value(key, options.registry);

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
                    options.registry[key] = result;
                    out << result;
                }
            }
            // Here, we compare variables.
            else {
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
                    options.registry[key] = values;
                    engine.render_block(out, if_, context, options);
                }
            }
        }
    };
};

//
// ifequal_tag
////////////////////////////////////////////////////////////////////////////////

struct ifequal_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("ifequal"
                    >> +_s >> engine.expression  // A
                    >> +_s >> engine.expression) // B
                               >> engine.block   // C
              >> !(TAG("else") >> engine.block)  // D
              >>   TAG("endifequal");
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& left  = get_nested<A>(match);
            Match const& right = get_nested<B>(match);
            Match const& if_   = get_nested<C>(match);
            Match const& else_ = get_nested<D>(match);
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
////////////////////////////////////////////////////////////////////////////////

struct ifnotequal_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("ifnotequal"
                    >> +_s >> engine.expression  // A
                    >> +_s >> engine.expression) // B
                               >> engine.block   // C
              >> !(TAG("else") >> engine.block)  // D
              >>   TAG("endifnotequal");
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& left  = get_nested<A>(match);
            Match const& right = get_nested<B>(match);
            Match const& if_   = get_nested<C>(match);
            Match const& else_ = get_nested<D>(match);
            bool  const  cond_ = engine.evaluate(left, context, options)
                              != engine.evaluate(right, context, options);

                 if (cond_) engine.render_block(out, if_,   context, options);
            else if (else_) engine.render_block(out, else_, context, options);
        }
    };
};

//
// include_tag
////////////////////////////////////////////////////////////////////////////////

struct include_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("include" >> +_s >> (s1 = engine.string_literal));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            String const path = engine.extract_string(match[1]);
            engine.render_file(out, path, context, options);
        }
    };
};

//
// now_tag
////////////////////////////////////////////////////////////////////////////////

struct now_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("now" >> +_s >> (s1 = engine.string_literal));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            String const format = engine.extract_string(match[1]);
            out << detail::format_current_time<String>(format);
        }
    };
};

//
// spaceless_tag
////////////////////////////////////////////////////////////////////////////////

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

        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("spaceless") >> engine.block // A
                >> TAG("endspaceless");
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {

            std::basic_ostringstream<Char> stream;
            std::ostreambuf_iterator<Char> output(out);

            Match const& body = get_nested<A>(match);
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
////////////////////////////////////////////////////////////////////////////////

struct ssi_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("ssi" >> +_s >> (s1 = '/' >> +~_s) // absolute path
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
                    engine.template convert<char>(path);
                std::basic_ifstream<Char> file(path_.c_str());
                while (std::getline(file, line)) out << line;
            }
        }
    };
};

//
// templatetag_tag
////////////////////////////////////////////////////////////////////////////////

struct templatetag_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("templatetag" >> +_s >> (s1 = engine.identifier));
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            String const name = match[1].str();
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
                out << engine.default_value;
            }
        }
    };
};

//
// variable_tag
////////////////////////////////////////////////////////////////////////////////

struct variable_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return engine.variable_open
                    >> *_s >> engine.expression        // A
                >> !(*_s >> '|' >> *_s >> engine.pipe) // B
            >> *_s >> engine.variable_close;
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& expr = get_nested<A>(match);
            Match const& pipe = get_nested<B>(match);
            Value value;

            try {
                value = engine.evaluate(expr, context, options);
                if (pipe) value = engine.apply_pipe(value, pipe, context, options);
            }
            catch (missing_variable const&) { value = engine.default_value; }
            catch (missing_attribute const&) { value = engine.default_value; }

            bool const safe = !options.autoescape || value.safe();
            safe ? out << value : out << value.escape();
        }
    };
};

//
// widthratio_tag
////////////////////////////////////////////////////////////////////////////////

struct widthratio_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("widthratio"
                >> +_s >> engine.expression // A
                >> +_s >> engine.expression // B
                >> +_s >> engine.expression // C
            );
        }

        template <class T>
        inline static T round(T const r) {
            return (r > 0.0) ? std::floor(r + 0.5) : std::ceil(r - 0.5);
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& value = get_nested<A>(match);
            Match const& limit = get_nested<B>(match);
            Match const& width = get_nested<C>(match);

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
////////////////////////////////////////////////////////////////////////////////

struct with_tag {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        Regex syntax(Engine const& engine) const {
            using namespace xpressive;
            return TAG("with" >> +_s >> engine.expression  // A
                >> +_s >> "as" >> +_s >> engine.identifier // B
                    ) >> engine.block                      // C
                >> TAG("endwith");
        }

        void render( Match   const& match,   Engine  const& engine
                   , Context const& context, Options const& options
                   , typename Engine::stream_type& out) const {
            Match const& expr  = get_nested<A>(match);
            Match const& name  = get_nested<B>(match);
            Match const& body  = get_nested<C>(match);

            Context copy = context;
            copy[name.str()] = engine.evaluate(expr, context, options);
            engine.render_block(out, body, copy, options);
        }
    };
};

#undef TAG

}}} // namespace ajg::synthesis::django

#endif // AJG_SYNTHESIS_ENGINES_DJANGO_TAGS_HPP_INCLUDED
