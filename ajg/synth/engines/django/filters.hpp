//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_FILTERS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_FILTERS_HPP_INCLUDED

#include <cmath>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <algorithm>

#include <boost/format.hpp>
#include <boost/cstdint.hpp>
#include <boost/tokenizer.hpp>
#include <boost/throw_exception.hpp>

#include <ajg/synth/engines/detail.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace ajg {
namespace synth {
namespace django {

using detail::text;

//
// argument exceptions
////////////////////////////////////////////////////////////////////////////////

struct superfluous_argument : public std::invalid_argument {
    superfluous_argument() :
        std::invalid_argument("superfluous argument to filter") {}
};

struct missing_argument : public std::invalid_argument {
    missing_argument() :
        std::invalid_argument("missing argument to filter") {}
};

namespace {
    static char const word_delimiters[] = " \t\n.,;:!?'\"-";
} // anonymous

//
// add_filter
////////////////////////////////////////////////////////////////////////////////

struct add_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("add"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            return value.count() + args[0].count();
        }
    };
};

//
// addslashes_filter
////////////////////////////////////////////////////////////////////////////////

struct addslashes_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("addslashes"); }

        inline static Value escape(Value const& value) {
            String const string = value.to_string();
            // std::basic_ostringstream<Char> stream;
            if (Size const escapes = std::count_if(string.begin(),
                string.end(), algorithm::is_any_of("'\"\\"))) {

                String result;
                result.reserve(string.size() + escapes);

                BOOST_FOREACH(Char const c, string) {
                    switch (c) {
                        case Char('\''): result += text("\\'");  break;
                        case Char('"'):  result += text("\\\""); break;
                        case Char('\\'): result += text("\\\\"); break;
                        default: result += c;
                    }
                }

                return result;
            }
            else { // no escapes
                return value;
            }
        }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());
            return escape(value).mark_safe();
        }
    };
};

//
// capfirst_filter
////////////////////////////////////////////////////////////////////////////////

struct capfirst_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("capfirst"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            String string = value.to_string();
            if (!string.empty()) string[0] = std::toupper(string[0]);
            return string;
        }
    };
};

//
// center_filter
////////////////////////////////////////////////////////////////////////////////

struct center_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("center"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            String const string = value.to_string();
            Size const width = args[0].count();
            Size const pad = width > string.length() ?
                (width - string.length()) / 2 : 0;
            return String(pad, Char(' ')) + string;
        }
    };
};

//
// cut_filter
////////////////////////////////////////////////////////////////////////////////

struct cut_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("cut"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            String const from = value.to_string();
            String const what = args[0].to_string();
            return algorithm::erase_all_copy(from, what);
        }
    };
};

//
// date_filter
////////////////////////////////////////////////////////////////////////////////

struct date_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("date"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            String format = "DATE_FORMAT";

            if (args.size() > 1) throw_exception(superfluous_argument());
            if (args.size() > 0) format = args[0].to_string();

            return engine.format_datetime(options, format, value);
        }
    };
};

//
// default_filter
////////////////////////////////////////////////////////////////////////////////

struct default_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("default"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            return value ? value : args[0];
        }
    };
};

//
// default_if_none_filter
////////////////////////////////////////////////////////////////////////////////

struct default_if_none_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("default_if_none"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            return value.empty() ? args[0] : value;
        }
    };
};

//
// divisibleby_filter
////////////////////////////////////////////////////////////////////////////////

struct divisibleby_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("divisibleby"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            intmax_t const dividend = value.count();
            intmax_t const divisor  = args[0].count();
            return dividend % divisor == 0;
        }
    };
};

//
// escape_filter
////////////////////////////////////////////////////////////////////////////////

struct escape_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("escape"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());
            return value.mark_unsafe();
        }
    };
};

//
// escapejs_filter
////////////////////////////////////////////////////////////////////////////////

struct escapejs_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("escapejs"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            String string = value.to_string(), result;
            result.reserve(string.size()); // Assume no escapes.

            BOOST_FOREACH(Char const c, string) {
                result += c < 32 ? String(text("\\x")) + detail::to_hex<2>(c)
                                 : String(1, c);
            }

            return result;
        }
    };
};

//
// filesizeformat_filter
////////////////////////////////////////////////////////////////////////////////

struct filesizeformat_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("filesizeformat"); }

        inline static String format(uintmax_t const size) {
            return detail::abbreviate_size<String>(size);
        }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            return format(std::abs(value.count()));
        }
    };
};

//
// first_filter
////////////////////////////////////////////////////////////////////////////////

struct first_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("first"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            if (!value.length()) {
                throw_exception(std::invalid_argument("sequence"));
            }

            return value.front();
        }
    };
};

//
// fix_ampersands_filter
////////////////////////////////////////////////////////////////////////////////

struct fix_ampersands_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("fix_ampersands"); }

        definition() {
            using namespace xpressive;
            regex_ = as_xpr('&') >> ~before((+_w | '#' >> +_d) >> ';');
        }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            return Value(xpressive::regex_replace(value.to_string(),
                regex_, text("&amp;"))).mark_safe();
        }

      private:

        typename Engine::string_regex_type regex_;
    };
};

//
// floatformat_filter
////////////////////////////////////////////////////////////////////////////////

struct floatformat_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("floatformat"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() > 1) throw_exception(superfluous_argument());

            // Get the number and the decimal places.
            std::basic_ostringstream<Char> stream;
            int const n = args.empty() ? -1 : args[0].count();
            typename Value::number_type const number = value.count();

            // If it's an integer and n < 0, we don't want decimals.
            bool const is_integer = detail::is_integer(number);
            int const precision = n < 0 && is_integer ? 0 : std::abs(n);
            stream << std::fixed << std::setprecision(precision) << number;

            return Value(stream.str()).mark_safe();
        }
    };
};

//
// force_escape_filter
////////////////////////////////////////////////////////////////////////////////

struct force_escape_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("force_escape"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());
            return value.escape().mark_safe();
        }
    };
};

//
// get_digit_filter
////////////////////////////////////////////////////////////////////////////////

struct get_digit_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("get_digit"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            try {
                typename Value::number_type const number = value.count();
                intmax_t const position = args[0].count();
                intmax_t const integer = number;

                if (position > 0) {
                    // Ensure the number operated on is whole.
                    if (number == integer && integer >= 1) {
                        String const text = lexical_cast<String>(integer);

                        if (Size(position) <= text.length()) {
                            return *(text.end() - position);
                        }
                    }
                }
            }
            catch (bad_method       const&) {} // Do nothing.
            catch (bad_lexical_cast const&) {} // Do nothing.
            // Otherwise, (e.g. if any of the above failed):
            return value;
        }
    };
};

//
// iriencode_filter
////////////////////////////////////////////////////////////////////////////////

struct iriencode_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("iriencode"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());
            return detail::iri_encode(value.to_string());
        }
    };
};

//
// join_filter
////////////////////////////////////////////////////////////////////////////////

struct join_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("join"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            Size i = 0;
            std::basic_ostringstream<Char> stream;
            String const delimiter = args[0].to_string();

            BOOST_FOREACH(Value const& v, value) {
                if (i++) stream << delimiter;
                stream << v;
            }

            BOOST_ASSERT(stream);
            Value result = stream.str();
            result.safe(value.safe());
            return result;
        }
    };
};

//
// last_filter
////////////////////////////////////////////////////////////////////////////////

struct last_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("last"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            if (!value.length()) {
                throw_exception(std::invalid_argument("sequence"));
            }

            return value.back();
        }
    };
};

//
// length_filter
////////////////////////////////////////////////////////////////////////////////

struct length_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("length"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());
            return value.length();
        }
    };
};

//
// length_is_filter
////////////////////////////////////////////////////////////////////////////////

struct length_is_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("length_is"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            Size const length = args[0].count();
            return length == value.length();
        }
    };
};

//
// linebreaks_filter
////////////////////////////////////////////////////////////////////////////////

struct linebreaks_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("linebreaks"); }

        definition() {
            using namespace xpressive;
            newlines_ = _n >> +_n;
            regex_ = '\r' >> !as_xpr('\n');
        }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            std::basic_ostringstream<Char> stream;
            String const input = xpressive::regex_replace(
                value.to_string(), regex_, engine.newline);

            xpressive::regex_token_iterator<typename String::const_iterator>
                begin(input.begin(), input.end(), newlines_, -1), end;
            bool const safe = !options.autoescape || value.safe();

            BOOST_FOREACH(String const& line, std::make_pair(begin, end)) {
                String p = safe ? Value(line).escape().to_string() : line;
                algorithm::replace_all(p, engine.newline, text("<br />"));
                stream << "<p>" << p << "</p>" << std::endl << std::endl;
            }

            return Value(stream.str()).mark_safe();
        }

      private:

        typename Engine::string_regex_type regex_;
        typename Engine::string_regex_type newlines_;
    };
};

//
// linebreaksbr_filter
////////////////////////////////////////////////////////////////////////////////

struct linebreaksbr_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("linebreaksbr"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            return Value(algorithm::replace_all_copy(value.to_string(),
                engine.newline, text("<br />"))).mark_safe();
        }
    };
};

//
// linenumbers_filter
////////////////////////////////////////////////////////////////////////////////

struct linenumbers_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("linenumbers"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            Size count = 1;
            std::vector<String> lines;
            typedef basic_format<Char> format;
            std::basic_ostringstream<Char> stream;
            String const input = value.to_string();
            String const pattern = text("%%0%dd. %%s");
            bool const safe = !options.autoescape || value.safe();
            algorithm::split(lines, input, algorithm::is_any_of("\n"));
            Size const width = lexical_cast<String>(lines.size()).size();
            String const spec = (format(pattern) % width).str();

            BOOST_FOREACH(Value const& line, lines) {
                Value const item = safe ? line : line.escape();
                stream << (format(spec) % count++ % item) << std::endl;
            }

            return Value(stream.str()).mark_safe();
        }
    };
};

//
// ljust_filter
////////////////////////////////////////////////////////////////////////////////

struct ljust_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("ljust"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            Size const width = args[0].count();
            std::basic_ostringstream<Char> stream;
            stream << std::left << std::setw(width) << value;
            BOOST_ASSERT(stream);
            return stream.str();
        }
    };
};

//
// lower_filter
////////////////////////////////////////////////////////////////////////////////

struct lower_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("lower"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());
            return algorithm::to_lower_copy(value.to_string());
        }
    };
};

//
// make_list_filter
////////////////////////////////////////////////////////////////////////////////

struct make_list_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("make_list"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            Size i = 0;
            std::basic_ostringstream<Char> stream;
            Value const sequence = value.is_numeric() ?
                value.to_string() : value;

            BOOST_FOREACH(Value const& item, sequence) {
                stream << (i++ ? ", " : "[") << item;
            }

            return stream.str() + Char(']');
        }
    };
};

//
// phone2numeric_filter
////////////////////////////////////////////////////////////////////////////////

struct phone2numeric_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("phone2numeric"); }

        inline static int convert(int const c) {
            switch (c) {
                case 'a': case 'b': case 'c': return '2';
                case 'd': case 'e': case 'f': return '3';
                case 'g': case 'h': case 'i': return '4';
                case 'j': case 'k': case 'l': return '5';
                case 'm': case 'n': case 'o': return '6';
                case 't': case 'u': case 'v': return '8';
                case 'p': case 'q': case 'r': case 's': return '7';
                case 'w': case 'x': case 'y': case 'z': return '9';
                default: return c;
            }
        }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            String phone = value.to_string();
            algorithm::to_lower(phone);
            std::transform(phone.begin(), phone.end(),
                           phone.begin(), convert);
            return phone;
        }
    };
};


//
// pluralize_filter
////////////////////////////////////////////////////////////////////////////////

struct pluralize_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("pluralize"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() > 1) throw_exception(superfluous_argument());

            String singular, plural;
            Array const params = args.empty() ? Array() : engine.
                template split_argument<','>(args[0], context, options);

            switch (params.size()) {
                case 0: plural = text("s");             break;
                case 1: plural = params[0].to_string(); break;
                default: // 2+
                    singular = params[0].to_string();
                    plural   = params[1].to_string();
            }

            return value.length() ? plural : singular;
        }
    };
};

//
// pprint_filter
////////////////////////////////////////////////////////////////////////////////

struct pprint_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("pprint"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());
            // NOTE: Since this filter is for debugging, we don't normally try
            //       to do anything fancy. However, in the Python binding,
            //       this filter is overridden with a call to the real pprint.
            return value.to_string();
        }
    };
};

//
// random_filter
////////////////////////////////////////////////////////////////////////////////

struct random_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("random"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            if (Size const length = value.length()) {
                Size const index = detail::random_int(0, length - 1);
                return value[index];
            }
            else {
                throw_exception(std::invalid_argument("sequence"));
            }
        }
    };
};

//
// removetags_filter
////////////////////////////////////////////////////////////////////////////////

struct removetags_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("removetags"); }

        definition() {
            using namespace xpressive;
            typename Engine::string_regex_type const
                namechar = ~(set = ' ', '\t', '\n', '\v', '\f', '\r', '>'),
                whitespace = (set = ' ', '\t', '\n', '\v', '\f', '\r');
            tag_ = '<' >> !as_xpr('/')
                           // The tag's name:
                           >> (s1 = -+namechar)
                           // Attributes, if any:
                           >> !(+whitespace >> -*~as_xpr('>'))
                       >> !as_xpr('/')
                >> '>';
        }

        struct formatter {
            std::vector<String> const& tags;

            template <class Match_>
            String operator()(Match_ const& match) const {
                String const tag = match[xpressive::s1].str();
                return detail::find_value(tag, tags) ? String() : match.str();
            }
        };

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            std::vector<String> tags;
            formatter const format = { tags };
            int (*predicate)(int) = std::isspace;
            String const source = args[0].to_string();
            algorithm::split(tags, source, predicate);
            return xpressive::regex_replace(value.to_string(), tag_, format);
        }

      private:

        typename Engine::string_regex_type tag_;
    };
};

//
// rjust_filter
////////////////////////////////////////////////////////////////////////////////

struct rjust_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("rjust"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            Size const width = args[0].count();
            std::basic_ostringstream<Char> stream;
            stream << std::right << std::setw(width) << value;
            BOOST_ASSERT(stream);
            return stream.str();
        }
    };
};

//
// safe_filter
////////////////////////////////////////////////////////////////////////////////

struct safe_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("safe"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            return options.autoescape ?
                // NOTE: The to_string is there because `safe`
                //       is expected to stringize its operand
                //       immediately, not just mark it safe.
                Value(value.to_string()).mark_safe() : value;
        }
    };
};

//
// safeseq_filter
////////////////////////////////////////////////////////////////////////////////

struct safeseq_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("safeseq"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            Value copy = value;

            // FIXME: These values should be mutable.
            BOOST_FOREACH(Value const& v, copy) {
                const_cast<Value&>(v).safe(true);
            }

            return copy.mark_safe();
        }
    };
};

//
// slice_filter
////////////////////////////////////////////////////////////////////////////////

struct slice_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("slice"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() > 1) throw_exception(superfluous_argument());

            String singular, plural;
            Array const params = args.empty() ? Array() : engine.
                template split_argument<':'>(args[0], context, options);

            if (params.size() < 2) {
                throw_exception(missing_argument());
            }

            Array result;
            Value const lower = params[0];
            Value const upper = params[1];
            typename Value::range_type range =
                value.slice(lower ? optional<int>(lower.count()) : none,
                            upper ? optional<int>(upper.count()) : none);
            std::copy(range.first, range.second, std::back_inserter(result));
            return result;
        }
    };
};

//
// slugify_filter
////////////////////////////////////////////////////////////////////////////////

struct slugify_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("slugify"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            struct invalid { inline static bool fn(Char const c) {
                return std::isalnum(c) || c == '_' || c == '-';
            }};

            String slug = algorithm::trim_copy(value.to_string());
            std::replace(slug.begin(), slug.end(), Char(' '), Char('-'));
            slug.erase(std::remove_if(slug.begin(), slug.end(),
                invalid::fn), slug.end());
            algorithm::to_lower(slug);
            return slug;
        }
    };
};

//
// stringformat_filter
////////////////////////////////////////////////////////////////////////////////

struct stringformat_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("stringformat"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            String const spec = args[0].to_string();
            return (basic_format<Char>(Char('%') + spec) % value).str();
        }
    };
};

//
// striptags_filter
////////////////////////////////////////////////////////////////////////////////

struct striptags_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("striptags"); }

        definition() {
            using namespace xpressive;
            tag_ = '<' >> -*~(as_xpr('>')) >> '>';
        }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());
            return xpressive::regex_replace(value.to_string(), tag_, text(""));
        }

      private:

        typename Engine::string_regex_type tag_;
    };
};

//
// time_filter
////////////////////////////////////////////////////////////////////////////////

struct time_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("time"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            String format = "TIME_FORMAT";

            if (args.size() > 1) throw_exception(superfluous_argument());
            if (args.size() > 0) format = args[0].to_string();

            return engine.format_datetime(options, format, value);
        }
    };
};

//
// title_filter
////////////////////////////////////////////////////////////////////////////////

struct title_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("title"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            String text = value.to_string();

            for (Size i = 0; i < text.length(); ++i) {
                if (i == 0 || std::isspace(text[i - 1])) {
                    text[i] = std::toupper(text[i]);
                }
            }

            return text;
        }
    };
};

//
// truncatechars_filter
////////////////////////////////////////////////////////////////////////////////

struct truncatechars_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("truncatechars"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            Size const limit = args[0].count();
            String const input = value.to_string();

            if (input.length() > limit) {
                return input.substr(0, limit) + engine.ellipsis;
            }
            else {
                return input;
            }
        }
    };
};

//
// truncatewords_filter
////////////////////////////////////////////////////////////////////////////////

struct truncatewords_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("truncatewords"); }

        typedef Char                            char_type;
        typedef String                          string_type;
        typedef typename String::const_iterator iterator_type;
        typedef char_separator<Char>            separator_type;
        typedef tokenizer < separator_type
                          , iterator_type
                          , String >            tokenizer_type;

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            Size const limit = args[0].count();
            String const input = value.to_string();
            String const delimiters = text(word_delimiters);
            separator_type const separator(delimiters.c_str());
            tokenizer_type const tokenizer(input, separator);

            std::basic_ostringstream<Char> stream;
            typename tokenizer_type::const_iterator word = tokenizer.begin();
            typename tokenizer_type::const_iterator const end = tokenizer.end();

            for (Size i = 0; i < limit && word != end; ++word, ++i) {
                stream << (i ? " " : "") << *word;
            }

            if (word != end) {
                stream << " " << engine.ellipsis;
            }

            return stream.str();
        }
    };
};

//
// unordered_list_filter
////////////////////////////////////////////////////////////////////////////////

struct unordered_list_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("unordered_list"); }

        inline static bool is_iterable(Value const& item) {
            if (item.is_string()) {
            // Treat strings atomically.
                return false;
            }
            else {
                try {
                    // length will throw if item
                    // cannot be iterated over.
                    return item.length(), true;
                }
                catch (bad_method const&) {
                    return false;
                }
            }
        }

        template <bool Safe, class Stream>
        inline static void push_item( Value  const& item
                                    , Engine const& engine
                                    , Size   const  level
                                    , Stream&       out
                                    ) {
            String const indent(level, Char('\t'));

            if (is_iterable(item)) {
                if (Size const length = item.length()) {
                    for (Size i = 0; i < length; ++i) {
                        out << indent << "<li>";
                        Safe ? out << item[i] : out << item[i].escape();

                        if (++i < length) {
                            if (is_iterable(item[i])) {
                                out << std::endl << indent << "<ul>" << std::endl;
                                push_item<Safe>(item[i], engine, level + 1, out);
                                out << indent << "</ul>" << std::endl << indent;
                            }
                            else {
                                out << "</li>" << std::endl << indent << "<li>";
                                Safe ? out << item[i] : out << item[i].escape();
                            }
                        }

                        out << "</li>" << std::endl;
                    }
                }
            }
            else {
                out << indent << "<li>";
                Safe ? out << item : out << item.escape();
                out << "</li>" << std::endl;
            }
        }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            std::basic_ostringstream<Char> stream;
            value.safe() ? push_item<true>(value, engine, 0, stream)
                         : push_item<false>(value, engine, 0, stream);
            return Value(stream.str()).mark_safe();
        }
    };
};

//
// upper_filter
////////////////////////////////////////////////////////////////////////////////

struct upper_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("upper"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());
            return algorithm::to_upper_copy(value.to_string());
        }
    };
};

//
// urlencode_filter
////////////////////////////////////////////////////////////////////////////////

struct urlencode_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("urlencode"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());
            return detail::uri_encode(value.to_string());
        }
    };
};

//
// urlize_filter
////////////////////////////////////////////////////////////////////////////////

struct urlize_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("urlize"); }

        definition() {
            using namespace xpressive;
            typename Engine::string_regex_type const safe =
                +(alnum | (set= '/', '&', '=', ':', ';', '#',
                                '?', '+', '-', '*', '%', '@'));
            url_ = !(s1 = +alnum >> ':') >> +safe >> +('.' >> +safe);
        }

        struct formatter {
            Size const limit;
            String const ellipsis;
            formatter(Size const limit, String const& ellipsis)
                : limit(limit), ellipsis(ellipsis) {}

            template <class Match_>
            String operator()(Match_ const& match) const {
                std::basic_ostringstream<Char> stream;
                String const link = match.str();
                String const full = match.str();
                String const text = full.substr(0, limit);
                bool const scheme = !match[xpressive::s1];
                bool const more = text.size() < full.size();
                stream << "<a href='" << (scheme ? "http://" : "") << link;
                stream << "'>" << text << (more ? ellipsis : "") << "</a>";
                return stream.str();
            }
        };

        Value urlize(Value const& value, Size const limit, String const& ellipsis) const {
            String const body = value.to_string();

            return Value(xpressive::regex_replace(body,
                url_, formatter(limit, ellipsis))).mark_safe();
        }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());
            return this->urlize(value, Size(-1), engine.ellipsis);
        }

      private:

        typename Engine::string_regex_type url_;
    };
};

//
// urlizetrunc_filter
////////////////////////////////////////////////////////////////////////////////

struct urlizetrunc_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition : urlize_filter
         ::definition< Char, Regex, String, Context, Value
                     , Size, Match, Engine, Options, Array
                     > {
        String name() const { return text("urlizetrunc"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());
            return this->urlize(value, args[0].count(), engine.ellipsis);
        }
    };
};


//
// wordcount_filter
////////////////////////////////////////////////////////////////////////////////

struct wordcount_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("wordcount"); }

        typedef Char                            char_type;
        typedef String                          string_type;
        typedef typename String::const_iterator iterator_type;
        typedef char_separator<Char>            separator_type;
        typedef tokenizer < separator_type
                          , iterator_type
                          , String >            tokenizer_type;

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (!args.empty()) throw_exception(superfluous_argument());

            String const input = value.to_string();
            String const delimiters = text(word_delimiters);
            separator_type const separator(delimiters.c_str());
            tokenizer_type const tokenizer(input, separator);

            return std::distance(tokenizer.begin(), tokenizer.end());
        }
    };
};


//
// wordwrap_filter
////////////////////////////////////////////////////////////////////////////////

struct wordwrap_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("wordwrap"); }

        inline static String wrap( String const& input
                                 , Size   const& width
                                 , String const& newline
                                 ) {
            Size i = 0;
            String word;
            String result;
            Char last = '\0';

            BOOST_FOREACH(Char const& c, input) {
                if (++i == width) {
                    algorithm::trim_left(word);
                    result += newline + word;
                    i = word.length();
                    word.clear();
                }
                else if (std::isspace(c)
                     && !std::isspace(last)) {
                    result += word;
                    word.clear();
                }

                word += last = c;
            }

            result += word;
            return result;
        }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            Size   const width = args[0].count();
            String const text = value.to_string();
            return wrap(text, width, engine.newline);
        }
    };
};

//
// yesno_filter
////////////////////////////////////////////////////////////////////////////////

struct yesno_filter {
    template < class Char, class Regex, class String, class Context, class Value
             , class Size, class Match, class Engine, class Options, class Array
             >
    struct definition {
        String name() const { return text("yesno"); }

        Value process(Value  const& value, Engine  const& engine,
                      String const& name,  Context const& context,
                      Array  const& args,  Options const& options) const {
            if (args.size() < 1) throw_exception(missing_argument());
            if (args.size() > 1) throw_exception(superfluous_argument());

            Value true_, false_, none_;
            Array const params = engine.template
                split_argument<','>(args[0], context, options);

            switch (params.size()) {
                case 0: throw_exception(missing_argument()); break;
                case 1: throw_exception(missing_argument()); break;
                case 2: true_ = params[0]; false_ = params[1]; none_ = params[1]; break;
                case 3: true_ = params[0]; false_ = params[1]; none_ = params[2]; break;
                default: throw_exception(superfluous_argument());
            }

            return value.empty() ? none_ : (value ? true_ : false_);
        }
    };
};

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_FILTERS_HPP_INCLUDED
