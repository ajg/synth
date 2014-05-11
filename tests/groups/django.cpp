//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <ctime>
#include <string>

#include <ajg/synth/testing.hpp>
#include <ajg/synth/templates.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/detail/filesystem.hpp>
#include <ajg/synth/detail/text.hpp>
#include <ajg/synth/engines/django.hpp>
#include <ajg/synth/engines/null/resolver.hpp>

#include <tests/data/kitchen_sink.hpp>

namespace {

namespace s = ajg::synth;

using boost::optional;
using s::detail::get_current_working_directory;

typedef s::default_traits<char>                                                 traits_type;
typedef s::engines::django::engine<traits_type>                                 engine_type;

typedef s::templates::path_template<engine_type>                                path_template_type;
typedef s::templates::string_template<engine_type>                              string_template_type;

typedef engine_type::traits_type                                                traits_type;
typedef engine_type::context_type                                               context_type;
typedef engine_type::options_type                                               options_type;
typedef engine_type::value_type                                                 value_type;

typedef traits_type::char_type                                                  char_type;
typedef traits_type::string_type                                                string_type;

typedef value_type::behavior_type                                               behavior_type;
typedef s::engines::null::resolver<options_type>                                null_resolver_type;
typedef s::detail::text<string_type>                                            text;

struct data_type : tests::data::kitchen_sink<engine_type> {};

AJG_SYNTH_TEST_GROUP_WITH_DATA("django", data_type);

static string_type const absolute_path = text::widen(get_current_working_directory());

} // namespace

///     TODO:
///     django::load_tag      (Tested implicitly in Python binding tests.)
///     django::load_from_tag (Tested implicitly in Python binding tests.)
///     django::library_tag   (Tested implicitly in Python binding tests.)

#define DJANGO_TEST_(name, in, out, context) \
    unit_test(name) { ensure_equals(string_template_type(in).render_to_string(context), out); }}}

#define DJANGO_TEST(name, in, out) DJANGO_TEST_(name, in, out, context)

#define NO_CONTEXT AJG_SYNTH_EMPTY

///
/// Sanity checks
////////////////////////////////////////////////////////////////////////////////////////////////////

DJANGO_TEST_(empty, "", "", NO_CONTEXT)
DJANGO_TEST_(empty, "", "", context)

DJANGO_TEST_(text, "ABC", "ABC", NO_CONTEXT)
DJANGO_TEST_(text, "ABC", "ABC", context)

DJANGO_TEST_(html, "<foo>\nA foo <bar /> element.\n</foo>", "<foo>\nA foo <bar /> element.\n</foo>", NO_CONTEXT)
DJANGO_TEST_(html, "<foo>\nA foo <bar /> element.\n</foo>", "<foo>\nA foo <bar /> element.\n</foo>", context)

DJANGO_TEST_(html, "{$ foo bar baz $}\n{ { { { {", "{$ foo bar baz $}\n{ { { { {", NO_CONTEXT)
DJANGO_TEST_(html, "{$ foo bar baz $}\n{ { { { {", "{$ foo bar baz $}\n{ { { { {", context)

///
/// Literal tests
////////////////////////////////////////////////////////////////////////////////////////////////////

DJANGO_TEST(none, "{{None}}", "None")

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

/// Value tests
///     NOTE: The use of firstof is incidental; many other tags would work for these tests.
////////////////////////////////////////////////////////////////////////////////////////////////////

DJANGO_TEST(multiple filters,   "{% firstof 0|add:1|add:2|add:3 %}", "6")
DJANGO_TEST(multiple pipelines, "{% firstof -1|add:1 2|add:-2 3 %}", "3")

///
/// Escaping tests
////////////////////////////////////////////////////////////////////////////////////////////////////

DJANGO_TEST(autoescape_tag, "{% autoescape on %}{{ xml_var }}{% endautoescape %}",               "&lt;foo&gt;&lt;bar&gt;&lt;qux /&gt;&lt;/bar&gt;&lt;/foo&gt;")
DJANGO_TEST(autoescape_tag, "{% autoescape off %}{{ xml_var }}{% endautoescape %}",              "<foo><bar><qux /></bar></foo>")
DJANGO_TEST(autoescape_tag, "{% autoescape on %}{{ xml_var|escape }}{% endautoescape %}",        "&lt;foo&gt;&lt;bar&gt;&lt;qux /&gt;&lt;/bar&gt;&lt;/foo&gt;")
DJANGO_TEST(autoescape_tag, "{% autoescape off %}{{ xml_var|escape }}{% endautoescape %}",       "<foo><bar><qux /></bar></foo>")
DJANGO_TEST(autoescape_tag, "{% autoescape on %}{{ xml_var|force_escape }}{% endautoescape %}",  "&lt;foo&gt;&lt;bar&gt;&lt;qux /&gt;&lt;/bar&gt;&lt;/foo&gt;")
DJANGO_TEST(autoescape_tag, "{% autoescape off %}{{ xml_var|force_escape }}{% endautoescape %}", "&lt;foo&gt;&lt;bar&gt;&lt;qux /&gt;&lt;/bar&gt;&lt;/foo&gt;")
DJANGO_TEST(autoescape_tag, "{% autoescape on %}{{ xml_var|safe }}{% endautoescape %}",          "<foo><bar><qux /></bar></foo>")
DJANGO_TEST(autoescape_tag, "{% autoescape off %}{{ xml_var|safe }}{% endautoescape %}",         "<foo><bar><qux /></bar></foo>")

DJANGO_TEST(autoescape_tag, "{{ xml_var }}",              "&lt;foo&gt;&lt;bar&gt;&lt;qux /&gt;&lt;/bar&gt;&lt;/foo&gt;")
DJANGO_TEST(autoescape_tag, "{{ xml_var|escape }}",       "&lt;foo&gt;&lt;bar&gt;&lt;qux /&gt;&lt;/bar&gt;&lt;/foo&gt;")
DJANGO_TEST(autoescape_tag, "{{ xml_var|force_escape }}", "&lt;foo&gt;&lt;bar&gt;&lt;qux /&gt;&lt;/bar&gt;&lt;/foo&gt;")
DJANGO_TEST(autoescape_tag, "{{ xml_var|safe }}",         "<foo><bar><qux /></bar></foo>")

DJANGO_TEST(no-filter,                  "{{tags}}",                   "&lt;X&gt;, &lt;Y&gt;, &lt;Z&gt;")
DJANGO_TEST(join_filter,                "{{tags|join:'-'}}",          "&lt;X&gt;-&lt;Y&gt;-&lt;Z&gt;")
DJANGO_TEST(safe_filter,                "{{tags|safe}}",              "<X>, <Y>, <Z>")
DJANGO_TEST(safe_filter+join_filter,    "{{tags|safe|join:'-'}}",     "<-X->-,- -<-Y->-,- -<-Z->")
DJANGO_TEST(safeseq_filter,             "{{tags|safeseq}}",           "<X>, <Y>, <Z>")
DJANGO_TEST(safeseq_filter+join_filter, "{{tags|safeseq|join:'-'}}",  "<X>-<Y>-<Z>")

///
/// Inheritance tests
////////////////////////////////////////////////////////////////////////////////////////////////////

DJANGO_TEST(inheritance, "{% include 'tests/templates/django/base.tpl' %}",
    "Base template\nBase header\nBase content\nBase footer\n")

DJANGO_TEST(inheritance, "{% include 'tests/templates/django/derived.tpl' %}",
    "Base template\nBase header\nBase content + 1, 2, 3, 4, 5, 6, 7, 8, 9\nBase footer\n")

// TODO: Test multiple levels of inheritance.

///
/// Tag tests
////////////////////////////////////////////////////////////////////////////////////////////////////

unit_test(missing tag) {
    string_template_type const t("{% xyz 42 %}");
    ensure_throws(s::missing_tag, t.render_to_string(context));
}}}

DJANGO_TEST(block_tag, "foo|{% block a_block %}This is a block{% endblock %}|bar",         "foo|This is a block|bar")
DJANGO_TEST(block_tag, "foo|{% block a_block %}This is a block{% endblock a_block %}|bar", "foo|This is a block|bar")

unit_test(mismatched block) {
    string_template_type const t("{% block foo %}{% endblock bar %}");
    ensure_throws(std::invalid_argument, t.render_to_string(context));
}}}

DJANGO_TEST(comment_tag-short, "0{# Foo Bar Qux #}1",                                "01")
DJANGO_TEST(comment_tag-short, "0{##}1",                                             "01")
DJANGO_TEST(comment_tag-short, "0{# {# #}1",                                         "01")
DJANGO_TEST(comment_tag-short, "0{# {{ x|y:'z' }} #}1",                              "01")
DJANGO_TEST(comment_tag-short, "0{# {% if foo %}bar{% else %} #}1",                  "01")
DJANGO_TEST(comment_tag-long,  "0{% comment %} Foo\n Bar\n Qux\n {% endcomment %}1", "01")

DJANGO_TEST_(csrf_token_tag,  "{% csrf_token %}", "", NO_CONTEXT)
DJANGO_TEST_(csrf_token_tag,  "{% csrf_token %}", "<div style='display:none'><input type='hidden' name='csrfmiddlewaretoken' value='ABCDEF123456' /></div>", context)

DJANGO_TEST(cycle_tag, "{% for n in numbers %}{% cycle 'a' 'b' %}{% endfor %}",                  "ababababa")
DJANGO_TEST(cycle_tag, "{% for n in numbers %}{% cycle 'a' 'b' as x %}{{x}}{% endfor %}",        "aabbaabbaabbaabbaa")
DJANGO_TEST(cycle_tag, "{% for n in numbers %}{% cycle 'a' 'b' as x silent %}{{x}}{% endfor %}", "ababababa")
DJANGO_TEST(cycle_tag, "{% for n in numbers %}{% cycle 'a' 'b' as x silent %}{% endfor %}",      "")

DJANGO_TEST(cycle_tag, "{% for k, v in states %}({% cycle k v %};) {% endfor %}",                   "(CA;) (Florida;) (NY;) ")
DJANGO_TEST(cycle_tag, "{% for k, v in states %}({% cycle k v as x %}; {{x}}) {% endfor %}",        "(CA; CA) (Florida; Florida) (NY; NY) ")
DJANGO_TEST(cycle_tag, "{% for k, v in states %}({% cycle k v as x silent %}; {{x}}) {% endfor %}", "(; CA) (; Florida) (; NY) ")
DJANGO_TEST(cycle_tag, "{% for k, v in states %}({% cycle k v as x silent %};) {% endfor %}",       "(;) (;) (;) ")

DJANGO_TEST(debug_tag, "{% debug %}",
    "<h1>Context:</h1>\n"
    "    after_past = 2002-Mar-01 01:22:03<br />\n"
    "    bar = B<br />\n"
    "    before_past = 2002-Jan-08 13:02:03<br />\n"
    "    cities = country: India, name: Mumbai, population: 19,000,000, country: India, name: Calcutta, population: 15,000,000, country: USA, name: New York, population: 20,000,000, country: USA, name: Chicago, population: 7,000,000, country: Japan, name: Tokyo, population: 33,000,000<br />\n"
    "    csrf_token = ABCDEF123456<br />\n"
    "    false_var = False<br />\n"
    "    foo = A<br />\n"
    "    friends = age: 23, name: joe, age: 55, name: bob, age: 41, name: lou<br />\n"
    "    future = 2202-Feb-11 03:02:01<br />\n"
    "    haiku = Haikus are easy,\nBut sometimes they don&apos;t make sense.\nRefrigerator.\n<br />\n"
    "    heterogenous = 42, 42, foo, foo<br />\n"
    "    numbers = 1, 2, 3, 4, 5, 6, 7, 8, 9<br />\n"
    "    past = 2002-Jan-10 01:02:03<br />\n"
    "    places = Parent, States, Kansas, Lawrence, Topeka, Illinois1, Illinois2<br />\n"
    "    qux = C<br />\n"
    "    states = CA: California, FL: Florida, NY: New York<br />\n"
    "    tags = &lt;X&gt;, &lt;Y&gt;, &lt;Z&gt;<br />\n"
    "    true_var = True<br />\n"
    "    xml_var = &lt;foo&gt;&lt;bar&gt;&lt;qux /&gt;&lt;/bar&gt;&lt;/foo&gt;<br />\n")

DJANGO_TEST(firstof_tag, "{% firstof true_var %}", "True")
DJANGO_TEST(firstof_tag, "{% firstof true_var 'FALLBACK' %}", "True")
DJANGO_TEST(firstof_tag, "{% firstof true_var false_var 'FALLBACK' %}", "True")
DJANGO_TEST(firstof_tag, "{% firstof false_var true_var 'FALLBACK' %}", "True")
DJANGO_TEST(firstof_tag, "{% firstof false_var %}", "")
DJANGO_TEST(firstof_tag, "{% firstof false_var 'FALLBACK' %}", "FALLBACK")
DJANGO_TEST(firstof_tag, "{% firstof nonextant false_var numbers cities %}", "1, 2, 3, 4, 5, 6, 7, 8, 9")
DJANGO_TEST(firstof_tag, "{% firstof nonextant %}", "")

DJANGO_TEST(if_tag, "{% if True %}Good{% endif %}{% if False %}Bad{% endif %}", "Good")
DJANGO_TEST(if_tag, "{% if True %}Good{% else %}Bad{% endif %}",                "Good")
DJANGO_TEST(if_tag, "{% if False %}Bad{% else %}Good{% endif %}",               "Good")
DJANGO_TEST(if_tag, "{% if 1 %}Good{% endif %}{% if False %}Bad{% endif %}",    "Good")
DJANGO_TEST(if_tag, "{% if 1 %}Good{% else %}Bad{% endif %}",                   "Good")
DJANGO_TEST(if_tag, "{% if 0 %}Bad{% else %}Good{% endif %}",                   "Good")

DJANGO_TEST(ifchanged_tag:content, "{% for v in heterogenous%}{% ifchanged %}{{ v }}{% endifchanged %}{% endfor %}",                  "42foo")
DJANGO_TEST(ifchanged_tag:content, "{% for v in heterogenous%}{% ifchanged %}{{ v }}{% else %}-{% endifchanged %}{% endfor %}",       "42-foo-")
DJANGO_TEST(ifchanged_tag:content, "{% for v in heterogenous%}{% ifchanged %}Y{% endifchanged %}{% endfor %}",                        "Y")
DJANGO_TEST(ifchanged_tag:content, "{% for v in heterogenous%}{% ifchanged %}Y{% else %}-{% endifchanged %}{% endfor %}",             "Y---")
DJANGO_TEST(ifchanged_tag:variables, "{% for v in heterogenous%}{% ifchanged v %}Y{% endifchanged %}{% endfor %}",                    "YY")
DJANGO_TEST(ifchanged_tag:variables, "{% for v in heterogenous%}{% ifchanged v %}Y{% else %}N{% endifchanged %}{% endfor %}",         "YNYN")
DJANGO_TEST(ifchanged_tag:variables, "{% for v in heterogenous%}{% ifchanged v %}{{ v }}{% endifchanged %}{% endfor %}",              "42foo")
DJANGO_TEST(ifchanged_tag:variables, "{% for v in heterogenous%}{% ifchanged v %}{{ v }}{% else %}-{% endifchanged %}{% endfor %}",   "42-foo-")
DJANGO_TEST(ifchanged_tag:variables, "{% for v in heterogenous%}{% ifchanged v v %}Y{% endifchanged %}{% endfor %}",                  "YY")
DJANGO_TEST(ifchanged_tag:variables, "{% for v in heterogenous%}{% ifchanged v v %}Y{% else %}N{% endifchanged %}{% endfor %}",       "YNYN")
DJANGO_TEST(ifchanged_tag:variables, "{% for v in heterogenous%}{% ifchanged v v %}{{ v }}{% endifchanged %}{% endfor %}",            "42foo")
DJANGO_TEST(ifchanged_tag:variables, "{% for v in heterogenous%}{% ifchanged v v %}{{ v }}{% else %}-{% endifchanged %}{% endfor %}", "42-foo-")

DJANGO_TEST(ifequal_tag, "{% ifequal 6 6 %}Good{% endifequal %}",              "Good")
DJANGO_TEST(ifequal_tag, "{% ifequal 5 6 %}Good{% endifequal %}",              "")
DJANGO_TEST(ifequal_tag, "{% ifequal 6 6 %}Good{% else %}Bad{% endifequal %}", "Good")
DJANGO_TEST(ifequal_tag, "{% ifequal 5 6 %}Bad{% else %}Good{% endifequal %}", "Good")

DJANGO_TEST(ifnotequal_tag, "{% ifnotequal 'hello' 'howdy' %}Good{% endifnotequal %}",              "Good")
DJANGO_TEST(ifnotequal_tag, "{% ifnotequal 'hello' 'hello' %}Good{% endifnotequal %}",              "")
DJANGO_TEST(ifnotequal_tag, "{% ifnotequal 'hello' 'howdy' %}Good{% else %}Bad{% endifnotequal %}", "Good")
DJANGO_TEST(ifnotequal_tag, "{% ifnotequal 'hello' 'hello' %}Bad{% else %}Good{% endifnotequal %}", "Good")

DJANGO_TEST(include_tag, "{% include 'tests/templates/django/empty.tpl' %}", "")
DJANGO_TEST(include_tag, "{% include 'tests/templates/django/variables.tpl' %}", "foo: A\nbar: B\nqux: C\n")

DJANGO_TEST(include_with_tag, "{% include 'tests/templates/django/empty.tpl' with foo=42 %}", "")
DJANGO_TEST(include_with_tag, "{% include 'tests/templates/django/variables.tpl' with foo=42 %}", "foo: 42\nbar: B\nqux: C\n")

DJANGO_TEST(include_with_only_tag, "{% include 'tests/templates/django/empty.tpl' with foo=42 only %}", "")
DJANGO_TEST(include_with_only_tag, "{% include 'tests/templates/django/variables.tpl' with foo=42 only %}", "foo: 42\nbar: \nqux: \n")

DJANGO_TEST(filter_tag, "{% filter escape %}<foo />{% endfilter %}", "<foo />")
DJANGO_TEST(filter_tag, "{% filter force_escape %}<foo />{% endfilter %}", "&lt;foo /&gt;")
DJANGO_TEST(filter_tag, "{% filter title|lower %}aBcD{% endfilter %}", "abcd")
DJANGO_TEST(filter_tag, "{% filter upper|lower|title %}aBcD{% endfilter %}", "Abcd")
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

DJANGO_TEST(for_tag-key-value,
    "{% for k, v in states %}[{{ k }}: {{ v }}]{% endfor %}",
    "[CA: California][FL: Florida][NY: New York]")

DJANGO_TEST(for_tag-key-value,
    "{% for k, v in states %}[{{ k }}: {{ v }}]{% empty %}Bad{% endfor %}",
    "[CA: California][FL: Florida][NY: New York]")

DJANGO_TEST(for_tag-value-reversed,
    "{% for v in friends reversed %}[{{ v }}]{% endfor %}",
    "[age: 41, name: lou][age: 55, name: bob][age: 23, name: joe]")

DJANGO_TEST(for_empty_tag-value-reversed,
    "{% for v in friends reversed %}[{{ v }}]{% empty %}Bad{% endfor %}",
    "[age: 41, name: lou][age: 55, name: bob][age: 23, name: joe]")

DJANGO_TEST(for_empty_tag-none-reversed,
    "{% for v in '' reversed %}Bad{% empty %} It's empty, Jim {% endfor %}",
    " It's empty, Jim ")

DJANGO_TEST(for_tag-key-value-reversed,
    "{% for k, v in states reversed %}[{{ k }}: {{ v }}]{% endfor %}",
    "[NY: New York][FL: Florida][CA: California]")

DJANGO_TEST(for_tag-key-value-reversed,
    "{% for k, v in states reversed %}[{{ k }}: {{ v }}]{% empty %}Bad{% endfor %}",
    "[NY: New York][FL: Florida][CA: California]")

unit_test(now_tag) {
    string_template_type const t("{% now 'y' %}");

    std::time_t time = std::time(0);
    string_type s = t.render_to_string(context);
    ensure_equals(s, behavior_type::to_string(std::localtime(&time)->tm_year % 100));
}}}

unit_test(now_tag) {
    string_template_type const t("{% now 'Y' %}");

    std::time_t time = std::time(0);
    string_type s = t.render_to_string(context);
    ensure_equals(s, behavior_type::to_string(std::localtime(&time)->tm_year + 1900));
}}}

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

DJANGO_TEST(widthratio_tag, "{% widthratio 100 100 100 %}", "100")
DJANGO_TEST(widthratio_tag, "{% widthratio 100 100 200 %}", "200")
DJANGO_TEST(widthratio_tag, "{% widthratio 100 100 300 %}", "300")
DJANGO_TEST(widthratio_tag, "{% widthratio 100 300 200 %}", "67")
DJANGO_TEST(widthratio_tag, "{% widthratio 100 200 100 %}", "50")
DJANGO_TEST(widthratio_tag, "{% widthratio 100 200 200 %}", "100")
DJANGO_TEST(widthratio_tag, "{% widthratio 100 200 300 %}", "150")
DJANGO_TEST(widthratio_tag, "{% widthratio 200 100 300 %}", "600")
DJANGO_TEST(widthratio_tag, "{% widthratio 200 300 100 %}", "67")
DJANGO_TEST(widthratio_tag, "{% widthratio 300 100 100 %}", "300")
DJANGO_TEST(widthratio_tag, "{% widthratio 300 100 200 %}", "600")
DJANGO_TEST(widthratio_tag, "{% widthratio 300 100 300 %}", "900")
DJANGO_TEST(widthratio_tag, "{% widthratio 300 200 100 %}", "150")

unit_test(url_tag) {
    string_template_type const t("{% url 'x.y.z' 1 2 3 %}");
    null_resolver_type::patterns_type patterns;
    options.resolvers.push_back(options_type::resolver_type(new null_resolver_type(patterns)));

    ensure_throws(std::runtime_error, t.render_to_string(context, options));
}}}

unit_test(url_tag) {
    string_template_type const t("{% url 'foo.bar.qux' 1 2 3 %}");
    null_resolver_type::patterns_type patterns;
    patterns["foo.bar.qux"] = "/foo-bar-qux";
    options.resolvers.push_back(options_type::resolver_type(new null_resolver_type(patterns)));

    ensure_equals(t.render_to_string(context, options), "/foo-bar-qux/1/2/3");
}}}

unit_test(url_tag) {
    string_template_type const t("{% url 'foo.bar.qux' 1 b=2 3 %}");
    null_resolver_type::patterns_type patterns;
    patterns["foo.bar.qux"] = "/foo-bar-qux";
    options.resolvers.push_back(options_type::resolver_type(new null_resolver_type(patterns)));

    ensure_equals(t.render_to_string(context, options), "/foo-bar-qux/1/3?b=2");
}}}

unit_test(url_as_tag) {
    string_template_type const t("{% url 'foo.bar.qux' 1 2 3 as foo %}_{{ foo }}");
    null_resolver_type::patterns_type patterns;
    patterns["foo.bar.qux"] = "/foo-bar-qux";
    options.resolvers.push_back(options_type::resolver_type(new null_resolver_type(patterns)));

    ensure_equals(t.render_to_string(context, options), "_/foo-bar-qux/1/2/3");
}}}

unit_test(url_as_tag) {
    string_template_type const t("{% url 'foo.bar.qux' a=1 2 c=3 as foo %}_{{ foo }}");
    null_resolver_type::patterns_type patterns;
    patterns["foo.bar.qux"] = "/foo-bar-qux";
    options.resolvers.push_back(options_type::resolver_type(new null_resolver_type(patterns)));

    ensure_equals(t.render_to_string(context, options), "_/foo-bar-qux/2?a=1&amp;c=3");
}}}

unit_test(url_as_tag) {
    string_template_type const t("{% url 'x.y.z' 1 2 3 as foo %}_{{ x }}");
    null_resolver_type::patterns_type patterns;
    options.resolvers.push_back(options_type::resolver_type(new null_resolver_type(patterns)));

    ensure_equals(t.render_to_string(context, options), "_");
}}}

DJANGO_TEST(variable_tag, "{{ heterogenous }}", "42, 42, foo, foo")

DJANGO_TEST_(variable_tag, "{{ foo }} {{ bar }} {{ qux }}", "  ",    NO_CONTEXT)
DJANGO_TEST_(variable_tag, "{{ foo }} {{ bar }} {{ qux }}", "A B C", context)
DJANGO_TEST_(variable_tag, "{{ '}}'|join:'}}' }}", "}}}}", NO_CONTEXT)
DJANGO_TEST_(variable_tag, "{{ '}}'|join:'}}' }}", "}}}}", context)

DJANGO_TEST(ssi_tag, "{% ssi " + text::quote(absolute_path + "/tests/templates/django/empty.tpl", '"') + " %}",            "")
DJANGO_TEST(ssi_tag, "{% ssi " + text::quote(absolute_path + "/tests/templates/django/empty.tpl", '"') + " parsed %}",     "")
DJANGO_TEST(ssi_tag, "{% ssi " + text::quote(absolute_path + "/tests/templates/django/variables.tpl", '"') + " %}",        "foo: {{ foo }}\nbar: {{ bar }}\nqux: {{ qux }}\n")
DJANGO_TEST(ssi_tag, "{% ssi " + text::quote(absolute_path + "/tests/templates/django/variables.tpl", '"') + " parsed %}", "foo: A\nbar: B\nqux: C\n")

unit_test(ssi_tag) {
    ensure_throws(std::invalid_argument, string_template_type("{% ssi '../foo' %}").render_to_string());
    ensure_throws(std::invalid_argument, string_template_type("{% ssi './foo' %}").render_to_string());
    ensure_throws(std::invalid_argument, string_template_type("{% ssi 'foo' %}").render_to_string());
    ensure_throws(std::invalid_argument, string_template_type("{% ssi '' %}").render_to_string());
}}}

DJANGO_TEST(verbatim_tag,
        "{% verbatim %}{% for v in friends %}\n"
        "    <p>{{ v }}</p>\n"
        "{% endfor %}{% endverbatim %}\n",
            "{% for v in friends %}\n"
            "    <p>{{ v }}</p>\n"
            "{% endfor %}\n")

DJANGO_TEST(with_tag, "[{{ls}}] {% with 'this is a long string' as ls %} {{ls}} {% endwith %} [{{ls}}]", "[]  this is a long string  []")

/// Filter tests
////////////////////////////////////////////////////////////////////////////////////////////////////

unit_test(missing-filter) {
    string_template_type const t("{{ 42|xyz }}");
    ensure_throws(s::missing_filter, t.render_to_string(context));
}}}

DJANGO_TEST(add_filter, "{{ '5'|add:6 }}", "11")
DJANGO_TEST(add_filter, "{{ 3|add:'8' }}", "11")

DJANGO_TEST(addslashes_filter, "{{ \"String with 'quotes'.\"|addslashes }}", "String with \\'quotes\\'.")

DJANGO_TEST(capfirst_filter, "{{ 'foo fa fa'|capfirst }}", "Foo fa fa")

DJANGO_TEST(center_filter, "{{ \"Django\"|center:\"15\" }}", "     Django    ")
DJANGO_TEST(center_filter, "{{ \"Django\"|center:\"16\" }}", "     Django     ")
DJANGO_TEST(center_filter, "{{ \"Django\"|center:\"2\" }}",  "Django")

DJANGO_TEST(cut_filter, "{{ 'String with spaces'|cut:' ' }}", "Stringwithspaces")

DJANGO_TEST(date_filter, "{{ past|date }}",                           "Jan 10, 2002")
DJANGO_TEST(date_filter, "{{ before_past|date:'r' }}",                "Tue, 08 Jan 2002 13:02:03 +0000")
DJANGO_TEST(date_filter, "{{ after_past|date:'SHORT_DATE_FORMAT' }}", "03/01/2002")

DJANGO_TEST(default_filter, "{{ True|default:\"default\" }}",  "True")
DJANGO_TEST(default_filter, "{{ False|default:\"default\" }}", "default")
DJANGO_TEST(default_filter, "{{ None|default:\"default\" }}",  "default")

DJANGO_TEST(default_if_none_filter, "{{ True|default_if_none:\"default\" }}",  "True")
DJANGO_TEST(default_if_none_filter, "{{ False|default_if_none:\"default\" }}", "False")
DJANGO_TEST(default_if_none_filter, "{{ None|default_if_none:\"default\" }}",  "default")

DJANGO_TEST(dictsort_filter, "{{ friends }}",                 "age: 23, name: joe, age: 55, name: bob, age: 41, name: lou")
DJANGO_TEST(dictsort_filter, "{{ friends|dictsort:'name' }}", "age: 55, name: bob, age: 23, name: joe, age: 41, name: lou")

DJANGO_TEST(dictsortreversed_filter, "{{ friends }}",                         "age: 23, name: joe, age: 55, name: bob, age: 41, name: lou")
DJANGO_TEST(dictsortreversed_filter, "{{ friends|dictsortreversed:'name' }}", "age: 41, name: lou, age: 23, name: joe, age: 55, name: bob")

DJANGO_TEST(divisibleby_filter, "{{ 21|divisibleby:\"3\" }}", "True")
DJANGO_TEST(divisibleby_filter, "{{ 20|divisibleby:\"3\" }}", "False")

DJANGO_TEST(escapejs_filter, "{{ haiku|escapejs }}", "Haikus are easy,\\x0ABut sometimes they don&apos;t make sense.\\x0ARefrigerator.\\x0A")
DJANGO_TEST(escapejs_filter, "{{ haiku|escapejs|safe }}", "Haikus are easy,\\x0ABut sometimes they don't make sense.\\x0ARefrigerator.\\x0A")
// DJANGO_TEST(escapejs_filter, "{{ binary_string|escapejs }}", "")

DJANGO_TEST(filesizeformat_filter, "{{ 123456789|filesizeformat }}", "117.7 MB")

DJANGO_TEST(fix_ampersands_filter, "{{ 'String & with & ampersands, but not &apos; or &#1234;'|fix_ampersands }}",
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

DJANGO_TEST(iriencode_filter, "{{ \"?test=1&me=2\"|iriencode }}", "?test=1&amp;me=2")

DJANGO_TEST(linenumbers_filter, "{{ haiku|linenumbers}}",
    "1. Haikus are easy,\n"
    "2. But sometimes they don&apos;t make sense.\n"
    "3. Refrigerator.\n"
    "4. \n")

DJANGO_TEST(linebreaksbr_filter, "{{ haiku|linebreaksbr }}", "Haikus are easy,<br />But sometimes they don't make sense.<br />Refrigerator.<br />")

DJANGO_TEST(linebreaks_filter, "{{ haiku|linebreaks }}", "<p>Haikus are easy,<br />But sometimes they don't make sense.<br />Refrigerator.<br /></p>\n\n")

DJANGO_TEST(ljust_filter, "{{ \"Django\"|ljust:\"10\" }}", "Django    ")
DJANGO_TEST(ljust_filter, "{{ \"Django\"|ljust:\"2\" }}",  "Django")

DJANGO_TEST(lower_filter, "{{ \"Still MAD At Yoko\"|lower }}", "still mad at yoko")

DJANGO_TEST(make_list_filter, "{{ numbers|make_list }}", "[1, 2, 3, 4, 5, 6, 7, 8, 9]")
DJANGO_TEST(make_list_filter, "{{ 12345|make_list }}",   "[1, 2, 3, 4, 5]")

DJANGO_TEST(phone2numeric_filter, "{{ \"1-800-COLLECT\"|phone2numeric }}", "1-800-2655328")

DJANGO_TEST(pluralize_filter, "ox{{ 0|pluralize:'en' }}",       "oxen")
DJANGO_TEST(pluralize_filter, "ox{{ 1|pluralize:'en' }}",       "ox")
DJANGO_TEST(pluralize_filter, "ox{{ 2|pluralize:'en' }}",       "oxen")
DJANGO_TEST(pluralize_filter, "tank{{ 0|pluralize }}",          "tanks")
DJANGO_TEST(pluralize_filter, "tank{{ 1|pluralize }}",          "tank")
DJANGO_TEST(pluralize_filter, "tank{{ 2|pluralize }}",          "tanks")
DJANGO_TEST(pluralize_filter, "cris{{ 0|pluralize:'is,es' }}",  "crises")
DJANGO_TEST(pluralize_filter, "cris{{ 1|pluralize:'is,es' }}",  "crisis")
DJANGO_TEST(pluralize_filter, "cris{{ 2|pluralize:'is,es' }}",  "crises")
DJANGO_TEST(pluralize_filter, "ferr{{ 0|pluralize:'y,ies' }}",  "ferries")
DJANGO_TEST(pluralize_filter, "ferr{{ 1|pluralize:'y,ies' }}",  "ferry")
DJANGO_TEST(pluralize_filter, "ferr{{ 2|pluralize:'y,ies' }}",  "ferries")

DJANGO_TEST(pprint_filter, "{{42|pprint }}",    "42")
DJANGO_TEST(pprint_filter, "{{6.6|pprint }}",   "6.6")
DJANGO_TEST(pprint_filter, "{{True|pprint }}",  "True")
DJANGO_TEST(pprint_filter, "{{'foo'|pprint }}", "&apos;foo&apos;")
DJANGO_TEST(pprint_filter, "{{past|pprint }}",  "2002-Jan-10 01:02:03")

DJANGO_TEST(removetags_filter, "{{ \"<b>Joel</b> <button>is</button> a <span>slug</span>\"|removetags:\"b span\"|safe }}", "Joel <button>is</button> a slug")
DJANGO_TEST(removetags_filter, "{{ \"<b>Begin</b> <foo /> <foo/> </foo> <foo> <span attr='value'>End</span>\"|removetags:\"b span foo\"|safe }}", "Begin     End")

DJANGO_TEST(rjust_filter, "{{ \"Django\"|rjust:\"10\" }}", "    Django")
DJANGO_TEST(rjust_filter, "{{ \"Django\"|rjust:\"2\" }}",  "Django")

DJANGO_TEST(slugify_filter, "{{ ' Joel is a slug '|slugify }}",           "joel-is-a-slug")
DJANGO_TEST(slugify_filter, "{{ '\tJoel\v is\n a\r slug\x01'|slugify }}", "joel-is-a-slug")

DJANGO_TEST(striptags_filter, "{{ '<b>Joel</b> <button>is</button> a <span>slug</span>'|striptags }}", "Joel is a slug")

DJANGO_TEST(stringformat_filter, "{{ 255|stringformat:'x' }}", "ff")

DJANGO_TEST(time_filter, "{{ past|time }}",                           "1:02 a.m.")
DJANGO_TEST(time_filter, "{{ before_past|time:'c' }}",                "2002-01-08T13:02:03")
DJANGO_TEST(time_filter, "{{ after_past|time:'YEAR_MONTH_FORMAT' }}", "March 2002")

DJANGO_TEST(formatting, "{{ past|time:'w;W;' }}",                "4;2;")
DJANGO_TEST(formatting, "{{ before_past|time:'w;W;' }}",         "2;2;")
DJANGO_TEST(formatting, "{{ after_past|time:'w;W;' }}",          "5;9;")

DJANGO_TEST(timesince_filter, "{{ past|timesince:before_past }}",       "0&nbsp;minutes")
DJANGO_TEST(timesince_filter, "{{ before_past|timesince:past }}",       "1&nbsp;day, 12&nbsp;hours")
DJANGO_TEST(timesince_filter, "{{ before_past|timesince:after_past }}", "1&nbsp;month, 3&nbsp;weeks")
DJANGO_TEST(timesince_filter, "{{ past|timesince:after_past }}",        "1&nbsp;month, 2&nbsp;weeks")
DJANGO_TEST(timesince_filter, "{{ future|timesince }}",                 "0&nbsp;minutes")

DJANGO_TEST(timeuntil_filter, "{{ past|timeuntil:before_past }}",       "1&nbsp;day, 12&nbsp;hours")
DJANGO_TEST(timeuntil_filter, "{{ after_past|timeuntil:before_past }}", "1&nbsp;month, 3&nbsp;weeks")
DJANGO_TEST(timeuntil_filter, "{{ after_past|timeuntil:past }}",        "1&nbsp;month, 2&nbsp;weeks")
DJANGO_TEST(timeuntil_filter, "{{ before_past|timeuntil:past }}",       "0&nbsp;minutes")
DJANGO_TEST(timeuntil_filter, "{{ past|timeuntil }}",                   "0&nbsp;minutes")

DJANGO_TEST(title_filter, "{{ \"my FIRST post\"|title }}", "My First Post")
DJANGO_TEST(title_filter, "{{ 'joel is a slug'|title }}", "Joel Is A Slug")

DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:0 }}",  "")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:1 }}",  "...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:2 }}",  "...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:3 }}",  "...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:4 }}",  "J...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:5 }}",  "Jo...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:6 }}",  "Joe...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:7 }}",  "Joel...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:8 }}",  "Joel ...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:9 }}",  "Joel i...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:10 }}", "Joel is...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:11 }}", "Joel is ...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:12 }}", "Joel is a...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:13 }}", "Joel is a ...")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:14 }}", "Joel is a slug")
DJANGO_TEST(truncatechars_filter, "{{ \"Joel is a slug\"|truncatechars:15 }}", "Joel is a slug")

DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatechars_html:0 }}",  "")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatechars_html:1 }}",  "<p>...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatechars_html:8 }}",  "<p>Joel ...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatechars_html:9 }}",  "<p>Joel i...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatechars_html:10 }}", "<p>Joel is...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatechars_html:14 }}", "<p>Joel is a slug</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatechars_html:15 }}", "<p>Joel is a slug</p>")

DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\"|truncatechars_html:0 }}",  "")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\"|truncatechars_html:1 }}",  "<p>...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\"|truncatechars_html:8 }}",  "<p>Joel ...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\"|truncatechars_html:9 }}",  "<p>Joel i...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\"|truncatechars_html:10 }}", "<p>Joel is...</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\"|truncatechars_html:14 }}", "<p>Joel is a slug</p>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"<p>Joel is a slug\"|truncatechars_html:15 }}", "<p>Joel is a slug</p>")

// TODO: Complete once we know the right behavior:
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:0 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:1 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:2 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:3 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:4 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:5 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:6 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:7 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:8 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:9 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:10 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:11 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:12 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:13 }}", "")
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:14 }}", "Joel <a href='#'>is <i>a</i> slug</a>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatechars_html:15 }}", "Joel <a href='#'>is <i>a</i> slug</a>")

// TODO: Complete once we know the right behavior:
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:0 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:1 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:2 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:3 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:4 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:5 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:6 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:7 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:8 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:9 }}",  "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:10 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:11 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:12 }}", "")
// DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:13 }}", "")
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:14 }}", "Joel <a href='#'>is <i>a</i> slug</a>")
DJANGO_TEST(truncatechars_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatechars_html:15 }}", "Joel <a href='#'>is <i>a</i> slug</a>")

DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\"|truncatewords:0 }}", " ...")
DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\"|truncatewords:1 }}", "Joel ...")
DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\"|truncatewords:2 }}", "Joel is ...")
DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\"|truncatewords:3 }}", "Joel is a ...")
DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\"|truncatewords:4 }}", "Joel is a slug")
DJANGO_TEST(truncatewords_filter, "{{ \"Joel is a slug\"|truncatewords:5 }}", "Joel is a slug")

DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \"|truncatewords:0 }}", " ...")
DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \"|truncatewords:1 }}", "Joel ...")
DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \"|truncatewords:2 }}", "Joel is ...")
DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \"|truncatewords:3 }}", "Joel is a ...")
DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \"|truncatewords:4 }}", "Joel is a slug")
DJANGO_TEST(truncatewords_filter, "{{ \"  Joel  is  a  slug  \"|truncatewords:5 }}", "Joel is a slug")

DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatewords_html:1 }}", "<p>Joel ...</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatewords_html:2 }}", "<p>Joel is ...</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatewords_html:3 }}", "<p>Joel is a ...</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatewords_html:4 }}", "<p>Joel is a slug</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug</p>\"|truncatewords_html:5 }}", "<p>Joel is a slug</p>")

DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\"|truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\"|truncatewords_html:1 }}", "<p>Joel ...</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\"|truncatewords_html:2 }}", "<p>Joel is ...</p>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\"|truncatewords_html:3 }}", "<p>Joel is a ...</p>")
// FIXME: DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\"|truncatewords_html:4 }}", "<p>Joel is a slug")
DJANGO_TEST(truncatewords_html_filter, "{{ \"<p>Joel is a slug\"|truncatewords_html:5 }}", "<p>Joel is a slug")

DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatewords_html:1 }}", "Joel ...")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatewords_html:2 }}", "Joel <a href='#'>is ...</a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatewords_html:3 }}", "Joel <a href='#'>is <i>a ...</i></a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatewords_html:4 }}", "Joel <a href='#'>is <i>a</i> slug</a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug</a>\"|truncatewords_html:5 }}", "Joel <a href='#'>is <i>a</i> slug</a>")

DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatewords_html:1 }}", "Joel ...")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatewords_html:2 }}", "Joel <a href='#'>is ...</a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatewords_html:3 }}", "Joel <a href='#'>is <i>a ...</i></a>")
// FIXME: DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatewords_html:4 }}", "Joel <a href='#'>is <i>a</i> slug")
DJANGO_TEST(truncatewords_html_filter, "{{ \"Joel <a href='#'>is <i>a</i> slug\"|truncatewords_html:5 }}", "Joel <a href='#'>is <i>a</i> slug")

DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \"|truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \"|truncatewords_html:1 }}", "  Joel ...")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \"|truncatewords_html:2 }}", "  Joel  <a href='#'>  is ...</a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \"|truncatewords_html:3 }}", "  Joel  <a href='#'>  is  <i>  a ...</i></a>")
// FIXME: DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \"|truncatewords_html:4 }}", "  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  ")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  \"|truncatewords_html:5 }}", "  Joel  <a href='#'>  is  <i>  a  </i>  slug  </a>  ")

DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \"|truncatewords_html:0 }}", "")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \"|truncatewords_html:1 }}", "  Joel ...")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \"|truncatewords_html:2 }}", "  Joel  <a href='#'>  is ...</a>")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \"|truncatewords_html:3 }}", "  Joel  <a href='#'>  is  <i>  a ...</i></a>")
// FIXME: DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \"|truncatewords_html:4 }}", "  Joel  <a href='#'>  is  <i>  a  </i>  slug  ")
DJANGO_TEST(truncatewords_html_filter, "{{ \"  Joel  <a href='#'>  is  <i>  a  </i>  slug  \"|truncatewords_html:5 }}", "  Joel  <a href='#'>  is  <i>  a  </i>  slug  ")

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

DJANGO_TEST(upper_filter, "{{ \"Joel is a slug\"|upper }}", "JOEL IS A SLUG")

DJANGO_TEST(urlencode_filter, "{{ \"/this should/be encoded ^ because @ is not an option $ ()\"|urlencode }}", "/this%20should/be%20encoded%20%5E%20because%20%40%20is%20not%20an%20option%20%24%20%28%29")

DJANGO_TEST(urlize_filter, "{{ \"This is some text containing a http://www.url.com sir and also another.url.com.\"|urlize }}", "This is some text containing a <a href='http://www.url.com'>http://www.url.com</a> sir and also <a href='http://another.url.com'>another.url.com</a>.")

DJANGO_TEST(urlizetrunc_filter, "{{ \"This is some text containing a http://www.url.com sir and also another.url.com.\"|urlizetrunc:15 }}", "This is some text containing a <a href='http://www.url.com'>http://www.url....</a> sir and also <a href='http://another.url.com'>another.url.com</a>.")

DJANGO_TEST(wordcount_filter, "{{ 'joel is a slug'|wordcount }}", "4")

DJANGO_TEST(wordwrap_filter, "{{ 'Joel is a slug'|wordwrap:5 }}", "\nJoel\nis a\nslug")

DJANGO_TEST(yesno_filter, "{{ true_var|yesno:'Yes,No' }}",       "Yes")
DJANGO_TEST(yesno_filter, "{{ false_var|yesno:'Yes,No' }}",      "No")
DJANGO_TEST(yesno_filter, "{{ True|yesno:\"yeah,no,maybe\" }}",  "yeah")
DJANGO_TEST(yesno_filter, "{{ False|yesno:\"yeah,no,maybe\" }}", "no")
DJANGO_TEST(yesno_filter, "{{ None|yesno:\"yeah,no,maybe\" }}",  "maybe")
DJANGO_TEST(yesno_filter, "{{ None|yesno:\"yeah,no\" }}",        "no")

DJANGO_TEST(escape_filter, "{{xml_var|escape}}", "&lt;foo&gt;&lt;bar&gt;&lt;qux /&gt;&lt;/bar&gt;&lt;/foo&gt;")

DJANGO_TEST(first_filter, "{{ 'abcde'|first }}", "a")

DJANGO_TEST(last_filter, "{{ 'abcde'|last }}", "e")

DJANGO_TEST(length_filter, "{{ 'abcde'|length }}", "5")

DJANGO_TEST(length_is_filter, "{{ 'abcde'|length_is:'4' }}", "False")
DJANGO_TEST(length_is_filter, "{{ 'abcde'|length_is:'5' }}", "True")
DJANGO_TEST(length_is_filter, "{{ 'abcde'|length_is:'6' }}", "False")

// TODO: Better test for random_filter, maybe by making randomness a value trait or engine option.
unit_test(random_filter) {
    string_template_type const t("{{ 'abcde'|random }}");
    string_type s = t.render_to_string(context);
    ensure(s == "a" || s == "b" || s == "c" || s == "d" || s == "e");
}}}

DJANGO_TEST(join_filter, "{{ 'abcde'|join:'_' }}", "a_b_c_d_e")

DJANGO_TEST(slice_filter, "{{ numbers|slice:'0:9'}}",   "1, 2, 3, 4, 5, 6, 7, 8, 9")
DJANGO_TEST(slice_filter, "{{ numbers|slice:':9'}}",    "1, 2, 3, 4, 5, 6, 7, 8, 9")
DJANGO_TEST(slice_filter, "{{ numbers|slice:':'}}",     "1, 2, 3, 4, 5, 6, 7, 8, 9")
DJANGO_TEST(slice_filter, "{{ numbers|slice:'0:'}}",    "1, 2, 3, 4, 5, 6, 7, 8, 9")
DJANGO_TEST(slice_filter, "{{ numbers|slice:'2:6'}}",   "3, 4, 5, 6")
DJANGO_TEST(slice_filter, "{{ numbers|slice:'-6:-2'}}", "4, 5, 6, 7")
