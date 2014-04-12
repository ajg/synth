//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_BUILTIN_FILTERS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_BUILTIN_FILTERS_HPP_INCLUDED

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

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/engines/django/formatter.hpp>

namespace ajg {
namespace synth {
namespace detail {

template <std::size_t Min, std::size_t Max = Min>
struct with_arity {
    inline static void validate(std::size_t const n) {
        if (n > Max) throw_exception(superfluous_argument());
        else if (n < Min) throw_exception(missing_argument());
    }
};

template <std::size_t Max>
struct with_arity<0, Max> {
    inline static void validate(std::size_t const n) {
        if (n > Max) throw_exception(superfluous_argument());
    }
};

} // namespace detail


namespace django {
namespace {

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

namespace algo = boost::algorithm;

} // namespace

template <class Kernel>
struct builtin_filters {
  public:

    typedef Kernel                                                              kernel_type;
    typedef typename kernel_type::engine_type                                   engine_type;
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
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::string_type                                   string_type;

    typedef typename options_type::arguments_type                               arguments_type;
    typedef typename options_type::context_type                                 context_type;

    typedef value_type (*filter_type)( kernel_type    const&
                                     , value_type     const&
                                     , arguments_type const&
                                     , context_type   const&
                                     , options_type   const&
                                     );

  private:

    typedef boost::basic_format<char_type>                                      format_type;
    typedef std::basic_ostringstream<char_type>                                 string_stream_type;
    typedef typename kernel_type::string_regex_type                             string_regex_type;
    typedef typename string_type::const_iterator                                string_iterator_type;
    typedef xpressive::regex_token_iterator<string_iterator_type>               regex_iterator_type;
    typedef typename regex_iterator_type::value_type                            sub_match_type;
    typedef boost::char_separator<char_type>                                    separator_type;
    typedef boost::tokenizer<separator_type, string_iterator_type, string_type> tokenizer_type;
    typedef django::formatter<options_type>                                     formatter_type;
    typedef std::map<string_type, filter_type>                                  filters_type;

  private:

    inline static separator_type const& separator() {
        static string_type    const word_delimiters(traits_type::literal(" \t\n.,;:!?'\"-"));
        static separator_type const separator(word_delimiters.c_str());
        return separator;
    }

  public:

    inline static filter_type get(string_type const& name) {
        // TODO: Consider replacing with fastmatch.h switch or unordered_map.
        static filters_type const filters = boost::assign::map_list_of
            (traits_type::literal("add"),                add_filter::process)
            (traits_type::literal("addslashes"),         addslashes_filter::process)
            (traits_type::literal("capfirst"),           capfirst_filter::process)
            (traits_type::literal("center"),             center_filter::process)
            (traits_type::literal("cut"),                cut_filter::process)
            (traits_type::literal("date"),               date_filter::process)
            (traits_type::literal("default"),            default_filter::process)
            (traits_type::literal("default_if_none"),    default_if_none_filter::process)
            (traits_type::literal("dictsort"),           dictsort_filter::process)
            (traits_type::literal("dictsortreversed"),   dictsortreversed_filter::process)
            (traits_type::literal("divisibleby"),        divisibleby_filter::process)
            (traits_type::literal("escape"),             escape_filter::process)
            (traits_type::literal("escapejs"),           escapejs_filter::process)
            (traits_type::literal("filesizeformat"),     filesizeformat_filter::process)
            (traits_type::literal("first"),              first_filter::process)
            (traits_type::literal("fix_ampersands"),     fix_ampersands_filter::process)
            (traits_type::literal("floatformat"),        floatformat_filter::process)
            (traits_type::literal("force_escape"),       force_escape_filter::process)
            (traits_type::literal("get_digit"),          get_digit_filter::process)
            (traits_type::literal("iriencode"),          iriencode_filter::process)
            (traits_type::literal("join"),               join_filter::process)
            (traits_type::literal("last"),               last_filter::process)
            (traits_type::literal("length"),             length_filter::process)
            (traits_type::literal("length_is"),          length_is_filter::process)
            (traits_type::literal("linebreaks"),         linebreaks_filter::process)
            (traits_type::literal("linebreaksbr"),       linebreaksbr_filter::process)
            (traits_type::literal("linenumbers"),        linenumbers_filter::process)
            (traits_type::literal("ljust"),              ljust_filter::process)
            (traits_type::literal("lower"),              lower_filter::process)
            (traits_type::literal("make_list"),          make_list_filter::process)
            (traits_type::literal("phone2numeric"),      phone2numeric_filter::process)
            (traits_type::literal("pluralize"),          pluralize_filter::process)
            (traits_type::literal("pprint"),             pprint_filter::process)
            (traits_type::literal("random"),             random_filter::process)
            (traits_type::literal("removetags"),         removetags_filter::process)
            (traits_type::literal("rjust"),              rjust_filter::process)
            (traits_type::literal("safe"),               safe_filter::process)
            (traits_type::literal("safeseq"),            safeseq_filter::process)
            (traits_type::literal("slice"),              slice_filter::process)
            (traits_type::literal("slugify"),            slugify_filter::process)
            (traits_type::literal("stringformat"),       stringformat_filter::process)
            (traits_type::literal("striptags"),          striptags_filter::process)
            (traits_type::literal("time"),               time_filter::process)
            (traits_type::literal("timesince"),          timesince_filter::process)
            (traits_type::literal("timeuntil"),          timeuntil_filter::process)
            (traits_type::literal("title"),              title_filter::process)
            (traits_type::literal("truncatechars"),      truncatechars_filter::process)
            (traits_type::literal("truncatechars_html"), truncatechars_html_filter::process)
            (traits_type::literal("truncatewords"),      truncatewords_filter::process)
            (traits_type::literal("truncatewords_html"), truncatewords_html_filter::process)
            (traits_type::literal("unordered_list"),     unordered_list_filter::process)
            (traits_type::literal("upper"),              upper_filter::process)
            (traits_type::literal("urlencode"),          urlencode_filter::process)
            (traits_type::literal("urlize"),             urlize_filter::process)
            (traits_type::literal("urlizetrunc"),        urlizetrunc_filter::process)
            (traits_type::literal("wordcount"),          wordcount_filter::process)
            (traits_type::literal("wordwrap"),           wordwrap_filter::process)
            (traits_type::literal("yesno"),              yesno_filter::process)
            ;
        typename std::map<string_type, filter_type>::const_iterator it = filters.find(name);
        return it == filters.end() ? 0 : it->second;
    }

//
// add_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct add_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            return value.to_number() + arguments.first[0].to_number();
        }
    };

//
// addslashes_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct addslashes_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            return escape_slashes(value).mark_safe();
        }

        inline static value_type escape_slashes(value_type const& value) {
            string_type const string = value.to_string();
            // TODO: string_stream_type stream;
            if (size_type const escapes = std::count_if(string.begin(), string.end(), algo::is_any_of("'\"\\"))) {
                string_type result;
                result.reserve(string.size() + escapes);

                BOOST_FOREACH(char_type const c, string) {
                    switch (c) {
                    case char_type('\''): result += traits_type::literal("\\'");  break;
                    case char_type('"'):  result += traits_type::literal("\\\""); break;
                    case char_type('\\'): result += traits_type::literal("\\\\"); break;
                    default: result += c;
                    }
                }

                return result;
            }
            else { // No escapes.
                return value;
            }
        }
    };

//
// capfirst_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct capfirst_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            string_type string = value.to_string();
            if (!string.empty()) string[0] = std::toupper(string[0]);
            return string;
        }
    };

//
// center_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct center_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            string_type const string = value.to_string();
			size_type   const width  = arguments.first[0].to_size();
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
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            string_type const from = value.to_string();
            string_type const what = arguments.first[0].to_string();
            return algo::erase_all_copy(from, what);
        }
    };

//
// date_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct date_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0, 1>::validate(arguments.first.size());
            string_type const format = arguments.first.empty() ?
                traits_type::literal("DATE_FORMAT") : arguments.first[0].to_string();
            return formatter_type::format_datetime(options, format, value.to_datetime());
        }
    };

//
// default_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct default_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            return value ? value : arguments.first[0];
        }
    };

//
// default_if_none_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct default_if_none_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            return value.is_none() ? arguments.first[0] : value;
        }
    };

//
// dictsort_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct dictsort_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            return value.sort_by(arguments.first[0], false);
        }
    };

//
// dictsortreversed_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct dictsortreversed_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            return value.sort_by(arguments.first[0], true);
        }
    };

//
// divisibleby_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct divisibleby_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            integer_type const dividend = static_cast<integer_type>(value.to_number());
			integer_type const divisor  = static_cast<integer_type>(arguments.first[0].to_number());
            return dividend % divisor == 0;
        }
    };

//
// escape_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct escape_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            return value.copy().mark_unsafe();
        }
    };

//
// escapejs_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct escapejs_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            string_type string = value.to_string(), result;
            result.reserve(string.size()); // Assume no escapes.

            BOOST_FOREACH(char_type const c, string) {
                result += c < 32 ? traits_type::literal("\\x") + detail::to_hex<2>(c) : string_type(1, c);
            }

            return result;
        }
    };

//
// filesizeformat_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct filesizeformat_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            integer_type const integer = static_cast<integer_type>(value.to_number());
            size_type    const size    = static_cast<size_type>((std::abs)(integer));
            return detail::format_size<string_type>(size);
        }
    };

//
// first_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct first_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            if (value.empty()) throw_exception(std::invalid_argument("sequence"));
            return value.front();
        }
    };

//
// fix_ampersands_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct fix_ampersands_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            static string_regex_type const regex = as_xpr('&') >> ~before((+_w | '#' >> +_d) >> ';');
            return value_type(regex_replace(value.to_string(), regex, traits_type::literal("&amp;"))).mark_safe();
        }
    };

//
// floatformat_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct floatformat_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0, 1>::validate(arguments.first.size());
            // Get the number and the decimal places.
            string_stream_type stream;
            int const n = arguments.first.empty() ? -1 : static_cast<int>(arguments.first[0].to_number());
            number_type const number = value.to_number();

            // If it's an integer and n < 0, we don't want decimals.
            boolean_type const is_integer = detail::is_integer(number);
            int const precision = n < 0 && is_integer ? 0 : std::abs(n);
            stream << std::fixed << std::setprecision(precision) << number;

            return value_type(stream.str()).mark_safe();
        }
    };

//
// force_escape_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct force_escape_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            return value.escape().mark_safe();
        }
    };

//
// get_digit_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct get_digit_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            try {
                number_type  const number   = value.to_number();
                integer_type const position = static_cast<integer_type>(arguments.first[0].to_number());
                integer_type const integer  = static_cast<integer_type>(number);

                if (position > 0) {
                    // TODO: Use detail::is_integer.
                    if (number == integer && integer >= 1) { // Ensure the number operated on is whole.
                        string_type const text     = behavior_type::to_string(integer);
						size_type   const distance = static_cast<size_type>(position);

						if (distance <= text.length()) {
							return *(text.end() - distance);
                        }
                    }
                }
            }
            catch (bad_method       const&) {} // Do nothing.
            catch (conversion_error const&) {} // Do nothing.
            // Otherwise, (e.g. if any of the above failed):
            return value;
        }
    };

//
// iriencode_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct iriencode_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            return detail::iri_encode(value.to_string());
        }
    };

//
// join_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct join_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            string_type const delimiter = arguments.first[0].to_string();

            size_type i = 0;
            string_stream_type stream;
            BOOST_FOREACH(value_type const& v, value) {
                if (i++) stream << delimiter;
                stream << v;
            }

            BOOST_ASSERT(stream);
            value_type result = stream.str();
            result.safe(value.safe());
            return result;
        }
    };

//
// last_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct last_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            if (value.empty()) throw_exception(std::invalid_argument("sequence"));
            return value.back();
        }
    };

//
// length_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct length_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            return value.size();
        }
    };

//
// length_is_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct length_is_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
			return arguments.first[0].to_size() == value.size();
        }
    };

//
// linebreaks_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct linebreaks_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            static string_regex_type const newline  = _ln;
            static string_regex_type const newlines = _ln >> +_ln;

            string_stream_type stream;
            string_type const input = regex_replace(value.to_string(), newline, kernel.newline);

            regex_iterator_type begin(input.begin(), input.end(), newlines, -1), end;
            boolean_type const safe = !options.autoescape || value.safe();

            BOOST_FOREACH(string_type const& line, std::make_pair(begin, end)) {
                string_type p = safe ? value_type(line).escape().to_string() : line;
                algo::replace_all(p, kernel.newline, traits_type::literal("<br />"));
                stream << "<p>" << p << "</p>" << std::endl << std::endl;
            }

            return value_type(stream.str()).mark_safe();
        }
    };

//
// linebreaksbr_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct linebreaksbr_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            string_type const text = algo::replace_all_copy(value.to_string(),
                kernel.newline, traits_type::literal("<br />"));
            return value_type(text).mark_safe();
        }
    };

//
// linenumbers_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct linenumbers_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            std::vector<string_type>            lines;
            string_stream_type stream;

            string_type  const input   = value.to_string();
            string_type  const pattern = traits_type::literal("%%0%dd. %%s");
            boolean_type const safe    = !options.autoescape || value.safe();

            algo::split(lines, input, algo::is_any_of("\n"));

            size_type   const width = behavior_type::to_string(lines.size()).size();
            string_type const spec  = (format_type(pattern) % width).str();

            size_type i = 1;
            BOOST_FOREACH(value_type const& line, lines) {
                value_type const item = safe ? line : line.escape();
                stream << (format_type(spec) % i++ % item) << std::endl;
            }

            return value_type(stream.str()).mark_safe();
        }
    };

//
// ljust_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct ljust_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            size_type const width = arguments.first[0].to_size();
            string_stream_type stream;
            stream << std::left << std::setw(width) << value;
            BOOST_ASSERT(stream);
            return stream.str();
        }
    };

//
// lower_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct lower_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            return algo::to_lower_copy(value.to_string());
        }
    };

//
// make_list_filter
//     TODO: Investigate whether this should return a string or a list.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct make_list_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            string_stream_type stream;
            value_type const sequence = value.is_numeric() ? value.to_string() : value;

            size_type i = 0;
            stream << traits_type::literal("[");
            BOOST_FOREACH(value_type const& item, sequence) {
                if (i++) stream << traits_type::literal(", ");
                stream << item;
            }
            stream << traits_type::literal("]");
            return stream.str();
        }
    };

//
// phone2numeric_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct phone2numeric_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            string_type phone = algo::to_lower_copy(value.to_string());
            std::transform(phone.begin(), phone.end(), phone.begin(), translate);
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
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0, 1>::validate(arguments.first.size());
            string_type singular, plural;
            sequence_type const args = arguments.first.empty() ? sequence_type() :
                kernel.template split_argument<','>(arguments.first[0], context, options);

            switch (args.size()) {
            case 0: plural = traits_type::literal("s");           break;
            case 1: plural = args[0].to_string(); break;
            default: // 2+
                singular = args[0].to_string();
                plural   = args[1].to_string();
            }

            return value.to_number() == 1 ? singular : plural;
        }
    };

//
// pprint_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct pprint_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            // NOTE: Since this filter is for debugging, we don't normally try to do anything fancy.
            //       In the Python binding it can be overridden with a call to the real pprint.
            return value.is_string() ? detail::quote(value.to_string(), '\'') : value.to_string();
        }
    };

//
// random_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct random_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            if (size_type const size  = value.size()) {
                size_type const index = detail::random_int(0, size - 1);
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
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            std::vector<string_type> tags;
            formatter const format = { tags };
            int (*predicate)(int) = std::isspace;
            string_type const source = arguments.first[0].to_string();
            algo::split(tags, source, predicate);
            return regex_replace(value.to_string(), kernel.html_tag, format);
        }

      private:

        struct formatter {
            std::vector<string_type> const& tags;

            template <class Match>
            string_type operator()(Match const& match) const {
                string_type const tag = match[s1].str();
                return detail::find_value(tag, tags) ? string_type() : match.str();
            }
        };
    };

//
// rjust_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct rjust_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            size_type const width = arguments.first[0].to_size();
            string_stream_type stream;
            stream << std::right << std::setw(width) << value;
            BOOST_ASSERT(stream);
            return stream.str();
        }
    };

//
// safe_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct safe_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            // NOTE: The to_string is there because `safe` is expected to stringize its operand
            //       immediately, not just mark it safe.
            return options.autoescape ? value_type(value.to_string()).mark_safe() : value;
        }
    };

//
// safeseq_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct safeseq_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            value_type copy = value;

            // FIXME: These values should be mutable.
            BOOST_FOREACH(value_type const& v, copy) {
                const_cast<value_type&>(v).safe(true);
            }

            return copy.mark_safe();
        }
    };

//
// slice_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct slice_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0, 1>::validate(arguments.first.size());
            string_type singular, plural;
            sequence_type const args = arguments.first.empty() ? sequence_type() :
                kernel.template split_argument<':'>(arguments.first[0], context, options);
            if (args.size() < 2) throw_exception(missing_argument());

            sequence_type result;
            value_type const lower = args[0];
            value_type const upper = args[1];
            range_type  range = value.slice
                ( lower ? optional<integer_type>(static_cast<integer_type>(lower.to_number())) : none
                , upper ? optional<integer_type>(static_cast<integer_type>(upper.to_number())) : none
                );
            std::copy(range.first, range.second, std::back_inserter(result));
            return result;
        }
    };

//
// slugify_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct slugify_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            struct invalid {
                inline static boolean_type fn(char_type const c) {
                    return !std::isalnum(c) && c != '_' && c != '-';
                }
            };

            string_type slug = algo::trim_copy(value.to_string());
            std::replace(slug.begin(), slug.end(), char_type(' '), char_type('-'));
            slug.erase(std::remove_if(slug.begin(), slug.end(), invalid::fn), slug.end());
            return algo::to_lower_copy(slug);
        }
    };

//
// stringformat_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct stringformat_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            string_type const spec = arguments.first[0].to_string();
            return (format_type(char_type('%') + spec) % value).str();
        }
    };

//
// striptags_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct striptags_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            static string_regex_type const tag = '<' >> -*~(as_xpr('>')) >> '>';
            return regex_replace(value.to_string(), tag, traits_type::literal(""));
        }
    };

//
// time_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct time_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0, 1>::validate(arguments.first.size());
            string_type const format = arguments.first.empty() ?
                traits_type::literal("TIME_FORMAT") : arguments.first[0].to_string();
            return formatter_type::format_datetime(options, format, value.to_datetime());
        }
    };

//
// timesince_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct timesince_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0, 1>::validate(arguments.first.size());
            datetime_type const to   = value.to_datetime();
            datetime_type const from = arguments.first.empty() ? detail::local_now() : arguments.first[0].to_datetime();
            return value_type(formatter_type::format_duration(options, from - to)).mark_safe();
        }
    };

//
// timeuntil_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct timeuntil_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0, 1>::validate(arguments.first.size());
            datetime_type const to   = value.to_datetime();
            datetime_type const from = arguments.first.empty() ? detail::local_now() : arguments.first[0].to_datetime();
            return value_type(formatter_type::format_duration(options, to - from)).mark_safe();
        }
    };

//
// title_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct title_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            string_type text = value.to_string();

            for (size_type i = 0, n = text.length(); i < n; ++i) {
                boolean_type const boundary = i == 0 || std::isspace(text[i - 1]);
                text[i] = boundary ? std::toupper(text[i]) : std::tolower(text[i]);
            }

            return text;
        }
    };

//
// truncatechars_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct truncatechars_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
			size_type const limit = arguments.first[0].to_size();
            if (limit == 0) return string_type();

            size_type   const ellip = kernel.ellipsis.length();
            string_type const text  = value.to_string();

            if (text.length() > limit) {
                size_type const trunc = ellip < limit ? limit - ellip : 0;
                return text.substr(0, trunc) + kernel.ellipsis;
            }
            else {
                return text;
            }
        }
    };

//
// truncatechars_html_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct truncatechars_html_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
			size_type const limit = arguments.first[0].to_size();
            if (limit == 0) return string_type();

            size_type   const ellip = kernel.ellipsis.length();
            string_type const input = value.to_string();
            string_stream_type stream;

            string_iterator_type last = input.begin(), done = input.end();
            regex_iterator_type begin(last, done, kernel.html_tag), end;
            std::stack<string_type> open_tags;
            size_type length = 0;
            static string_type const boundaries = traits_type::literal(" \t\n\v\f\r>");

            BOOST_FOREACH(sub_match_type const& match, std::make_pair(begin, end)) {
                string_type const tag  = match.str();
                string_type const name = tag.substr(1, tag.find_first_of(boundaries, 1) - 1);
                string_type const text = string_type(last, match.first);

                last = match.second;
                size_type current = length;

                if ((length += text.length()) > limit) {
                    size_type const trunc = current + ellip < limit ? limit - (current + ellip) : 0;
                    stream << text.substr(0, trunc) + kernel.ellipsis;
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
                    stream << text.substr(0, trunc) + kernel.ellipsis;
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
    };

//
// truncatewords_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct truncatewords_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            size_type   const limit = arguments.first[0].to_size();
            string_type const text  = value.to_string();
            size_type count = 0;
            string_stream_type stream;

            process_words(stream, text.begin(), text.end(), count, limit, kernel.ellipsis);
            return stream.str();
        }

      private:

        inline static boolean_type process_words( string_stream_type&         stream
                                                , string_iterator_type const& from
                                                , string_iterator_type const& to
                                                , size_type&                  count
                                                , size_type            const  limit
                                                , string_type          const& ellipsis
                                                ) {
            tokenizer_type const tokenizer(from, to, separator());
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
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            static string_type const boundaries = traits_type::literal(" \t\n\v\f\r>");
			size_type const limit = arguments.first[0].to_size();
            if (limit == 0) return string_type();

            size_type count = 0;
            string_stream_type stream;
            string_type const input = value.to_string();
            string_iterator_type last = input.begin(), done = input.end();
            regex_iterator_type begin(last, done, kernel.html_tag), end;
            std::stack<string_type> open_tags;

            BOOST_FOREACH(sub_match_type const& match, std::make_pair(begin, end)) {
                string_type   const tag  = match.str();
                string_type   const name = tag.substr(1, tag.find_first_of(boundaries, 1) - 1);
                string_iterator_type const prev = last; last = match.second;

                if (!process_words(stream, prev, match.first, count, limit, kernel.ellipsis) || count >= limit) {
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

            if (!process_words(stream, last, done, count, limit, kernel.ellipsis) || count >= limit) {
                while (!open_tags.empty()) {
                    stream << "</" << open_tags.top() << ">";
                    open_tags.pop();
                }
            }

            return value_type(stream.str()).mark_safe();
        }

      private:

        inline static boolean_type process_words( string_stream_type&         stream
                                                , string_iterator_type const& from
                                                , string_iterator_type const& to
                                                , size_type&                  count
                                                , size_type            const  limit
                                                , string_type          const& ellipsis
                                                ) {
            tokenizer_type const tokenizer(from, to, separator());
            typename tokenizer_type::const_iterator       word = tokenizer.begin();
            typename tokenizer_type::const_iterator const stop = tokenizer.end();
            string_iterator_type it = from;

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
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            string_stream_type stream;
            value.safe() ? push_item<true>(value, kernel, 0, stream)
                         : push_item<false>(value, kernel, 0, stream);
            return value_type(stream.str()).mark_safe();
        }

      private:

        template <boolean_type Safe, class Stream>
        inline static void push_item( value_type  const& item
                                    , kernel_type const& kernel
                                    , size_type   const  level
                                    , Stream&            out
                                    ) {
            string_type const indent(level, char_type('\t'));

            if (!item.is_string() && item.is_iterable()) {
                if (size_type const size = item.size()) {
                    for (size_type i = 0; i < size; ++i) {
                        value_type const& value = item[i];
                        out << indent << "<li>";
                        Safe ? out << value : out << value.escape();

                        if (++i < size) {
                            value_type const& next = item[i];

                            if (!next.is_string() && next.is_iterable()) {
                                out << std::endl << indent << "<ul>" << std::endl;
                                push_item<Safe>(next, kernel, level + 1, out);
                                out << indent << "</ul>" << std::endl << indent;
                            }
                            else {
                                out << "</li>" << std::endl << indent << "<li>";
                                Safe ? out << next : out << next.escape();
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
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            return algo::to_upper_copy(value.to_string());
        }
    };

//
// urlencode_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct urlencode_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            return detail::uri_encode(value.to_string());
        }
    };

//
// urlize_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct urlize_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            return urlize(value, (std::numeric_limits<size_type>::max)(), kernel.ellipsis);
        }

      private:

        struct formatter {
            size_type   const limit;
            string_type const ellipsis;

            template <class Match>
            string_type operator()(Match const& match) const {
                string_stream_type stream;
                string_type  const link   = match.str();
                string_type  const full   = match.str();
                string_type  const text   = full.substr(0, limit);
                boolean_type const scheme = !match[xpressive::s1];
                boolean_type const more   = text.size() < full.size();
                stream << "<a href='" << (scheme ? "http://" : "") << link;
                stream << "'>" << text << (more ? ellipsis : string_type()) << "</a>";
                return stream.str();
            }
        };

      protected:

        inline static value_type urlize(value_type const& value, size_type const limit, string_type const& ellipsis) {
            static string_regex_type const safe = +(alnum | (set= '/', '&', '=', ':', ';', '#', '?', '+', '-', '*', '%', '@'));
            static string_regex_type const url  = !(s1 = +alnum >> ':') >> +safe >> +('.' >> +safe);

            string_type const body   = value.to_string();
            formatter   const format = {limit, ellipsis};
            return value_type(regex_replace(body, url, format)).mark_safe();
        }
    };

//
// urlizetrunc_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct urlizetrunc_filter : urlize_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            return urlize_filter::urlize(value, arguments.first[0].to_size(), kernel.ellipsis);
        }
    };

//
// wordcount_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct wordcount_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<0>::validate(arguments.first.size());
            string_type    const string = value.to_string();
            tokenizer_type const tokenizer(string, separator());
            return std::distance(tokenizer.begin(), tokenizer.end());
        }
    };

//
// wordwrap_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct wordwrap_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            size_type   const width = arguments.first[0].to_size();
            string_type const text  = value.to_string();
            return wrap(text, width, kernel.newline);
        }

      private:

        inline static string_type wrap( string_type const& input
                                      , size_type   const  width
                                      , string_type const& newline
                                      ) {
            string_type word, result;

            size_type i    = 0;
            char_type last = '\0';
            BOOST_FOREACH(char_type const c, input) {
                if (++i == width) {
                    algo::trim_left(word);
                    result += newline + word;
                    i = word.length();
                    word.clear();
                }
                else if (std::isspace(c) && !std::isspace(last)) {
                    result += word;
                    word.clear();
                }

                word += (last = c);
            }

            result += word;
            return result;
        }
    };

//
// yesno_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct yesno_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type   const& context
                                        , options_type   const& options
                                        ) {
            detail::with_arity<1>::validate(arguments.first.size());
            sequence_type const args =
                kernel.template split_argument<','>(arguments.first[0], context, options);

            switch (args.size()) {
            case 0:
            case 1:
                throw_exception(missing_argument());
            case 3:
                if (value.is_none()) return args[2]; // Else, fall through:
            case 2:
                return value ? args[0] : args[1];
            default:
                throw_exception(superfluous_argument());
            }
        }
    };
}; // builtin_filters

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_BUILTIN_FILTERS_HPP_INCLUDED
