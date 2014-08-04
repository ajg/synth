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

#include <boost/xpressive/regex_token_iterator.hpp>

// TODO[c++11]: Replace with <random>.
// XXX: Not used because it is not header-only:
// #include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <ajg/synth/detail/has_fraction.hpp>
#include <ajg/synth/detail/text.hpp>
#include <ajg/synth/engines/django/formatter.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace django {

template <class Kernel>
struct builtin_filters {
  public:

    typedef Kernel                                                              kernel_type;
    typedef typename kernel_type::engine_type                                   engine_type;
    typedef typename kernel_type::state_type                                    state_type;

    typedef typename engine_type::options_type                                  options_type;
    typedef typename engine_type::value_type                                    value_type;

    typedef typename value_type::range_type                                     range_type;
    typedef typename value_type::sequence_type                                  sequence_type;
    typedef typename value_type::association_type                               association_type;
    typedef typename value_type::index_type                                     index_type;
    typedef typename value_type::traits_type                                    traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::integer_type                                  integer_type;
    typedef typename traits_type::floating_type                                 floating_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::string_type                                   string_type;

    typedef typename options_type::arguments_type                               arguments_type;
    typedef typename options_type::context_type                                 context_type;

    typedef detail::text<string_type>                                           text;

    typedef value_type (*filter_type)( kernel_type    const&
                                     , options_type   const&
                                     , state_type     const&
                                     , value_type     const&
                                     , arguments_type const&
                                     , context_type&
                                     );

  private:

    typedef boost::basic_format<char_type>                                      format_type;
    typedef std::basic_ostringstream<char_type>                                 string_stream_type;
    typedef typename kernel_type::string_regex_type                             string_regex_type;
    typedef typename string_type::const_iterator                                string_iterator_type;
    typedef x::regex_token_iterator<string_iterator_type>                       regex_iterator_type;
    typedef typename regex_iterator_type::value_type                            sub_match_type;
    typedef boost::char_separator<char_type>                                    separator_type;
    typedef boost::tokenizer<separator_type, string_iterator_type, string_type> tokenizer_type;
    typedef django::formatter<options_type>                                     formatter_type;
    typedef std::map<string_type, filter_type>                                  filters_type;

  private:

    inline static separator_type const& separator() {
        static string_type    const word_delimiters(text::literal(" \t\n.,;:!?'\"-"));
        static separator_type const separator(word_delimiters.c_str());
        return separator;
    }

    template <std::size_t Min, std::size_t Max = Min>
    struct with_arity {
        inline static void validate(std::size_t const n) {
            if (n > Max) AJG_SYNTH_THROW(superfluous_argument());
            else if (n < Min) AJG_SYNTH_THROW(missing_argument());
        }
    };

    template <std::size_t Max>
    struct with_arity<0, Max> {
        inline static void validate(std::size_t const n) {
            if (n > Max) AJG_SYNTH_THROW(superfluous_argument());
        }
    };

  public:

    inline static filter_type get(string_type const& name) {
        // TODO: Consider replacing with fastmatch.h switch or unordered_map.
        static filters_type const filters = boost::assign::map_list_of
            (text::literal("add"),                add_filter::process)
            (text::literal("addslashes"),         addslashes_filter::process)
            (text::literal("capfirst"),           capfirst_filter::process)
            (text::literal("center"),             center_filter::process)
            (text::literal("cut"),                cut_filter::process)
            (text::literal("date"),               date_filter::process)
            (text::literal("default"),            default_filter::process)
            (text::literal("default_if_none"),    default_if_none_filter::process)
            (text::literal("dictsort"),           dictsort_filter::process)
            (text::literal("dictsortreversed"),   dictsortreversed_filter::process)
            (text::literal("divisibleby"),        divisibleby_filter::process)
            (text::literal("escape"),             escape_filter::process)
            (text::literal("escapejs"),           escapejs_filter::process)
            (text::literal("filesizeformat"),     filesizeformat_filter::process)
            (text::literal("first"),              first_filter::process)
            (text::literal("fix_ampersands"),     fix_ampersands_filter::process)
            (text::literal("floatformat"),        floatformat_filter::process)
            (text::literal("force_escape"),       force_escape_filter::process)
            (text::literal("get_digit"),          get_digit_filter::process)
            (text::literal("iriencode"),          iriencode_filter::process)
            (text::literal("join"),               join_filter::process)
            (text::literal("last"),               last_filter::process)
            (text::literal("length"),             length_filter::process)
            (text::literal("length_is"),          length_is_filter::process)
            (text::literal("linebreaks"),         linebreaks_filter::process)
            (text::literal("linebreaksbr"),       linebreaksbr_filter::process)
            (text::literal("linenumbers"),        linenumbers_filter::process)
            (text::literal("ljust"),              ljust_filter::process)
            (text::literal("lower"),              lower_filter::process)
            (text::literal("make_list"),          make_list_filter::process)
            (text::literal("phone2numeric"),      phone2numeric_filter::process)
            (text::literal("pluralize"),          pluralize_filter::process)
            (text::literal("pprint"),             pprint_filter::process)
            (text::literal("random"),             random_filter::process)
            (text::literal("removetags"),         removetags_filter::process)
            (text::literal("rjust"),              rjust_filter::process)
            (text::literal("safe"),               safe_filter::process)
            (text::literal("safeseq"),            safeseq_filter::process)
            (text::literal("slice"),              slice_filter::process)
            (text::literal("slugify"),            slugify_filter::process)
            (text::literal("stringformat"),       stringformat_filter::process)
            (text::literal("striptags"),          striptags_filter::process)
            (text::literal("time"),               time_filter::process)
            (text::literal("timesince"),          timesince_filter::process)
            (text::literal("timeuntil"),          timeuntil_filter::process)
            (text::literal("title"),              title_filter::process)
            (text::literal("truncatechars"),      truncatechars_filter::process)
            (text::literal("truncatechars_html"), truncatechars_html_filter::process)
            (text::literal("truncatewords"),      truncatewords_filter::process)
            (text::literal("truncatewords_html"), truncatewords_html_filter::process)
            (text::literal("unordered_list"),     unordered_list_filter::process)
            (text::literal("upper"),              upper_filter::process)
            (text::literal("urlencode"),          urlencode_filter::process)
            (text::literal("urlize"),             urlize_filter::process)
            (text::literal("urlizetrunc"),        urlizetrunc_filter::process)
            (text::literal("wordcount"),          wordcount_filter::process)
            (text::literal("wordwrap"),           wordwrap_filter::process)
            (text::literal("yesno"),              yesno_filter::process)
            ;
        typename std::map<string_type, filter_type>::const_iterator it = filters.find(name);
        return it == filters.end() ? 0 : it->second;
    }

//
// add_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct add_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            return value.to_number() + arguments.first[0].to_number();
        }
    };

//
// addslashes_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct addslashes_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            return escape_slashes(value).mark_safe();
        }

        inline static value_type escape_slashes(value_type const& value) {
            string_type const string = value.to_string();
            string_stream_type ss;

            BOOST_FOREACH(char_type const c, string) {
                switch (c) {
                case char_type('\''): ss << "\\'";  break;
                case char_type('"'):  ss << "\\\""; break;
                case char_type('\\'): ss << "\\\\"; break;
                default: ss << c;
                }
            }

            AJG_SYNTH_ASSERT(ss);
            return ss.str();
        }
    };

//
// capfirst_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct capfirst_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
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
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
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
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            string_type const from = value.to_string();
            string_type const what = arguments.first[0].to_string();
            return text::remove(from, what);
        }
    };

//
// date_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct date_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0, 1>::validate(arguments.first.size());
            string_type const f = arguments.first.empty() ?
                text::literal("DATE_FORMAT") : arguments.first[0].to_string();
            return formatter_type::format_datetime(context.format_or(f, f), value.to_datetime());
        }
    };

//
// default_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct default_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            return value ? value : arguments.first[0];
        }
    };

//
// default_if_none_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct default_if_none_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            return value.is_unit() ? arguments.first[0] : value;
        }
    };

//
// dictsort_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct dictsort_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            return value.sort_by(arguments.first[0], false);
        }
    };

//
// dictsortreversed_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct dictsortreversed_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            return value.sort_by(arguments.first[0], true);
        }
    };

//
// divisibleby_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct divisibleby_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            integer_type const dividend = value.to_integer();
            integer_type const divisor  = arguments.first[0].to_integer();
            return dividend % divisor == 0;
        }
    };

//
// escape_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct escape_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            return value.metacopy().mark_unsafe();
        }
    };

//
// escapejs_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct escapejs_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            return text::escape_controls(value.to_string());
        }
    };

//
// filesizeformat_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct filesizeformat_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            return traits_type::format_size(value.to_size());
        }
    };

//
// first_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct first_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            if (value.empty()) AJG_SYNTH_THROW(std::invalid_argument("sequence"));
            return value.front();
        }
    };

//
// fix_ampersands_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct fix_ampersands_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            static string_regex_type const regex = as_xpr('&') >> ~x::before((+_w | '#' >> +_d) >> ';');
            return value_type(x::regex_replace(value.to_string(), regex, text::literal("&amp;"))).mark_safe();
        }
    };

//
// floatformat_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct floatformat_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0, 1>::validate(arguments.first.size());
            // Get the number and the decimal places.
            string_stream_type stream;
            integer_type  const n = arguments.first.empty() ? -1 : arguments.first[0].to_integer();
            floating_type const f = value.to_floating();

            // If it's an integer and n < 0, we don't want decimals.
            integer_type const precision = (n < 0 && !detail::has_fraction(f)) ? 0 : (std::abs)(n);
            stream << std::fixed << std::setprecision(precision) << f;

            return value_type(stream.str()).mark_safe();
        }
    };

//
// force_escape_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct force_escape_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            return value.escape().mark_safe();
        }
    };

//
// get_digit_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct get_digit_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            try {
                number_type   const number   = value.to_number();
                integer_type  const integer  = value.to_integer();
                integer_type  const position = arguments.first[0].to_integer();

                if (position > 0) {
                    // TODO: Use detail::has_fraction.
                    if (number == integer && integer >= 1) { // Ensure the number operated on is whole.
                        string_type const text     = text::stringize(integer);
                        size_type   const distance = static_cast<size_type>(position);

                        if (distance <= text.length()) {
                            return *(text.end() - distance);
                        }
                    }
                }
            }
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
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            return text::iri_encode(value.to_string());
        }
    };

//
// join_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct join_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            string_type const delimiter = arguments.first[0].to_string();

            size_type i = 0;
            string_stream_type stream;
            BOOST_FOREACH(value_type const& v, value) {
                if (i++) stream << delimiter;
                stream << v;
            }

            AJG_SYNTH_ASSERT(stream);
            value_type v((stream.str()));
            return value.safe() ? v.mark_safe() : v.mark_unsafe();
        }
    };

//
// last_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct last_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            if (value.empty()) AJG_SYNTH_THROW(std::invalid_argument("sequence"));
            return value.back();
        }
    };

//
// length_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct length_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            return value.size();
        }
    };

//
// length_is_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct length_is_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            return arguments.first[0].to_size() == value.size();
        }
    };

//
// linebreaks_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct linebreaks_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            static string_regex_type const newline  = _ln;
            static string_regex_type const newlines = _ln >> +_ln;

            string_stream_type stream;
            string_type const input = x::regex_replace(value.to_string(), newline, kernel.newline);

            regex_iterator_type begin(input.begin(), input.end(), newlines, -1), end;
            boolean_type const safe = context.safe() || value.safe();

            BOOST_FOREACH(string_type const& line, std::make_pair(begin, end)) {
                string_type const s = safe ? value_type(line).escape().to_string() : line;
                string_type const p = text::replace(s, kernel.newline, text::literal("<br />"));
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
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            string_type const s = text::replace(value.to_string(), kernel.newline, text::literal("<br />"));
            return value_type(s).mark_safe();
        }
    };

//
// linenumbers_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct linenumbers_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            string_stream_type stream;

            string_type  const input   = value.to_string();
            string_type  const pattern = text::literal("%%0%dd. %%s");
            boolean_type const safe    = context.safe() || value.safe();

            std::vector<string_type> const& lines = text::split(input, text::literal("\n"));
            size_type   const width = text::stringize(lines.size()).size();
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
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            size_type const width = arguments.first[0].to_size();
            string_stream_type stream;
            stream << std::left << std::setw(width) << value;
            AJG_SYNTH_ASSERT(stream);
            return stream.str();
        }
    };

//
// lower_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct lower_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            return text::lower(value.to_string());
        }
    };

//
// make_list_filter
//     TODO: Investigate whether this should return a string or a list.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct make_list_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            string_stream_type stream;
            value_type const sequence = value.is_numeric() ? value.to_string() : value;

            size_type i = 0;
            stream << text::literal("[");
            BOOST_FOREACH(value_type const& item, sequence) {
                if (i++) stream << text::literal(", ");
                stream << item;
            }
            stream << text::literal("]");
            return stream.str();
        }
    };

//
// phone2numeric_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct phone2numeric_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            string_type phone = text::lower(value.to_string());
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
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0, 1>::validate(arguments.first.size());
            string_type singular, plural;
            sequence_type const args = arguments.first.empty() ? sequence_type() :
                kernel.split_argument(arguments.first[0], char_type(','));

            switch (args.size()) {
            case 0: plural = text::literal("s");  break;
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
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            // NOTE: Since this filter is for debugging, we don't normally try to do anything fancy.
            //       In the Python binding it can be overridden with a call to the real pprint.
            return value.is_textual() ? text::quote(value.to_string(), '\'') : value.to_string();
        }
    };

//
// random_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct random_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());

            if (size_type const size = value.size()) {
                // TODO[c++11]: Replace with std::random_device.
                // XXX: Not used because it is not header-only.
                // boost::random::random_device device;
                boost::random::mt19937 device(static_cast<unsigned int>((std::time)(0)));
                boost::random::uniform_int_distribution<size_type> distribution(0, size - 1);
                size_type const index = distribution(device);
                return value[index];
            }
            else {
                AJG_SYNTH_THROW(std::invalid_argument("empty sequence"));
            }
        }
    };

//
// removetags_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct removetags_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            string_type const source = arguments.first[0].to_string();
            replacer const r = { text::space(source) };
            return x::regex_replace(value.to_string(), kernel.html_tag, r);
        }

      private:

        struct replacer {
            std::vector<string_type> const tags;

            template <class Match>
            string_type operator()(Match const& match) const {
                string_type const tag = match[s1].str();
                return detail::find(tag, tags) ? string_type() : match.str();
            }
        };
    };

//
// rjust_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct rjust_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            size_type const width = arguments.first[0].to_size();
            string_stream_type stream;
            stream << std::right << std::setw(width) << value;
            AJG_SYNTH_ASSERT(stream);
            return stream.str();
        }
    };

//
// safe_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct safe_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            // NOTE: The to_string is there because `safe` is expected to stringize its operand
            //       immediately, not just mark it safe.
            return context.safe() ? value : value_type(value.to_string()).mark_safe();
        }
    };

//
// safeseq_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct safeseq_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            sequence_type result;

            BOOST_FOREACH(value_type const& element, value) {
                result.push_back(element.metacopy().mark_safe());
            }

            return value_type(result).mark_safe();
        }
    };

//
// slice_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct slice_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0, 1>::validate(arguments.first.size());
            string_type singular, plural;
            sequence_type const args = arguments.first.empty() ? sequence_type() :
                kernel.split_argument(arguments.first[0], char_type(':'));
            if (args.size() < 2) AJG_SYNTH_THROW(missing_argument());

            sequence_type seq;
            value_type const lower = args[0];
            value_type const upper = args[1];
            range_type  range = value.slice
                ( lower ? index_type(lower.to_integer()) : index_type()
                , upper ? index_type(upper.to_integer()) : index_type()
                );
            std::copy(range.first, range.second, std::back_inserter(seq));
            return seq;
        }
    };

//
// slugify_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct slugify_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            struct invalid {
                inline static boolean_type fn(char_type const c) {
                    return !std::isalnum(c) && c != '_' && c != '-';
                }
            };

            string_type slug = text::strip(value.to_string());
            std::replace(slug.begin(), slug.end(), char_type(' '), char_type('-'));
            slug.erase(std::remove_if(slug.begin(), slug.end(), invalid::fn), slug.end());
            return text::lower(slug);
        }
    };

//
// stringformat_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct stringformat_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            string_type const spec = arguments.first[0].to_string();
            return (format_type(char_type('%') + spec) % value).str();
        }
    };

//
// striptags_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct striptags_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            static string_regex_type const tag = '<' >> -*~(as_xpr('>')) >> '>';
            return x::regex_replace(value.to_string(), tag, text::literal(""));
        }
    };

//
// time_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct time_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0, 1>::validate(arguments.first.size());
            string_type const f = arguments.first.empty() ?
                text::literal("TIME_FORMAT") : arguments.first[0].to_string();
            return formatter_type::format_datetime(context.format_or(f, f), value.to_datetime());
        }
    };

//
// timesince_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct timesince_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0, 1>::validate(arguments.first.size());
            datetime_type const to   = value.to_datetime();
            datetime_type const from = arguments.first.empty() ?
                traits_type::local_datetime(context.timezone()) :
                arguments.first[0].to_datetime(context.timezone());
            return value_type(formatter_type::format_duration(from - to)).mark_safe();
        }
    };

//
// timeuntil_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct timeuntil_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0, 1>::validate(arguments.first.size());
            datetime_type const to   = value.to_datetime();
            datetime_type const from = arguments.first.empty() ?
                traits_type::local_datetime(context.timezone()) :
                arguments.first[0].to_datetime(context.timezone());
            return value_type(formatter_type::format_duration(to - from)).mark_safe();
        }
    };

//
// title_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct title_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            string_type s = value.to_string();

            for (size_type i = 0, n = s.length(); i < n; ++i) {
                boolean_type const boundary = i == 0 || std::isspace(s[i - 1]);
                s[i] = boundary ? std::toupper(s[i]) : std::tolower(s[i]);
            }

            return s;
        }
    };

//
// truncatechars_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct truncatechars_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            size_type const limit = arguments.first[0].to_size();
            if (limit == 0) return string_type();

            size_type   const ellip = kernel.ellipsis.length();
            string_type const s     = value.to_string();

            if (s.length() > limit) {
                size_type const trunc = ellip < limit ? limit - ellip : 0;
                return s.substr(0, trunc) + kernel.ellipsis;
            }
            else {
                return s;
            }
        }
    };

//
// truncatechars_html_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct truncatechars_html_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            size_type const limit = arguments.first[0].to_size();
            if (limit == 0) return string_type();

            size_type   const ellip = kernel.ellipsis.length();
            string_type const input = value.to_string();
            string_stream_type stream;

            string_iterator_type last = input.begin(), done = input.end();
            regex_iterator_type begin(last, done, kernel.html_tag), end;
            std::stack<string_type> open_tags;
            size_type length = 0;
            static string_type const boundaries = text::literal(" \t\n\v\f\r>");

            BOOST_FOREACH(sub_match_type const& match, std::make_pair(begin, end)) {
                string_type const tag  = match.str();
                string_type const name = tag.substr(1, tag.find_first_of(boundaries, 1) - 1);
                string_type const s(last, match.first);

                last = match.second;
                size_type current = length;

                if ((length += s.length()) > limit) {
                    size_type const trunc = current + ellip < limit ? limit - (current + ellip) : 0;
                    stream << s.substr(0, trunc) + kernel.ellipsis;
                    break;
                }
                else {
                    stream << s << tag;

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
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
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
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            static string_type const boundaries = text::literal(" \t\n\v\f\r>");
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
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
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
                                    , Stream&            ostream
                                    ) {
            string_type const indent(level, char_type('\t'));

            if (!item.is_textual() && item.is_iterable()) {
                if (size_type const size = item.size()) {
                    for (size_type i = 0; i < size; ++i) {
                        value_type const& value = item[i];
                        ostream << indent << "<li>";
                        Safe ? ostream << value : ostream << value.escape();

                        if (++i < size) {
                            value_type const& next = item[i];

                            if (!next.is_textual() && next.is_iterable()) {
                                ostream << std::endl << indent << "<ul>" << std::endl;
                                push_item<Safe>(next, kernel, level + 1, ostream);
                                ostream << indent << "</ul>" << std::endl << indent;
                            }
                            else {
                                ostream << "</li>" << std::endl << indent << "<li>";
                                Safe ? ostream << next : ostream << next.escape();
                            }
                        }

                        ostream << "</li>" << std::endl;
                    }
                }
            }
            else {
                ostream << indent << "<li>";
                Safe ? ostream << item : ostream << item.escape();
                ostream << "</li>" << std::endl;
            }
        }
    };

//
// upper_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct upper_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            return text::upper(value.to_string());
        }
    };

//
// urlencode_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct urlencode_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
            return text::uri_encode(value.to_string());
        }
    };

//
// urlize_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct urlize_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
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
                boolean_type const scheme = !match[s1];
                boolean_type const more   = text.size() < full.size();
                stream << "<a href='" << (scheme ? "http://" : "") << link;
                stream << "'>" << text << (more ? ellipsis : string_type()) << "</a>";
                return stream.str();
            }
        };

      protected:

        inline static value_type urlize(value_type const& value, size_type const limit, string_type const& ellipsis) {
            static string_regex_type const safe = +(x::alnum | (x::set = '/', '&', '=', ':', ';', '#', '?', '+', '-', '*', '%', '@'));
            static string_regex_type const url  = !(s1 = +x::alnum >> ':') >> +safe >> +('.' >> +safe);

            string_type const body   = value.to_string();
            formatter   const format = {limit, ellipsis};
            return value_type(x::regex_replace(body, url, format)).mark_safe();
        }
    };

//
// urlizetrunc_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct urlizetrunc_filter : urlize_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            return urlize_filter::urlize(value, arguments.first[0].to_size(), kernel.ellipsis);
        }
    };

//
// wordcount_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct wordcount_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<0>::validate(arguments.first.size());
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
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            size_type   const width = arguments.first[0].to_size();
            string_type const text  = value.to_string();
            return wrap(text, width, kernel.newline);
        }

      private:

        inline static string_type wrap( string_type const& input
                                      , size_type   const  width
                                      , string_type const& newline
                                      ) {
            string_type word, s;
            // TODO: Use a stream.

            size_type i    = 0;
            char_type last = '\0';
            BOOST_FOREACH(char_type const c, input) {
                if (++i == width) {
                    word = text::strip_left(word);
                    s += newline + word;
                    i = word.length();
                    word.clear();
                }
                else if (std::isspace(c) && !std::isspace(last)) {
                    s += word;
                    word.clear();
                }

                word += (last = c);
            }

            s += word;
            return s;
        }
    };

//
// yesno_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct yesno_filter {
        inline static value_type process( kernel_type    const& kernel
                                        , options_type   const& options
                                        , state_type     const& state
                                        , value_type     const& value
                                        , arguments_type const& arguments
                                        , context_type&         context
                                        ) {
            with_arity<1>::validate(arguments.first.size());
            sequence_type const args = kernel.split_argument(arguments.first[0], char_type(','));

            switch (args.size()) {
            case 0:
            case 1:
                AJG_SYNTH_THROW(missing_argument());
            case 3:
                if (value.is_unit()) return args[2]; // Else, fall through:
            case 2:
                return value ? args[0] : args[1];
            default:
                AJG_SYNTH_THROW(superfluous_argument());
            }
        }
    };
}; // builtin_filters

}}}} // namespace ajg::synth::engines::django

#endif // AJG_SYNTH_ENGINES_DJANGO_BUILTIN_FILTERS_HPP_INCLUDED
