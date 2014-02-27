//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <string>

#include <ajg/testing.hpp>
#include <ajg/synth/template.hpp>
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
    unit_test(sanity check) { \
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

DJANGO_TEST(yesno_filter yes, "{{ true_var|yesno:'Yes,No' }}", "Yes", context)
DJANGO_TEST(yesno_filter no, "{{ false_var|yesno:'Yes,No' }}", "No", context)
// DJANGO_TEST(yesno_filter maybe, "{{ no_var|yesno:'Yes,No,Maybe' }}", "Maybe", context)

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

/* TODO: unit_testize
{{some_strings[2]}}, {{some_strings[1]}}
{{3.3}}
{{3.30}}

{{34.23234|floatformat }} == 34.2
{{34.00000|floatformat }} == 34
{{34.26000|floatformat }} == 34.3

{{34.23234|floatformat:3 }} == 34.232
{{34.00000|floatformat:3 }} == 34.000
{{34.26000|floatformat:3 }} == 34.260

{{34.23234|floatformat:"-3" }} == 34.232
{{34.00000|floatformat:"-3" }} == 34
{{34.26000|floatformat:"-3" }} == 34.260


{% for k, v in a_string_array %}
    {% cycle k v as ttt %} hello. {{ttt}}
{% endfor %}
hi.

{{"Joel is a slug"|truncatewords:2}}

{{"This is some text containing a http://www.url.com sir and also another.url.com."|urlize}}
{{"This is some text containing a http://www.url.com sir and also another.url.com."|urlizetrunc:15}}

{{"<b>Joel</b> <button>is</button> a <span>slug</span>"|removetags:"b span"|safe }}
{{"<b>Begin</b> <foo /> <foo/> </foo> <foo> <span attr='value'>End</span>"|removetags:"b span foo"|safe }}
*/
