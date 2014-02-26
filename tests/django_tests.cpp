//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <string>

#include <ajg/testing.hpp>
#include <ajg/synthesis/template.hpp>
#include <ajg/synthesis/adapters.hpp>
#include <ajg/synthesis/engines/django.hpp>

namespace {
namespace s = ajg::synthesis;

typedef char char_t;
typedef s::django::engine<> engine_type;
typedef s::file_template<char_t, engine_type> file_template;
typedef s::string_template<char_t, engine_type> string_template;

struct context_data {
    context_data() {
        context["foo"] = "A";
        context["bar"] = "B";
        context["qux"] = "C";

        context["true_var"]  = true;
        context["false_var"] = false;

        std::map<std::string, std::string> joe, bob, lou;
        joe["name"] = "joe";
        joe["age"]  = "23";
        bob["name"] = "bob";
        bob["age"]  = "55";
        lou["name"] = "lou";
        lou["age"]  = "41";

        friends[0] = joe;
        friends[1] = bob;
        friends[2] = lou;

        context["friends"] = friends;
    }

    string_template::context_type context;
    std::map<std::string, std::string> friends[3];
};

typedef ajg::test_group<context_data> group_type;
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
