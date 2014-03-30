//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <string>

#include <ajg/testing.hpp>
#include <ajg/synth/templates.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/engines/django.hpp>
#include <ajg/synth/engines/null_resolver.hpp>

#include <tests/data/kitchen_sink.hpp>

namespace {

namespace s = ajg::synth;
using boost::optional;
using s::null_resolver;

typedef char                                                                    char_type;
typedef s::django::engine<>                                                     engine_type;
typedef s::file_template<char_type, engine_type>                                file_template;
typedef s::string_template<char_type, engine_type>                              string_template;
typedef string_template::traits_type                                            traits_type;
typedef string_template::context_type                                           context_type;
typedef string_template::options_type                                           options_type;
typedef tests::data::kitchen_sink<context_type, traits_type, options_type>      data_type;
typedef ajg::test_group<data_type>                                              group_type;

group_type group_object("django");

} // namespace

AJG_TESTING_BEGIN

#define DJANGO_TEST_(name, in, out, context) \
    unit_test(name) { ensure_equals(string_template(in).render_to_string(context), out); }}}

#define DJANGO_TEST(name, in, out) DJANGO_TEST_(name, in, out, context)

#define NO_CONTEXT // Nothing.

/// Sanity checks
////////////////////////////////////////////////////////////////////////////////////////////////////

DJANGO_TEST_(empty, "", "", NO_CONTEXT)
DJANGO_TEST_(empty, "", "", context)

DJANGO_TEST_(text, "ABC", "ABC", NO_CONTEXT)
DJANGO_TEST_(text, "ABC", "ABC", context)

DJANGO_TEST_(html, "<foo>\nA foo <bar /> element.\n</foo>", "<foo>\nA foo <bar /> element.\n</foo>", NO_CONTEXT)
DJANGO_TEST_(html, "<foo>\nA foo <bar /> element.\n</foo>", "<foo>\nA foo <bar /> element.\n</foo>", context)

/// Literal tests
////////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME: DJANGO_TEST(boolean, "{{None}}",   "None")

DJANGO_TEST(boolean, "{{True}}",   "True")
DJANGO_TEST(boolean, "{{False}}",  "False")

DJANGO_TEST(number integral zero, "{{0}}",   "0")
// FIXME: DJANGO_TEST(number integral zero, "{{-0}}",  "0")

DJANGO_TEST(number integral, "{{42}}",   "42")
DJANGO_TEST(number integral, "{{-42}}",  "-42")

DJANGO_TEST(number floating zero, "{{0.0}}",   "0")
DJANGO_TEST(number floating zero, "{{-0.0}}",  "-0")

DJANGO_TEST(number floating, "{{3.3}}",   "3.3")
DJANGO_TEST(number floating, "{{3.30}}",  "3.3")
DJANGO_TEST(number floating, "{{03.3}}",  "3.3")
DJANGO_TEST(number floating, "{{03.30}}", "3.3")

DJANGO_TEST(string, "{{'Foo'}}",   "Foo")
DJANGO_TEST(string, "{{\"Bar\"}}", "Bar")


/// Tags
///     TODO:
///     django::block_tag
///     django::debug_tag
///     django::extends_tag
///     django::ifchanged_tag
///     django::load_tag
///     django::load_from_tag
///     django::now_tag
///     django::ssi_tag
///     django::widthratio_tag
///     django::library_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

unit_test(missing tag) {
    string_template const t("{% xyz 42 %}");
    ensure_throws(s::missing_tag, t.render_to_string(context));
}}}

DJANGO_TEST(autoescape_tag, "{% autoescape on %}{{ xml_var }}{% endautoescape %}", "&lt;foo&gt;&lt;bar&gt;&lt;qux /&gt;&lt;/bar&gt;&lt;/foo&gt;")
DJANGO_TEST(autoescape_tag, "{% autoescape off %}{{ xml_var }}{% endautoescape %}", "<foo><bar><qux /></bar></foo>")
// TODO: DJANGO_TEST(autoescape_tag, "{% autoescape off %}{{ xml_var | escape }}{% endautoescape %}", "???")
// TODO: DJANGO_TEST(autoescape_tag, "{% autoescape off %}{{ xml_var | force_escape }}{% endautoescape %}", "???")
DJANGO_TEST(autoescape_tag, "{% autoescape on %}{{ xml_var | safe }}{% endautoescape %}", "<foo><bar><qux /></bar></foo>")

DJANGO_TEST(comment_tag-short, "0{# Foo Bar Qux #}1",                                "01")
DJANGO_TEST(comment_tag-short, "0{##}1",                                             "01")
DJANGO_TEST(comment_tag-short, "0{# {# #}1",                                         "01")
DJANGO_TEST(comment_tag-short, "0{# {{ x | y:'z' }} #}1",                            "01")
DJANGO_TEST(comment_tag-long,  "0{% comment %} Foo\n Bar\n Qux\n {% endcomment %}1", "01")

DJANGO_TEST_(csrf_token_tag,  "{% csrf_token %}", "", NO_CONTEXT)
DJANGO_TEST_(csrf_token_tag,  "{% csrf_token %}", "<div style='display:none'><input type='hidden' name='csrfmiddlewaretoken' value='ABCDEF123456' /></div>", context)

DJANGO_TEST(if_tag, "{% if True %}Good{% endif %}{% if False %}Bad{% endif %}", "Good")
DJANGO_TEST(if_tag, "{% if True %}Good{% else %}Bad{% endif %}",                "Good")
DJANGO_TEST(if_tag, "{% if False %}Bad{% else %}Good{% endif %}",               "Good")
DJANGO_TEST(if_tag, "{% if 1 %}Good{% endif %}{% if False %}Bad{% endif %}",    "Good")
DJANGO_TEST(if_tag, "{% if 1 %}Good{% else %}Bad{% endif %}",                   "Good")
DJANGO_TEST(if_tag, "{% if 0 %}Bad{% else %}Good{% endif %}",                   "Good")

DJANGO_TEST(ifequal_tag, "{% ifequal 6 6 %}Good{% endifequal %}",              "Good")
DJANGO_TEST(ifequal_tag, "{% ifequal 5 6 %}Good{% endifequal %}",              "")
DJANGO_TEST(ifequal_tag, "{% ifequal 6 6 %}Good{% else %}Bad{% endifequal %}", "Good")
DJANGO_TEST(ifequal_tag, "{% ifequal 5 6 %}Bad{% else %}Good{% endifequal %}", "Good")

DJANGO_TEST(include_tag, "{% include 'tests/templates/django/empty.tpl' %}", "")
DJANGO_TEST(include_tag, "{% include 'tests/templates/django/variables.tpl' %}", "foo: A\nbar: B\nqux: C\n")

DJANGO_TEST(filter_tag, "{% filter escape %}<foo />{% endfilter %}", "<foo />")
DJANGO_TEST(filter_tag, "{% filter force_escape %}<foo />{% endfilter %}", "&lt;foo /&gt;")
DJANGO_TEST(filter_tag, "{% filter title | lower %}aBcD{% endfilter %}", "abcd")
DJANGO_TEST(filter_tag, "{% filter upper | lower | title %}aBcD{% endfilter %}", "Abcd")
DJANGO_TEST(filter_tag,
        "{% filter upper %}\n"
        "    <p>\n"
        "        <a href=\"foo/\">Foo</a>\n"
        "    </p>\n"
        "{% endfilter %}\n",
            "\n"
            "    <P>\n"
            "        <A HREF=\"FOO/\">FOO</A>\n"
            "    </P>\n"
            "\n")

DJANGO_TEST(for_tag-value,
    "{% for v in friends %}[{{ v }}]{% endfor %}",
    "[age: 23, name: joe][age: 55, name: bob][age: 41, name: lou]")

DJANGO_TEST(for_empty_tag-value,
    "{% for v in friends %}[{{ v }}]{% empty %}Bad{% endfor %}",
    "[age: 23, name: joe][age: 55, name: bob][age: 41, name: lou]")

DJANGO_TEST(for_empty_tag-none,
    "{% for v in '' %}Bad{% empty %} It's empty, Jim {% endfor %}",
    " It's empty, Jim ")

/*
DJANGO_TEST(for_tag-key-value,
    "{% for k, v in friends %}[{{ k }}| {{ v }}]{% endfor %}",
    "[0| age: 23, name: joe][1| age: 55, name: bob][2| age: 41, name: lou]")
*/

DJANGO_TEST(regroup_tag,
    "{% regroup cities by country as country_list %}\n"
    "\n"
    "<ul>\n"
    "{% for country in country_list %}\n"
    "    <li>{{ country.grouper }}\n"
    "    <ul>\n"
    "        {% for item in country.list %}\n"
    "          <li>{{ item.name }}: {{ item.population }}</li>\n"
    "        {% endfor %}\n"
    "    </ul>\n"
    "    </li>\n"
    "{% endfor %}\n"
    "</ul>\n",
        "\n"
        "\n"
        "<ul>\n"
        "\n"
        "    <li>India\n"
        "    <ul>\n"
        "        \n"
        "          <li>Mumbai: 19,000,000</li>\n"
        "        \n"
        "          <li>Calcutta: 15,000,000</li>\n"
        "        \n"
        "    </ul>\n"
        "    </li>\n"
        "\n"
        "    <li>USA\n"
        "    <ul>\n"
        "        \n"
        "          <li>New York: 20,000,000</li>\n"
        "        \n"
        "          <li>Chicago: 7,000,000</li>\n"
        "        \n"
        "    </ul>\n"
        "    </li>\n"
        "\n"
        "    <li>Japan\n"
        "    <ul>\n"
        "        \n"
        "          <li>Tokyo: 33,000,000</li>\n"
        "        \n"
        "    </ul>\n"
        "    </li>\n"
        "\n"
        "</ul>\n")

DJANGO_TEST(spaceless_tag,
        "{% spaceless %}\n"
        "    <p>\n"
        "        <a href=\"foo/\">Foo</a>\n"
        "    </p>\n"
        "{% endspaceless %}\n",
            "\n"
            "    <p><a href=\"foo/\">Foo</a></p>\n"
            "\n")

DJANGO_TEST(spaceless_tag,
        "{% spaceless %}\n"
        "    <strong>\n"
        "        Hello\n"
        "    </strong>\n"
        "{% endspaceless %}\n",
            "\n"
            "    <strong>\n"
            "        Hello\n"
            "    </strong>\n"
            "\n")

DJANGO_TEST(templatetag_tag, "{% templatetag openbrace %}",     "{")
DJANGO_TEST(templatetag_tag, "{% templatetag closevariable %}", "}}")

unit_test(url_tag) {
    string_template const t("{% url 'foo.bar.qux' 1 2 3 %}");
    null_resolver<options_type>::patterns_type patterns;
    patterns["foo.bar.qux"] = "/foo-bar-qux";
    options.resolvers.push_back(options_type::resolver_type(new null_resolver<options_type>(patterns)));

    ensure_equals(t.render_to_string(context, options), "/foo-bar-qux/1/2/3");
}}}

unit_test(url_tag) {
    string_template const t("{% url 'x.y.z' 1 2 3 %}");
    null_resolver<options_type>::patterns_type patterns;
    options.resolvers.push_back(options_type::resolver_type(new null_resolver<options_type>(patterns)));

    ensure_throws(std::runtime_error, t.render_to_string(context, options));
}}}

unit_test(url_as_tag) {
    string_template const t("{% url 'foo.bar.qux' 1 2 3 as foo %}_{{ foo }}");
    null_resolver<options_type>::patterns_type patterns;
    patterns["foo.bar.qux"] = "/foo-bar-qux";
    options.resolvers.push_back(options_type::resolver_type(new null_resolver<options_type>(patterns)));

    ensure_equals(t.render_to_string(context, options), "_/foo-bar-qux/1/2/3");
}}}

unit_test(url_as_tag) {
    string_template const t("{% url 'x.y.z' 1 2 3 as foo %}_{{ x }}");
    null_resolver<options_type>::patterns_type patterns;
    options.resolvers.push_back(options_type::resolver_type(new null_resolver<options_type>(patterns)));

    ensure_equals(t.render_to_string(context, options), "_");
}}}

DJANGO_TEST_(variable_tag, "{{ foo }} {{ bar }} {{ qux }}", "  ",    NO_CONTEXT)
DJANGO_TEST_(variable_tag, "{{ foo }} {{ bar }} {{ qux }}", "A B C", context)

DJANGO_TEST(verbatim_tag,
        "{% verbatim %}{% for v in friends %}\n"
        "    <p>{{ v }}</p>\n"
        "{% endfor %}{% endverbatim %}\n",
            "{% for v in friends %}\n"
            "    <p>{{ v }}</p>\n"
            "{% endfor %}\n")

DJANGO_TEST(with_tag, "[{{ls}}] {% with 'this is a long string' as ls %} {{ls}} {% endwith %} [{{ls}}]", "[]  this is a long string  []")

/// Filters
///     TODO:
///     django::force_escape_filter
///     django::iriencode_filter
///     django::pprint_filter
////////////////////////////////////////////////////////////////////////////////////////////////////

unit_test(missing-filter) {
    string_template const t("{{ 42 | xyz }}");
    ensure_throws(s::missing_filter, t.render_to_string(context));
}}}

DJANGO_TEST(add_filter, "{{ '5'|add:6 }}", "11")
DJANGO_TEST(add_filter, "{{ 3|add:'8' }}", "11")

DJANGO_TEST(addslashes_filter, "{{ \"String with 'quotes'.\" |addslashes }}", "String with \\'quotes\\'.")

DJANGO_TEST(capfirst_filter, "{{ 'foo fa fa'|capfirst }}", "Foo fa fa")

DJANGO_TEST(center_filter, "{{ \"Django\" | center:\"15\" }}", "     Django    ")
DJANGO_TEST(center_filter, "{{ \"Django\" | center:\"16\" }}", "     Django     ")
DJANGO_TEST(center_filter, "{{ \"Django\" | center:\"2\" }}", "Django")

DJANGO_TEST(cut_filter, "{{ 'String with spaces' | cut:' ' }}", "Stringwithspaces")

DJANGO_TEST(date_filter, "{{ past        | date }}",                     "Jan 10, 2002")
DJANGO_TEST(date_filter, "{{ before_past | date:'r' }}",                 "Tue, 08 Jan 2002 13:02:03")
DJANGO_TEST(date_filter, "{{ after_past  | date:'SHORT_DATE_FORMAT' }}", "03/01/2002")

DJANGO_TEST(default_filter, "{{ True  |default:\"default\" }}", "True")
DJANGO_TEST(default_filter, "{{ False |default:\"default\" }}", "default")
DJANGO_TEST(default_filter, "{{ None  |default:\"default\" }}", "default")

DJANGO_TEST(default_if_none_filter, "{{ True  |default_if_none:\"default\" }}", "True")
DJANGO_TEST(default_if_none_filter, "{{ False |default_if_none:\"default\" }}", "False")
DJANGO_TEST(default_if_none_filter, "{{ None  |default_if_none:\"default\" }}", "default")

DJANGO_TEST(dictsort_filter, "{{ friends }}",                   "age: 23, name: joe, age: 55, name: bob, age: 41, name: lou")
DJANGO_TEST(dictsort_filter, "{{ friends | dictsort:'name' }}", "age: 55, name: bob, age: 23, name: joe, age: 41, name: lou")

DJANGO_TEST(dictsortreversed_filter, "{{ friends }}",                           "age: 23, name: joe, age: 55, name: bob, age: 41, name: lou")
DJANGO_TEST(dictsortreversed_filter, "{{ friends | dictsortreversed:'name' }}", "age: 41, name: lou, age: 23, name: joe, age: 55, name: bob")

DJANGO_TEST(divisibleby_filter, "{{ 21 | divisibleby:\"3\" }}", "True")
DJANGO_TEST(divisibleby_filter, "{{ 20 | divisibleby:\"3\" }}", "False")

DJANGO_TEST(filesizeformat_filter, "{{ 123456789|filesizeformat }}", "117.7 MB")

DJANGO_TEST(firstof_filter, "{% firstof true_var %}", "True")
DJANGO_TEST(firstof_filter, "{% firstof true_var 'FALLBACK' %}", "True")
DJANGO_TEST(firstof_filter, "{% firstof true_var false_var 'FALLBACK' %}", "True")
DJANGO_TEST(firstof_filter, "{% firstof false_var true_var 'FALLBACK' %}", "True")
DJANGO_TEST(firstof_filter, "{% firstof false_var %}", "")
DJANGO_TEST(firstof_filter, "{% firstof false_var 'FALLBACK' %}", "FALLBACK")
DJANGO_TEST(firstof_filter, "{% firstof nonextant false_var numbers cities %}", "1, 2, 3, 4, 5, 6, 7, 8, 9")
DJANGO_TEST(firstof_filter, "{% firstof nonextant %}", "")

DJANGO_TEST(fix_ampersands_filter, "{{ 'String & with & ampersands, but not &apos; or &#1234;' |fix_ampersands }}",
                "String &amp; with &amp; ampersands, but not &apos; or &#1234;")

DJANGO_TEST(floatformat_filter, "{{34.23234|floatformat }}", "34.2")
DJANGO_TEST(floatformat_filter, "{{34.00000|floatformat }}", "34")
DJANGO_TEST(floatformat_filter, "{{34.26000|floatformat }}", "34.3")

DJANGO_TEST(floatformat_filter, "{{34.23234|floatformat:3 }}", "34.232")
DJANGO_TEST(floatformat_filter, "{{34.00000|floatformat:3 }}", "34.000")
DJANGO_TEST(floatformat_filter, "{{34.26000|floatformat:3 }}", "34.260")

DJANGO_TEST(floatformat_filter, "{{34.23234|floatformat:\"-3\" }}", "34.232")
DJANGO_TEST(floatformat_filter, "{{34.00000|floatformat:\"-3\" }}", "34")
DJANGO_TEST(floatformat_filter, "{{34.26000|floatformat:\"-3\" }}", "34.260")

DJANGO_TEST(getdigit_filter, "{{ 123456789|get_digit:'2' }}",  "8")
DJANGO_TEST(getdigit_filter, "{{ -123456789|get_digit:'2' }}", "-123456789")
DJANGO_TEST(getdigit_filter, "{{ 'foobar'|get_digit:'2' }}",   "foobar")

DJANGO_TEST(ljust_filter, "{{ \"Django\" | ljust:\"10\" }}", "Django    ")
DJANGO_TEST(ljust_filter, "{{ \"Django\" | ljust:\"2\" }}", "Django")

DJANGO_TEST(lower_filter, "{{ \"Still MAD At Yoko\" | lower }}", "still mad at yoko")

DJANGO_TEST(make_list_filter, "{{ numbers|make_list}}", "[1, 2, 3, 4, 5, 6, 7, 8, 9]")
DJANGO_TEST(make_list_filter, "{{ 12345|make_list }}", "[1, 2, 3, 4, 5]")

DJANGO_TEST(phone2numeric_filter, "{{ \"1-800-COLLECT\" | phone2numeric }}", "1-800-2655328")

DJANGO_TEST(pluralize_filter, "ox{{ 0 | pluralize:'en' }}",       "oxen")
DJANGO_TEST(pluralize_filter, "ox{{ 1 | pluralize:'en' }}",       "ox")
DJANGO_TEST(pluralize_filter, "ox{{ 2 | pluralize:'en' }}",       "oxen")
DJANGO_TEST(pluralize_filter, "tank{{ 0 | pluralize }}",          "tanks")
DJANGO_TEST(pluralize_filter, "tank{{ 1 | pluralize }}",          "tank")
DJANGO_TEST(pluralize_filter, "tank{{ 2 | pluralize }}",          "tanks")
DJANGO_TEST(pluralize_filter, "cris{{ 0 | pluralize:'is,es' }}",  "crises")
DJANGO_TEST(pluralize_filter, "cris{{ 1 | pluralize:'is,es' }}",  "crisis")
DJANGO_TEST(pluralize_filter, "cris{{ 2 | pluralize:'is,es' }}",  "crises")
DJANGO_TEST(pluralize_filter, "ferr{{ 0 | pluralize:'y,ies' }}",  "ferries")
DJANGO_TEST(pluralize_filter, "ferr{{ 1 | pluralize:'y,ies' }}",  "ferry")
DJANGO_TEST(pluralize_filter, "ferr{{ 2 | pluralize:'y,ies' }}",  "ferries")

DJANGO_TEST(removetags_filter, "{{ \"<b>Joel</b> <button>is</button> a <span>slug</span>\" | removetags:\"b span\"|safe }}", "Joel <button>is</button> a slug")
DJANGO_TEST(removetags_filter, "{{ \"<b>Begin</b> <foo /> <foo/> </foo> <foo> <span attr='value'>End</span>\" | removetags:\"b span foo\"|safe }}", "Begin     End")

DJANGO_TEST(rjust_filter, "{{ \"Django\" | rjust:\"10\" }}", "    Django")
DJANGO_TEST(rjust_filter, "{{ \"Django\" | rjust:\"2\" }}", "Django")

DJANGO_TEST(slugify_filter, "{{ ' Joel is a slug '|slugify }}", "joel-is-a-slug")
DJANGO_TEST(slugify_filter, "{{ '\tJoel\v is\n a\r slug\x01'|slugify }}", "joel-is-a-slug")

DJANGO_TEST(striptags_filter, "{{ '<b>Joel</b> <button>is</button> a <span>slug</span>'|striptags }}", "Joel is a slug")

DJANGO_TEST(stringformat_filter, "{{ 255|stringformat:'x' }}", "ff")

DJANGO_TEST(time_filter, "{{ past        | time }}",                     "01:02:03 AM")
DJANGO_TEST(time_filter, "{{ before_past | time:'c' }}",                 "2002-01-08T13:02:03")
DJANGO_TEST(time_filter, "{{ after_past  | time:'YEAR_MONTH_FORMAT' }}", "March 2002")

DJANGO_TEST(timesince_filter, "{{ past | timesince:before_past }}",       "0&nbsp;minutes")
DJANGO_TEST(timesince_filter, "{{ before_past | timesince:past }}",       "1&nbsp;day, 12&nbsp;hours")
DJANGO_TEST(timesince_filter, "{{ before_past | timesince:after_past }}", "1&nbsp;month, 3&nbsp;weeks")
DJANGO_TEST(timesince_filter, "{{ past | timesince:after_past }}",        "1&nbsp;month, 2&nbsp;weeks")
DJANGO_TEST(timesince_filter, "{{ future | timesince }}",                 "0&nbsp;minutes")

DJANGO_TEST(timeuntil_filter, "{{ past | timeuntil:before_past }}",       "1&nbsp;day, 12&nbsp;hours")
DJANGO_TEST(timeuntil_filter, "{{ after_past | timeuntil:before_past }}", "1&nbsp;month, 3&nbsp;weeks")
DJANGO_TEST(timeuntil_filter, "{{ after_past | timeuntil:past }}",        "1&nbsp;month, 2&nbsp;weeks")
DJANGO_TEST(timeuntil_filter, "{{ before_past | timeuntil:past }}",       "0&nbsp;minutes")
DJANGO_TEST(timeuntil_filter, "{{ past | timeuntil }}",                   "0&nbsp;minutes")

DJANGO_TEST(title_filter, "{{ \"my FIRST post\" | title }}", "My First Post")
DJANGO_TEST(title_filter, "{{ 'joel is a slug' | title }}", "Joel Is A Slug")

DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:0 }}",  "")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:1 }}",  "...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:2 }}",  "...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:3 }}",  "...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:4 }}",  "J...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:5 }}",  "Jo...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:6 }}",  "Joe...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:7 }}",  "Joel...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:8 }}",  "Joel ...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:9 }}",  "Joel i...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:10 }}", "Joel is...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:11 }}", "Joel is ...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:12 }}", "Joel is a...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:13 }}", "Joel is a ...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:14 }}", "Joel is a slug")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\" | truncatechars:15 }}", "Joel is a slug")

DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatechars_html:0 }}",  "")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatechars_html:1 }}",  "<p>...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatechars_html:8 }}",  "<p>Joel ...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatechars_html:9 }}",  "<p>Joel i...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatechars_html:10 }}", "<p>Joel is...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatechars_html:14 }}", "<p>Joel is a slug</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatechars_html:15 }}", "<p>Joel is a slug</p>")

DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\" | truncatechars_html:0 }}",  "")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\" | truncatechars_html:1 }}",  "<p>...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\" | truncatechars_html:8 }}",  "<p>Joel ...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\" | truncatechars_html:9 }}",  "<p>Joel i...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\" | truncatechars_html:10 }}", "<p>Joel is...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\" | truncatechars_html:14 }}", "<p>Joel is a slug</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\" | truncatechars_html:15 }}", "<p>Joel is a slug</p>")

// TODO: Complete once we know the right behavior:
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:0 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:1 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:2 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:3 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:4 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:5 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:6 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:7 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:8 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:9 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:10 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:11 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:12 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:13 }}", "")
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:14 }}", "Joel <a href='#'>is <i>a</i> slug</a>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatechars_html:15 }}", "Joel <a href='#'>is <i>a</i> slug</a>")

// TODO: Complete once we know the right behavior:
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:0 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:1 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:2 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:3 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:4 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:5 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:6 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:7 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:8 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:9 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:10 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:11 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:12 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:13 }}", "")
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:14 }}", "Joel <a href='#'>is <i>a</i> slug</a>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatechars_html:15 }}", "Joel <a href='#'>is <i>a</i> slug</a>")

DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\" | truncatewords:0 }}", " ...")
DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\" | truncatewords:1 }}", "Joel ...")
DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\" | truncatewords:2 }}", "Joel is ...")
DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\" | truncatewords:3 }}", "Joel is a ...")
DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\" | truncatewords:4 }}", "Joel is a slug")
DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\" | truncatewords:5 }}", "Joel is a slug")

DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \" | truncatewords:0 }}", " ...")
DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \" | truncatewords:1 }}", "Joel ...")
DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \" | truncatewords:2 }}", "Joel is ...")
DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \" | truncatewords:3 }}", "Joel is a ...")
DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \" | truncatewords:4 }}", "Joel is a slug")
DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \" | truncatewords:5 }}", "Joel is a slug")

DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatewords_html:1 }}", "<p>Joel ...</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatewords_html:2 }}", "<p>Joel is ...</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatewords_html:3 }}", "<p>Joel is a ...</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatewords_html:4 }}", "<p>Joel is a slug</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\" | truncatewords_html:5 }}", "<p>Joel is a slug</p>")

DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\" | truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\" | truncatewords_html:1 }}", "<p>Joel ...</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\" | truncatewords_html:2 }}", "<p>Joel is ...</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\" | truncatewords_html:3 }}", "<p>Joel is a ...</p>")
// FIXME: DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\" | truncatewords_html:4 }}", "<p>Joel is a slug")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\" | truncatewords_html:5 }}", "<p>Joel is a slug")

DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatewords_html:1 }}", "Joel ...")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatewords_html:2 }}", "Joel <a href='#'>is ...</a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatewords_html:3 }}", "Joel <a href='#'>is <i>a ...</i></a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatewords_html:4 }}", "Joel <a href='#'>is <i>a</i> slug</a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\" | truncatewords_html:5 }}", "Joel <a href='#'>is <i>a</i> slug</a>")

DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatewords_html:1 }}", "Joel ...")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatewords_html:2 }}", "Joel <a href='#'>is ...</a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatewords_html:3 }}", "Joel <a href='#'>is <i>a ...</i></a>")
// FIXME: DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatewords_html:4 }}", "Joel <a href='#'>is <i>a</i> slug")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\" | truncatewords_html:5 }}", "Joel <a href='#'>is <i>a</i> slug")

DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \" | truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \" | truncatewords_html:1 }}", "  Joel ...")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \" | truncatewords_html:2 }}", "  Joel  <a href='#'>  is ...</a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \" | truncatewords_html:3 }}", "  Joel  <a href='#'>  is  <i>  a ...</i></a>")
// FIXME: DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \" | truncatewords_html:4 }}", "  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  ")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \" | truncatewords_html:5 }}", "  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  ")

DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \" | truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \" | truncatewords_html:1 }}", "  Joel ...")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \" | truncatewords_html:2 }}", "  Joel  <a href='#'>  is ...</a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \" | truncatewords_html:3 }}", "  Joel  <a href='#'>  is  <i>  a ...</i></a>")
// FIXME: DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \" | truncatewords_html:4 }}", "  Joel  <a href='#'>  is  <i>  a  </i>  slug  ")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \" | truncatewords_html:5 }}", "  Joel  <a href='#'>  is  <i>  a  </i>  slug  ")

DJANGO_TEST(unordered_list_filter, "{{ 'abc'|unordered_list }}", "<li>abc</li>\n")
DJANGO_TEST(unordered_list_filter, "{{ places|unordered_list }}",
                "<li>Parent\n"
                "<ul>\n"
                "\t<li>States\n"
                "\t<ul>\n"
                "\t\t<li>Kansas\n"
                "\t\t<ul>\n"
                "\t\t\t<li>Lawrence</li>\n"
                "\t\t\t<li>Topeka</li>\n"
                "\t\t</ul>\n"
                "\t\t</li>\n"
                "\t\t<li>Illinois1</li>\n"
                "\t\t<li>Illinois2</li>\n"
                "\t</ul>\n"
                "\t</li>\n"
                "</ul>\n"
                "</li>\n")

DJANGO_TEST(upper_filter, "{{ \"Joel is a slug\" | upper }}", "JOEL IS A SLUG")

DJANGO_TEST(urlencode_filter, "{{ \"/this should/be encoded ^ because @ is not an option $ ()\" | urlencode }}", "/this%20should/be%20encoded%20%5E%20because%20%40%20is%20not%20an%20option%20%24%20%28%29")

DJANGO_TEST(urlize_filter, "{{ \"This is some text containing a http://www.url.com sir and also another.url.com.\" | urlize }}", "This is some text containing a <a href='http://www.url.com'>http://www.url.com</a> sir and also <a href='http://another.url.com'>another.url.com</a>.")

DJANGO_TEST(urlizetrunc_filter, "{{ \"This is some text containing a http://www.url.com sir and also another.url.com.\" | urlizetrunc:15 }}", "This is some text containing a <a href='http://www.url.com'>http://www.url....</a> sir and also <a href='http://another.url.com'>another.url.com</a>.")

DJANGO_TEST(wordcount_filter, "{{ 'joel is a slug' | wordcount }}", "4")

DJANGO_TEST(wordwrap_filter, "{{ 'Joel is a slug'|wordwrap:5 }}", "\nJoel\nis a\nslug")

DJANGO_TEST(yesno_filter, "{{ true_var|yesno:'Yes,No' }}",        "Yes")
DJANGO_TEST(yesno_filter, "{{ false_var|yesno:'Yes,No' }}",       "No")
DJANGO_TEST(yesno_filter, "{{ True  |yesno:\"yeah,no,maybe\" }}", "yeah")
DJANGO_TEST(yesno_filter, "{{ False |yesno:\"yeah,no,maybe\" }}", "no")
DJANGO_TEST(yesno_filter, "{{ None  |yesno:\"yeah,no,maybe\" }}", "maybe")
DJANGO_TEST(yesno_filter, "{{ None  |yesno:\"yeah,no\" }}",       "no")

DJANGO_TEST(escape_filter, "{{xml_var|escape}}", "&lt;foo&gt;&lt;bar&gt;&lt;qux /&gt;&lt;/bar&gt;&lt;/foo&gt;")

DJANGO_TEST(first_filter, "{{ 'abcde'|first }}", "a")
DJANGO_TEST(last_filter, "{{ 'abcde'|last }}", "e")
DJANGO_TEST(length_filter, "{{ 'abcde'|length }}", "5")
DJANGO_TEST(length_is_filter, "{{ 'abcde'|length_is:'4' }}", "False")
DJANGO_TEST(length_is_filter, "{{ 'abcde'|length_is:'5' }}", "True")
DJANGO_TEST(length_is_filter, "{{ 'abcde'|length_is:'6' }}", "False")

// TODO: Better test for random_filter.
unit_test(random_filter) {
    string_template const t("{{ 'abcde'|random }}");
    string_type s = t.render_to_string(context);
    ensure(s == "a" || s == "b" || s == "c" || s == "d" || s == "e");
}}}

DJANGO_TEST(join_filter, "{{ 'abcde'|join:'_' }}", "a_b_c_d_e")

DJANGO_TEST(slice_filter, "{{ numbers|slice:'0:9'}}", "1, 2, 3, 4, 5, 6, 7, 8, 9")
DJANGO_TEST(slice_filter, "{{ numbers|slice:':9'}}", "1, 2, 3, 4, 5, 6, 7, 8, 9")
DJANGO_TEST(slice_filter, "{{ numbers|slice:':'}}", "1, 2, 3, 4, 5, 6, 7, 8, 9")
DJANGO_TEST(slice_filter, "{{ numbers|slice:'0:'}}", "1, 2, 3, 4, 5, 6, 7, 8, 9")
DJANGO_TEST(slice_filter, "{{ numbers|slice:'2:6'}}", "3, 4, 5, 6")
DJANGO_TEST(slice_filter, "{{ numbers|slice:'-6:-2'}}", "4, 5, 6, 7")

/* TODO:
DJANGO_TEST(linenumbers_filter, "{{ lines_of_text|linenumbers}}", "")
DJANGO_TEST(linebreaksbr_filter, "{{ lines_of_text|linebreaksbr }}", "")
DJANGO_TEST(linebreaks_filter, "{{ lines_of_text|linebreaks }}", "")
DJANGO_TEST(escapejs_filter, "{{ binary_string|escapejs }}", "")

DJANGO_TEST(join_filter, "{{tags|join:', '}}", "")
DJANGO_TEST(safe_filter, "{{tags|safe}}", "")
DJANGO_TEST(safe_filter+join_filter, "{{tags|safe|join:', '}}", "")
DJANGO_TEST(safeseq_filter, "{{tags|safeseq}}", "")
DJANGO_TEST(safeseq_filter+join_filter, "{{tags|safeseq|join:', '}}", "")
*/

/*
TODO:
{% for k, v in numbers %}{% cycle k v as x %}({{x}}){% endfor %}

<img src="bar.gif" height="10" width="{% widthratio this_value max_value 100 %}" />

{#% ssi /etc/adjtime parsed % -- normally unavailable on Windows and OS X #}
{#% ssi /etc/adjtime % -- normally unavailable on Windows and OS X #}

{% block a_block %}This is a block{% endblock a_block %}
*/
