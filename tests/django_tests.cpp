//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <string>

#include <ajg/testing.hpp>
#include <ajg/synth/templates.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/engines/django.hpp>

#include <tests/context_data.hpp>

namespace {

namespace s = ajg::synth;

typedef char                                       char_type;
typedef s::django::engine<>                        engine_type;
typedef s::file_template<char_type, engine_type>   file_template;
typedef s::string_template<char_type, engine_type> string_template;
typedef string_template::context_type              context_type;
typedef tests::context_data<context_type>          context_data_type;
typedef ajg::test_group<context_data_type>         group_type;

group_type group_object("django tests");

} // namespace

AJG_TESTING_BEGIN

#define DJANGO_TEST(name, in, out, context) \
    unit_test(name) { \
        string_template const t(in); \
        ensure_equals(t.render_to_string(context), out); \
    }}} \


DJANGO_TEST(empty w/o context,  "", "",)
DJANGO_TEST(empty with context, "", "", context)

DJANGO_TEST(text w/o context,  "ABC", "ABC",)
DJANGO_TEST(text with context, "ABC", "ABC", context)

DJANGO_TEST(html w/o context,  "<foo>\nA foo <bar /> element.\n</foo>", "<foo>\nA foo <bar /> element.\n</foo>",)
DJANGO_TEST(html with context, "<foo>\nA foo <bar /> element.\n</foo>", "<foo>\nA foo <bar /> element.\n</foo>", context)

DJANGO_TEST(variable_tag w/o context,  "{{ foo }} {{ bar }} {{ qux }}", "  ",)
DJANGO_TEST(variable_tag with context, "{{ foo }} {{ bar }} {{ qux }}", "A B C", context)

DJANGO_TEST(for_tag with value,
    "{% for v in friends %}[{{ v }}]{% endfor %}",
    "[age: 23, name: joe][age: 55, name: bob][age: 41, name: lou]", context)

/*
DJANGO_TEST(for_tag with key and value,
    "{% for k, v in friends %}[{{ k }}| {{ v }}]{% endfor %}",
    "[0| age: 23, name: joe][1| age: 55, name: bob][2| age: 41, name: lou]", context)
*/

DJANGO_TEST(verbatim_tag w/o context,
        "{% verbatim %}{% for v in friends %}\n"
        "    <p>{{ v }}</p>\n"
        "{% endfor %}{% endverbatim %}\n",
        "{% for v in friends %}\n"
        "    <p>{{ v }}</p>\n"
        "{% endfor %}\n",)

DJANGO_TEST(verbatim_tag with context,
        "{% verbatim %}{% for v in friends %}\n"
        "    <p>{{ v }}</p>\n"
        "{% endfor %}{% endverbatim %}\n",
        "{% for v in friends %}\n"
        "    <p>{{ v }}</p>\n"
        "{% endfor %}\n", context)


DJANGO_TEST(plain float,   "{{3.3}}",  "3.3", context)
DJANGO_TEST(plain float 0, "{{3.30}}", "3.3", context)

DJANGO_TEST(floatformat_filter A, "{{34.23234|floatformat }}", "34.2", context)
DJANGO_TEST(floatformat_filter B, "{{34.00000|floatformat }}", "34",   context)
DJANGO_TEST(floatformat_filter C, "{{34.26000|floatformat }}", "34.3", context)

DJANGO_TEST(floatformat_filter D, "{{34.23234|floatformat:3 }}", "34.232", context)
DJANGO_TEST(floatformat_filter E, "{{34.00000|floatformat:3 }}", "34.000", context)
DJANGO_TEST(floatformat_filter F, "{{34.26000|floatformat:3 }}", "34.260", context)

DJANGO_TEST(floatformat_filter G, "{{34.23234|floatformat:\"-3\" }}", "34.232", context)
DJANGO_TEST(floatformat_filter H, "{{34.00000|floatformat:\"-3\" }}", "34",     context)
DJANGO_TEST(floatformat_filter I, "{{34.26000|floatformat:\"-3\" }}", "34.260", context)

DJANGO_TEST(lower_filter, "{{ \"Still MAD At Yoko\" | lower }}", "still mad at yoko", context)

DJANGO_TEST(removetags_filter A, "{{ \"<b>Joel</b> <button>is</button> a <span>slug</span>\" | removetags:\"b span\"|safe }}", "Joel <button>is</button> a slug", context)
DJANGO_TEST(removetags_filter B, "{{ \"<b>Begin</b> <foo /> <foo/> </foo> <foo> <span attr='value'>End</span>\" | removetags:\"b span foo\"|safe }}", "Begin     End", context)

DJANGO_TEST(title_filter, "{{ \"my FIRST post\" | title }}", "My First Post", context)

DJANGO_TEST(truncatechars_filter  1, "{{ \"Joel is a slug\" | truncatechars: 1 }}", ".", context)
DJANGO_TEST(truncatechars_filter  2, "{{ \"Joel is a slug\" | truncatechars: 2 }}", "..", context)
DJANGO_TEST(truncatechars_filter  3, "{{ \"Joel is a slug\" | truncatechars: 3 }}", "...", context)
DJANGO_TEST(truncatechars_filter  4, "{{ \"Joel is a slug\" | truncatechars: 4 }}", "J...", context)
DJANGO_TEST(truncatechars_filter  5, "{{ \"Joel is a slug\" | truncatechars: 5 }}", "Jo...", context)
DJANGO_TEST(truncatechars_filter  6, "{{ \"Joel is a slug\" | truncatechars: 6 }}", "Joe...", context)
DJANGO_TEST(truncatechars_filter  7, "{{ \"Joel is a slug\" | truncatechars: 7 }}", "Joel...", context)
DJANGO_TEST(truncatechars_filter  8, "{{ \"Joel is a slug\" | truncatechars: 8 }}", "Joel ...", context)
DJANGO_TEST(truncatechars_filter  9, "{{ \"Joel is a slug\" | truncatechars: 9 }}", "Joel i...", context)
DJANGO_TEST(truncatechars_filter 10, "{{ \"Joel is a slug\" | truncatechars:10 }}", "Joel is...", context)
DJANGO_TEST(truncatechars_filter 11, "{{ \"Joel is a slug\" | truncatechars:11 }}", "Joel is ...", context)
DJANGO_TEST(truncatechars_filter 12, "{{ \"Joel is a slug\" | truncatechars:12 }}", "Joel is a...", context)
DJANGO_TEST(truncatechars_filter 13, "{{ \"Joel is a slug\" | truncatechars:13 }}", "Joel is a ...", context)
DJANGO_TEST(truncatechars_filter 14, "{{ \"Joel is a slug\" | truncatechars:14 }}", "Joel is a slug", context)

DJANGO_TEST(truncatechars_html_filter  8, "{{ \"<p>Joel is a slug</p>\" | truncatechars_html: 8 }}", "<p>Joel ...</p>", context)
DJANGO_TEST(truncatechars_html_filter  9, "{{ \"<p>Joel is a slug</p>\" | truncatechars_html: 9 }}", "<p>Joel i...</p>", context)
DJANGO_TEST(truncatechars_html_filter 10, "{{ \"<p>Joel is a slug</p>\" | truncatechars_html:10 }}", "<p>Joel is...</p>", context)

DJANGO_TEST(truncatewords_filter 1, "{{ \"Joel is a slug\" | truncatewords:1 }}", "Joel ...", context)
DJANGO_TEST(truncatewords_filter 2, "{{ \"Joel is a slug\" | truncatewords:2 }}", "Joel is ...", context)
DJANGO_TEST(truncatewords_filter 3, "{{ \"Joel is a slug\" | truncatewords:3 }}", "Joel is a ...", context)

DJANGO_TEST(truncatewords_html_filter 1, "{{ \"<p>Joel is a slug</p>\" | truncatewords_html:1 }}", "<p>Joel ...</p>", context)
DJANGO_TEST(truncatewords_html_filter 2, "{{ \"<p>Joel is a slug</p>\" | truncatewords_html:2 }}", "<p>Joel is ...</p>", context)
DJANGO_TEST(truncatewords_html_filter 3, "{{ \"<p>Joel is a slug</p>\" | truncatewords_html:3 }}", "<p>Joel is a ...</p>", context)

DJANGO_TEST(upper_filter, "{{ \"Joel is a slug\" | upper }}", "JOEL IS A SLUG", context)

DJANGO_TEST(urlize_filter, "{{ \"This is some text containing a http://www.url.com sir and also another.url.com.\" | urlize }}", "This is some text containing a <a href='http://www.url.com'>http://www.url.com</a> sir and also <a href='http://another.url.com'>another.url.com</a>.", context)

DJANGO_TEST(urlizetrunc_filter, "{{ \"This is some text containing a http://www.url.com sir and also another.url.com.\" | urlizetrunc:15 }}", "This is some text containing a <a href='http://www.url.com'>http://www.url....</a> sir and also <a href='http://another.url.com'>another.url.com</a>.", context)

DJANGO_TEST(yesno_filter yes, "{{ true_var|yesno:'Yes,No' }}", "Yes", context)
DJANGO_TEST(yesno_filter no, "{{ false_var|yesno:'Yes,No' }}", "No", context)
// DJANGO_TEST(yesno_filter maybe, "{{ no_var|yesno:'Yes,No,Maybe' }}", "Maybe", context)

/*
TODO:
{% for k, v in a_string_array %}
    {% cycle k v as ttt %} hello. {{ttt}}
{% endfor %}
*/

/*
Tested:

// django::autoescape_tag
// django::block_tag
// django::comment_tag
// django::csrf_token_tag
// django::cycle_tag
// django::debug_tag
// django::extends_tag
// django::filter_tag
// django::firstof_tag
django::for_tag
// django::for_empty_tag
// django::if_tag
// django::ifchanged_tag
// django::ifequal_tag
// django::ifnotequal_tag
// django::include_tag
// django::load_tag
// django::load_from_tag
// django::now_tag
// django::regroup_tag
// django::spaceless_tag
// django::ssi_tag
// django::templatetag_tag
// django::url_tag
django::variable_tag
django::verbatim_tag
// django::widthratio_tag
// django::with_tag
// django::library_tag

// django::add_filter
// django::addslashes_filter
// django::capfirst_filter
// django::center_filter
// django::cut_filter
// django::date_filter
// django::default_filter
// django::default_if_none_filter
// django::dictsort_filter
// django::dictsortreversed_filter
// django::divisibleby_filter
// django::escape_filter
// django::escapejs_filter
// django::filesizeformat_filter
// django::first_filter
// django::fix_ampersands_filter
django::floatformat_filter
// django::force_escape_filter
// django::get_digit_filter
// django::iriencode_filter
// django::join_filter
// django::last_filter
// django::length_filter
// django::length_is_filter
// django::linebreaks_filter
// django::linebreaksbr_filter
// django::linenumbers_filter
// django::ljust_filter
django::lower_filter
// django::make_list_filter
// django::phone2numeric_filter
// django::pluralize_filter
// django::pprint_filter
// django::random_filter
django::removetags_filter
// django::rjust_filter
// django::safe_filter
// django::safeseq_filter
// django::slice_filter
// django::slugify_filter
// django::stringformat_filter
// django::striptags_filter
// django::time_filter
// django::timesince_filter
// django::timeuntil_filter
django::title_filter
django::truncatechars_filter
django::truncatechars_html_filter
django::truncatewords_filter
django::truncatewords_html_filter
// django::unordered_list_filter
django::upper_filter
// django::urlencode_filter
django::urlize_filter
django::urlizetrunc_filter
// django::wordcount_filter
// django::wordwrap_filter
django::yesno_filter

*/
