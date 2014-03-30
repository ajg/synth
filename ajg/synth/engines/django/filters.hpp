//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_FILTERS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_FILTERS_HPP_INCLUDED

#include <cmath>
#include <stack>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <algorithm>

#include <boost/format.hpp>
#include <boost/cstdint.hpp>
#include <boost/tokenizer.hpp>
#include <boost/assign/list_of.hpp>
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
namespace {

using detail::text;
using boost::xpressive::_d;
using boost::xpressive::_ln;
using boost::xpressive::_n;
using boost::xpressive::_w;
using boost::xpressive::alnum;
using boost::xpressive::as_xpr;
using boost::xpressive::before;
using boost::xpressive::regex_replace;
using boost::xpressive::s1;
using boost::xpressive::set;

static char const word_delimiters[] = " \t\n.,;:!?'\"-";

} // anonymous

//
// argument exceptions
////////////////////////////////////////////////////////////////////////////////////////////////////

struct superfluous_argument : public std::invalid_argument {
    superfluous_argument() :
        std::invalid_argument("superfluous argument to filter") {}
};

struct missing_argument : public std::invalid_argument {
    missing_argument() :
        std::invalid_argument("missing argument to filter") {}
};

template <class Engine>
struct builtin_filters {
    typedef Engine                                                              engine_type;
    typedef typename engine_type::options_type                                  options_type;
	typedef typename options_type::boolean_type                                 boolean_type;
	typedef typename options_type::size_type                                    size_type;
    typedef typename options_type::string_type                                  string_type;
    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::sequence_type                                sequence_type;
    typedef typename options_type::arguments_type                               arguments_type;
    typedef typename options_type::context_type                                 context_type;
    // TODO: Replace these with *_type versions:
    typedef typename options_type::char_type                                    Char;
    typedef typename options_type::string_type                                  String;
    typedef typename options_type::value_type                                   Value;
    typedef typename options_type::size_type                                    Size;

    typedef typename engine_type::string_regex_type                             string_regex_type;

//
// filter_type
////////////////////////////////////////////////////////////////////////////////////////////////////

    typedef value_type (*filter_type)( engine_type   const&
                                     , value_type    const&
                                     , sequence_type const& // TODO: arguments_type
                                     , context_type  const&
                                     , options_type  const&
                                     );

//
// get
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static filter_type get(string_type const& name) {
        // TODO: Consider replacing with fastmatch.h switch.
        static std::map<string_type, filter_type> const filters = boost::assign::map_list_of
            (string_type(detail::text("add")),                add_filter::process)
            (string_type(detail::text("addslashes")),         addslashes_filter::process)
            (string_type(detail::text("capfirst")),           capfirst_filter::process)
            (string_type(detail::text("center")),             center_filter::process)
            (string_type(detail::text("cut")),                cut_filter::process)
            (string_type(detail::text("date")),               date_filter::process)
            (string_type(detail::text("default")),            default_filter::process)
            (string_type(detail::text("default_if_none")),    default_if_none_filter::process)
            (string_type(detail::text("dictsort")),           dictsort_filter::process)
            (string_type(detail::text("dictsortreversed")),   dictsortreversed_filter::process)
            (string_type(detail::text("divisibleby")),        divisibleby_filter::process)
            (string_type(detail::text("escape")),             escape_filter::process)
            (string_type(detail::text("escapejs")),           escapejs_filter::process)
            (string_type(detail::text("filesizeformat")),     filesizeformat_filter::process)
            (string_type(detail::text("first")),              first_filter::process)
            (string_type(detail::text("fix_ampersands")),     fix_ampersands_filter::process)
            (string_type(detail::text("floatformat")),        floatformat_filter::process)
            (string_type(detail::text("force_escape")),       force_escape_filter::process)
            (string_type(detail::text("get_digit")),          get_digit_filter::process)
            (string_type(detail::text("iriencode")),          iriencode_filter::process)
            (string_type(detail::text("join")),               join_filter::process)
            (string_type(detail::text("last")),               last_filter::process)
            (string_type(detail::text("length")),             length_filter::process)
            (string_type(detail::text("length_is")),          length_is_filter::process)
            (string_type(detail::text("linebreaks")),         linebreaks_filter::process)
            (string_type(detail::text("linebreaksbr")),       linebreaksbr_filter::process)
            (string_type(detail::text("linenumbers")),        linenumbers_filter::process)
            (string_type(detail::text("ljust")),              ljust_filter::process)
            (string_type(detail::text("lower")),              lower_filter::process)
            (string_type(detail::text("make_list")),          make_list_filter::process)
            (string_type(detail::text("phone2numeric")),      phone2numeric_filter::process)
            (string_type(detail::text("pluralize")),          pluralize_filter::process)
            (string_type(detail::text("pprint")),             pprint_filter::process)
            (string_type(detail::text("random")),             random_filter::process)
            (string_type(detail::text("removetags")),         removetags_filter::process)
            (string_type(detail::text("rjust")),              rjust_filter::process)
            (string_type(detail::text("safe")),               safe_filter::process)
            (string_type(detail::text("safeseq")),            safeseq_filter::process)
            (string_type(detail::text("slice")),              slice_filter::process)
            (string_type(detail::text("slugify")),            slugify_filter::process)
            (string_type(detail::text("stringformat")),       stringformat_filter::process)
            (string_type(detail::text("striptags")),          striptags_filter::process)
            (string_type(detail::text("time")),               time_filter::process)
            (string_type(detail::text("timesince")),          timesince_filter::process)
            (string_type(detail::text("timeuntil")),          timeuntil_filter::process)
            (string_type(detail::text("title")),              title_filter::process)
            (string_type(detail::text("truncatechars")),      truncatechars_filter::process)
            (string_type(detail::text("truncatechars_html")), truncatechars_html_filter::process)
            (string_type(detail::text("truncatewords")),      truncatewords_filter::process)
            (string_type(detail::text("truncatewords_html")), truncatewords_html_filter::process)
            (string_type(detail::text("unordered_list")),     unordered_list_filter::process)
            (string_type(detail::text("upper")),              upper_filter::process)
            (string_type(detail::text("urlencode")),          urlencode_filter::process)
            (string_type(detail::text("urlize")),             urlize_filter::process)
            (string_type(detail::text("urlizetrunc")),        urlizetrunc_filter::process)
            (string_type(detail::text("wordcount")),          wordcount_filter::process)
            (string_type(detail::text("wordwrap")),           wordwrap_filter::process)
            (string_type(detail::text("yesno")),              yesno_filter::process)
            ;
        typename std::map<string_type, filter_type>::const_iterator it = filters.find(name);
        return it == filters.end() ? 0 : it->second;
    }

//
// add_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct add_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            return value.count() + arguments[0].count();
        }
    };

//
// addslashes_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct addslashes_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            return escape(value).mark_safe();
        }

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
    };

//
// capfirst_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct capfirst_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            String string = value.to_string();
            if (!string.empty()) string[0] = std::toupper(string[0]);
            return string;
        }
    };

//
// center_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct center_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            string_type const string = value.to_string();
			size_type   const width  = arguments[0].to_size();
			size_type   const length = string.length();

            if (width <= length) {
                return string;
            }

            size_type const right = (width - length) / 2;
			size_type const left = width - length - right;
			return string_type(left, char_type(' ')) + string + string_type(right, char_type(' '));
        }
    };

//
// cut_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct cut_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            String const from = value.to_string();
            String const what = arguments[0].to_string();
            return algorithm::erase_all_copy(from, what);
        }
    };

//
// date_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct date_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            String format = "DATE_FORMAT";

            if (arguments.size() > 1) throw_exception(superfluous_argument());
            if (arguments.size() > 0) format = arguments[0].to_string();

            return engine.format_datetime(options, format, value.to_datetime());
        }
    };

//
// default_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct default_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            return value ? value : arguments[0];
        }
    };

//
// default_if_none_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct default_if_none_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            return value.empty() ? arguments[0] : value;
        }
    };

//
// dictsort_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct dictsort_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            return value.sort_by(arguments[0], false);
        }
    };

//
// dictsortreversed_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct dictsortreversed_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            return value.sort_by(arguments[0], true);
        }
    };

//
// divisibleby_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct divisibleby_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            intmax_t const dividend = static_cast<intmax_t>(value.count());
			intmax_t const divisor  = static_cast<intmax_t>(arguments[0].count());
            return dividend % divisor == 0;
        }
    };

//
// escape_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct escape_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            return value.mark_unsafe();
        }
    };

//
// escapejs_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct escapejs_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            String string = value.to_string(), result;
            result.reserve(string.size()); // Assume no escapes.

            BOOST_FOREACH(Char const c, string) {
                result += c < 32 ? String(text("\\x")) + detail::to_hex<2>(c) : String(1, c);
            }

            return result;
        }
    };

//
// filesizeformat_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct filesizeformat_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            return format(static_cast<size_type>(std::abs(static_cast<intmax_t>(value.count()))));
        }

        inline static String format(size_type const size) {
            return detail::abbreviate_size<string_type>(size);
        }
    };

//
// first_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct first_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            if (!value.length()) {
                throw_exception(std::invalid_argument("sequence"));
            }

            return value.front();
        }
    };

//
// fix_ampersands_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct fix_ampersands_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            static string_regex_type const regex = as_xpr('&') >> ~before((+_w | '#' >> +_d) >> ';');
            return Value(regex_replace(value.to_string(), regex, text("&amp;"))).mark_safe();
        }
    };

//
// floatformat_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct floatformat_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            // Get the number and the decimal places.
            std::basic_ostringstream<Char> stream;
            int const n = arguments.empty() ? -1 : static_cast<int>(arguments[0].count());
            typename Value::number_type const number = value.count();

            // If it's an integer and n < 0, we don't want decimals.
            boolean_type const is_integer = detail::is_integer(number);
            int const precision = n < 0 && is_integer ? 0 : std::abs(n);
            stream << std::fixed << std::setprecision(precision) << number;

            return Value(stream.str()).mark_safe();
        }
    };

//
// force_escape_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct force_escape_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            return value.escape().mark_safe();
        }
    };

//
// get_digit_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct get_digit_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            try {
                typename Value::number_type const number = value.count();
                intmax_t const position = static_cast<intmax_t>(arguments[0].count());
                intmax_t const integer  = static_cast<intmax_t>(number);

                if (position > 0) {
                    // Ensure the number operated on is whole.
                    if (number == integer && integer >= 1) {
                        String const text = boost::lexical_cast<String>(integer);
						size_type const distance = static_cast<size_type>(position);

						if (distance <= text.length()) {
							return *(text.end() - distance);
                        }
                    }
                }
            }
            catch (bad_method              const&) {} // Do nothing.
            catch (boost::bad_lexical_cast const&) {} // Do nothing.
            // Otherwise, (e.g. if any of the above failed):
            return value;
        }
    };

//
// iriencode_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct iriencode_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            return detail::iri_encode(value.to_string());
        }
    };

//
// join_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct join_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            Size i = 0;
            std::basic_ostringstream<Char> stream;
            String const delimiter = arguments[0].to_string();

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

//
// last_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct last_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            if (!value.length()) {
                throw_exception(std::invalid_argument("sequence"));
            }

            return value.back();
        }
    };

//
// length_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct length_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            return value.length();
        }
    };

//
// length_is_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct length_is_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

			return arguments[0].to_size() == value.length();
        }
    };

//
// linebreaks_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct linebreaks_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            static string_regex_type const newline  = _ln;
            static string_regex_type const newlines = _ln >> +_ln;

            std::basic_ostringstream<Char> stream;
            String const input = regex_replace(value.to_string(), newline, engine.newline);

            xpressive::regex_token_iterator<typename String::const_iterator>
                begin(input.begin(), input.end(), newlines, -1), end;
            boolean_type const safe = !options.autoescape || value.safe();

            BOOST_FOREACH(String const& line, std::make_pair(begin, end)) {
                String p = safe ? Value(line).escape().to_string() : line;
                algorithm::replace_all(p, engine.newline, text("<br />"));
                stream << "<p>" << p << "</p>" << std::endl << std::endl;
            }

            return Value(stream.str()).mark_safe();
        }
    };

//
// linebreaksbr_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct linebreaksbr_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            return Value(algorithm::replace_all_copy(value.to_string(),
                engine.newline, text("<br />"))).mark_safe();
        }
    };

//
// linenumbers_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct linenumbers_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            Size count = 1;
            std::vector<String> lines;
            typedef basic_format<Char> format;
            std::basic_ostringstream<Char> stream;
            String const input = value.to_string();
            String const pattern = text("%%0%dd. %%s");
            boolean_type const safe = !options.autoescape || value.safe();
            algorithm::split(lines, input, algorithm::is_any_of("\n"));
            Size const width = boost::lexical_cast<String>(lines.size()).size();
            String const spec = (format(pattern) % width).str();

            BOOST_FOREACH(Value const& line, lines) {
                Value const item = safe ? line : line.escape();
                stream << (format(spec) % count++ % item) << std::endl;
            }

            return Value(stream.str()).mark_safe();
        }
    };

//
// ljust_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct ljust_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            size_type const width = arguments[0].to_size();
            std::basic_ostringstream<Char> stream;
            stream << std::left << std::setw(width) << value;
            BOOST_ASSERT(stream);
            return stream.str();
        }
    };

//
// lower_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct lower_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            return algorithm::to_lower_copy(value.to_string());
        }
    };

//
// make_list_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct make_list_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

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

//
// phone2numeric_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct phone2numeric_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            String phone = value.to_string();
            algorithm::to_lower(phone);
            std::transform(phone.begin(), phone.end(),
                           phone.begin(), translate);
            return phone;
        }

      private:

        inline static int translate(int const c) {
            switch (c) {
                case 'a': case 'b': case 'c':           return '2';
                case 'd': case 'e': case 'f':           return '3';
                case 'g': case 'h': case 'i':           return '4';
                case 'j': case 'k': case 'l':           return '5';
                case 'm': case 'n': case 'o':           return '6';
                case 't': case 'u': case 'v':           return '8';
                case 'p': case 'q': case 'r': case 's': return '7';
                case 'w': case 'x': case 'y': case 'z': return '9';
                default: return c;
            }
        }
    };


//
// pluralize_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct pluralize_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            String singular, plural;
            sequence_type const sequential_arguments = arguments.empty() ? sequence_type() :
                engine.template split_argument<','>(arguments[0], context, options);

            switch (sequential_arguments.size()) {
                case 0: plural = text("s");                           break;
                case 1: plural = sequential_arguments[0].to_string(); break;
                default: // 2+
                    singular = sequential_arguments[0].to_string();
                    plural   = sequential_arguments[1].to_string();
            }

            return value.count() == 1 ? singular : plural;
        }
    };

//
// pprint_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct pprint_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            // NOTE: Since this filter is for debugging, we don't normally try
            //       to do anything fancy. However, in the Python binding,
            //       this filter is overridden with a call to the real pprint.
            return value.to_string();
        }
    };

//
// random_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct random_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            if (Size const length = value.length()) {
                Size const index = detail::random_int(0, length - 1);
                return value[index];
            }
            else {
                throw_exception(std::invalid_argument("sequence"));
            }
        }
    };

//
// removetags_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct removetags_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            std::vector<String> tags;
            formatter const format = { tags };
            int (*predicate)(int) = std::isspace;
            String const source = arguments[0].to_string();
            algorithm::split(tags, source, predicate);
            return regex_replace(value.to_string(), engine.html_tag, format);
        }

      private:

        struct formatter {
            std::vector<String> const& tags;

            template <class Match>
            String operator()(Match const& match) const {
                String const tag = match[s1].str();
                return detail::find_value(tag, tags) ? String() : match.str();
            }
        };
    };

//
// rjust_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct rjust_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            size_type const width = arguments[0].to_size();
            std::basic_ostringstream<Char> stream;
            stream << std::right << std::setw(width) << value;
            BOOST_ASSERT(stream);
            return stream.str();
        }
    };

//
// safe_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct safe_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            return options.autoescape ?
                // NOTE: The to_string is there because `safe`
                //       is expected to stringize its operand
                //       immediately, not just mark it safe.
                Value(value.to_string()).mark_safe() : value;
        }
    };

//
// safeseq_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct safeseq_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            Value copy = value;

            // FIXME: These values should be mutable.
            BOOST_FOREACH(Value const& v, copy) {
                const_cast<Value&>(v).safe(true);
            }

            return copy.mark_safe();
        }
    };

//
// slice_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct slice_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            String singular, plural;
            sequence_type const sequential_arguments = arguments.empty() ? sequence_type() :
                engine.template split_argument<':'>(arguments[0], context, options);

            if (sequential_arguments.size() < 2) {
                throw_exception(missing_argument());
            }

            sequence_type result;
            Value const lower = sequential_arguments[0];
            Value const upper = sequential_arguments[1];
            typename Value::range_type range =
                value.slice(lower ? optional<int>(static_cast<int>(lower.count())) : none,
                            upper ? optional<int>(static_cast<int>(upper.count())) : none);
            std::copy(range.first, range.second, std::back_inserter(result));
            return result;
        }
    };

//
// slugify_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct slugify_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            struct invalid {
                inline static boolean_type fn(Char const c) {
                    return !std::isalnum(c) && c != '_' && c != '-';
                }
            };

            String slug = value.to_string();
            algorithm::trim(slug);
            std::replace(slug.begin(), slug.end(), Char(' '), Char('-'));
            slug.erase(std::remove_if(slug.begin(), slug.end(), invalid::fn), slug.end());
            algorithm::to_lower(slug);
            return slug;
        }
    };

//
// stringformat_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct stringformat_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            String const spec = arguments[0].to_string();
            return (basic_format<Char>(Char('%') + spec) % value).str();
        }
    };

//
// striptags_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct striptags_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            static string_regex_type const tag = '<' >> -*~(as_xpr('>')) >> '>';
            return regex_replace(value.to_string(), tag, text(""));
        }
    };

//
// time_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct time_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            String format = "TIME_FORMAT";

            if (arguments.size() > 1) throw_exception(superfluous_argument());
            if (arguments.size() > 0) format = arguments[0].to_string();

            return engine.format_datetime(options, format, value.to_datetime());
        }
    };

//
// timesince_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct timesince_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            typename options_type::datetime_type to = value.to_datetime();
            typename options_type::datetime_type from = arguments.empty() ?
                detail::local_now() : arguments[0].to_datetime();

            return Value(engine.format_duration(options, from - to)).mark_safe();
        }
    };

//
// timeuntil_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct timeuntil_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            typename options_type::datetime_type to = value.to_datetime();
            typename options_type::datetime_type from = arguments.empty() ?
                detail::local_now() : arguments[0].to_datetime();

            return Value(engine.format_duration(options, to - from)).mark_safe();
        }
    };

//
// title_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct title_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            String text = value.to_string();

            for (Size i = 0; i < text.length(); ++i) {
                if (i == 0 || std::isspace(text[i - 1])) {
                    text[i] = std::toupper(text[i]);
                }
                else {
                    text[i] = std::tolower(text[i]);
                }
            }

            return text;
        }
    };

//
// truncatechars_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct truncatechars_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

			size_type   const limit = arguments[0].to_size();
            size_type   const ellip = engine.ellipsis.length();
            string_type const text  = value.to_string();

            if (text.length() > limit) {
                size_type const trunc = ellip < limit ? limit - ellip : 0;
                return text.substr(0, trunc) + engine.ellipsis;
            }
            else {
                return text;
            }
        }

      private:

        typedef Char                                           char_type;
        typedef Size                                           size_type;
        typedef String                                         string_type;
        typedef Value                                          value_type;
        typedef typename value_type::number_type               number_type;
    };

//
// truncatechars_html_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct truncatechars_html_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

			size_type   const limit = arguments[0].to_size();
            size_type   const ellip = engine.ellipsis.length();
            string_type const input = value.to_string();
            std::basic_ostringstream<char_type> stream;

            iterator_type last = input.begin(), done = input.end();
            regex_iterator_type begin(last, done, engine.html_tag), end;
            std::stack<string_type> open_tags;
            size_type length = 0;
            static string_type const boundaries = detail::text(" \t\n\v\f\r>");

            BOOST_FOREACH(sub_match_type const& match, std::make_pair(begin, end)) {
                string_type const tag  = match.str();
                string_type const name = tag.substr(1, tag.find_first_of(boundaries, 1) - 1);
                string_type const text = string_type(last, match.first);

                last = match.second;
                size_type current = length;

                if ((length += text.length()) > limit) {
                    size_type const trunc = current + ellip < limit ? limit - (current + ellip) : 0;
                    stream << text.substr(0, trunc) + engine.ellipsis;
                    break;
                }
                else {
                    stream << text << tag;

                    if (name[0] == char_type('/')) {
                        if (!open_tags.empty() && open_tags.top() == name.substr(1)) {
                            open_tags.pop();
                        }
                    }
                    else {
                        open_tags.push(name);
                    }
                }
            }

            if (last != done && length <= limit) {
                string_type const text = string_type(last, done);

                if ((length += text.length()) > limit) {
                    size_type const trunc = ellip < limit ? limit - ellip : 0;
                    stream << text.substr(0, trunc) + engine.ellipsis;
                }
                else {
                    stream << text;
                }
            }

            while (!open_tags.empty()) {
                stream << "</" << open_tags.top() << ">";
                open_tags.pop();
            }

            return value_type(stream.str()).mark_safe();
        }

      private:

        typedef Char                                           char_type;
        typedef Size                                           size_type;
        typedef String                                         string_type;
        typedef Value                                          value_type;
        typedef typename value_type::number_type               number_type;
        typedef typename string_type::const_iterator           iterator_type;
        typedef xpressive::regex_token_iterator<iterator_type> regex_iterator_type;
        typedef typename regex_iterator_type::value_type       sub_match_type;
    };

//
// truncatewords_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct truncatewords_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            size_type   const limit = arguments[0].to_size();
            string_type const text  = value.to_string();
            size_type count = 0;
            stream_type stream;

            process_words(stream, text.begin(), text.end(), count, limit, engine.ellipsis);
            return stream.str();
        }

      private:

        typedef Char                                           char_type;
        typedef Size                                           size_type;
        typedef String                                         string_type;
        typedef Value                                          value_type;
        typedef typename value_type::number_type               number_type;
        typedef std::basic_ostringstream<char_type>            stream_type;
        typedef typename string_type::const_iterator           iterator_type;
        typedef char_separator<char_type>                      separator_type;
        typedef tokenizer < separator_type
                          , iterator_type
                          , string_type
                          >                                    tokenizer_type;

        inline static boolean_type process_words( stream_type&         stream
                                                , iterator_type const& from
                                                , iterator_type const& to
                                                , size_type&           count
                                                , size_type     const  limit
                                                , string_type   const& ellipsis
                                                ) {
            static string_type    const delimiters = detail::text(word_delimiters);
            static separator_type const separator(delimiters.c_str());

            tokenizer_type const tokenizer(from, to, separator);
            typename tokenizer_type::const_iterator       word = tokenizer.begin();
            typename tokenizer_type::const_iterator const stop = tokenizer.end();

            for (; count < limit && word != stop; ++word, ++count) {
                // Intra-word whitespace is collapsed to a single space;
                // leading and trailing whitespace is elided.
                stream << (count ? " " : "") << *word;
            }

            boolean_type const words_left = word != stop;
            if (words_left) {
                stream << " " << ellipsis;
            }
            return !words_left;
        }
    };

//
// truncatewords_html_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct truncatewords_html_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            static string_type const boundaries = detail::text(" \t\n\v\f\r>");
			size_type   const limit = arguments[0].to_size();
            string_type const input = value.to_string();
            size_type count = 0;
            stream_type stream;

            iterator_type last = input.begin(), done = input.end();
            regex_iterator_type begin(last, done, engine.html_tag), end;
            stack_type open_tags;

            BOOST_FOREACH(sub_match_type const& match, std::make_pair(begin, end)) {
                string_type   const tag  = match.str();
                string_type   const name = tag.substr(1, tag.find_first_of(boundaries, 1) - 1);
                iterator_type const prev = last; last = match.second;

                if (!process_words(stream, prev, match.first, count, limit, engine.ellipsis) || count >= limit) {
                    break;
                }
                stream << tag;

                if (name[0] == char_type('/')) {
                    if (!open_tags.empty() && open_tags.top() == name.substr(1)) {
                        open_tags.pop();
                    }
                }
                else {
                    open_tags.push(name);
                }
            }

            if (!process_words(stream, last, done, count, limit, engine.ellipsis) || count >= limit) {
                while (!open_tags.empty()) {
                    stream << "</" << open_tags.top() << ">";
                    open_tags.pop();
                }
            }

            return value_type(stream.str()).mark_safe();
        }

      private:

        typedef Char                                           char_type;
        typedef Size                                           size_type;
        typedef String                                         string_type;
        typedef Value                                          value_type;
        typedef typename value_type::number_type               number_type;
        typedef std::basic_ostringstream<char_type>            stream_type;
        typedef typename string_type::const_iterator           iterator_type;
        typedef std::stack<string_type>                        stack_type;
        typedef xpressive::regex_token_iterator<iterator_type> regex_iterator_type;
        typedef typename regex_iterator_type::value_type       sub_match_type;
        typedef char_separator<char_type>                      separator_type;
        typedef tokenizer < separator_type
                          , iterator_type
                          , string_type
                          >                                    tokenizer_type;

        inline static boolean_type process_words( stream_type&         stream
                                                , iterator_type const& from
                                                , iterator_type const& to
                                                , size_type&           count
                                                , size_type     const  limit
                                                , string_type   const& ellipsis
                                                ) {
            static string_type    const delimiters = detail::text(word_delimiters);
            static separator_type const separator(delimiters.c_str());

            tokenizer_type const tokenizer(from, to, separator);
            typename tokenizer_type::const_iterator       word = tokenizer.begin();
            typename tokenizer_type::const_iterator const stop = tokenizer.end();
            iterator_type it = from;

            for (; count < limit && word != stop; ++word, ++count) {
                string_type const lead = string_type(it, word.base() - word->length());
                stream << lead << *word;
                it = word.base();
            }

            boolean_type const words_left  = word != stop;
            boolean_type const under_limit = count < limit;

            if (words_left) {
                stream << " " << ellipsis;
            }
            else if (under_limit) {
                string_type const trail = string_type(it, word.end());
                stream << trail;
            }

            return !words_left;
        }
    };

//
// unordered_list_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct unordered_list_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            std::basic_ostringstream<Char> stream;
            value.safe() ? push_item<true>(value, engine, 0, stream)
                         : push_item<false>(value, engine, 0, stream);
            return Value(stream.str()).mark_safe();
        }

      private:

        inline static boolean_type is_iterable(Value const& item) {
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

        template <boolean_type Safe, class Stream>
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
    };

//
// upper_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct upper_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            return algorithm::to_upper_copy(value.to_string());
        }
    };

//
// urlencode_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct urlencode_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            return detail::uri_encode(value.to_string());
        }
    };

//
// urlize_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct urlize_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());
            return urlize(value, Size(-1), engine.ellipsis);
        }

      private:

        struct formatter {
            Size const limit;
            String const ellipsis;
            formatter(Size const limit, String const& ellipsis)
                : limit(limit), ellipsis(ellipsis) {}

            template <class Match>
            String operator()(Match const& match) const {
                std::basic_ostringstream<Char> stream;
                String const link = match.str();
                String const full = match.str();
                String const text = full.substr(0, limit);
                boolean_type const scheme = !match[xpressive::s1];
                boolean_type const more = text.size() < full.size();
                stream << "<a href='" << (scheme ? "http://" : "") << link;
                stream << "'>" << text << (more ? ellipsis : "") << "</a>";
                return stream.str();
            }
        };

      protected:

        inline static Value urlize(Value const& value, Size const limit, String const& ellipsis) {
            static string_regex_type const safe = +(alnum | (set= '/', '&', '=', ':', ';', '#',
                                                                  '?', '+', '-', '*', '%', '@'));
            static string_regex_type const url = !(s1 = +alnum >> ':') >> +safe >> +('.' >> +safe);

            String const body = value.to_string();
            return Value(regex_replace(body, url, formatter(limit, ellipsis))).mark_safe();
        }
    };

//
// urlizetrunc_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct urlizetrunc_filter : urlize_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());
            return urlize_filter::urlize(value, arguments[0].to_size(), engine.ellipsis);
        }
    };


//
// wordcount_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct wordcount_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (!arguments.empty()) throw_exception(superfluous_argument());

            String const input = value.to_string();
            String const delimiters = text(word_delimiters);
            separator_type const separator(delimiters.c_str());
            tokenizer_type const tokenizer(input, separator);

            return std::distance(tokenizer.begin(), tokenizer.end());
        }

      private:

        typedef Char                            char_type;
        typedef String                          string_type;
        typedef typename String::const_iterator iterator_type;
        typedef char_separator<Char>            separator_type;
        typedef tokenizer < separator_type
                          , iterator_type
                          , String >            tokenizer_type;

    };


//
// wordwrap_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct wordwrap_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            size_type   const width = arguments[0].to_size();
            string_type const text  = value.to_string();
            return wrap(text, width, engine.newline);
        }

      private:

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
    };

//
// yesno_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct yesno_filter {
        inline static value_type process( engine_type   const& engine
                                        , value_type    const& value
                                        , sequence_type const& arguments
                                        , context_type  const& context
                                        , options_type  const& options
                                        ) {
            if (arguments.size() < 1) throw_exception(missing_argument());
            if (arguments.size() > 1) throw_exception(superfluous_argument());

            Value true_, false_, none_;
            sequence_type const sequential_arguments =
                engine.template split_argument<','>(arguments[0], context, options);

            switch (sequential_arguments.size()) {
                case 0:
                case 1:
                    throw_exception(missing_argument());
                case 2:
                    true_  = sequential_arguments[0];
                    false_ = sequential_arguments[1];
                    none_  = sequential_arguments[1];
                    break;
                case 3:
                    true_  = sequential_arguments[0];
                    false_ = sequential_arguments[1];
                    none_  = sequential_arguments[2];
                    break;
                default: throw_exception(superfluous_argument());
            }

            return value.empty() ? none_ : (value ? true_ : false_);
        }
    };
}; // builtin_filters

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_FILTERS_HPP_INCLUDED
