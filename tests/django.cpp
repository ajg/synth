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

        context["true_var"] = true;
        context["false_var"] = false;

        std::map<std::string, std::string> joe, bob, lou;
        joe["name"] = "joe"; joe["age"] = "23";
        bob["name"] = "bob"; bob["age"] = "55";
        lou["name"] = "lou"; lou["age"] = "41";
        friends[0] = joe; friends[1] = bob; friends[2] = lou;

        context["friends"] = friends;
    }

    string_template::context_type context;
    std::map<std::string, std::string> friends[3];
};

typedef ajg::test_group<context_data> group_type;
group_type group_object("django tests");

} // namespace

AJG_TESTING_BEGIN

unit_test(sanity check) {
    string_template const t("");
    ensure_equals(t.render_to_string(), "");
    ensure_equals(t.render_to_string(context), "");
}}}

unit_test(plain text) {
    string_template const t("ABC");
    ensure_equals(t.render_to_string(), "ABC");
    ensure_equals(t.render_to_string(context), "ABC");
}}}

unit_test(html tags) {
    string_template const t("<foo>\nA foo <bar /> element.\n</foo>");
    ensure_equals(t.render_to_string(), t.text());
    ensure_equals(t.render_to_string(context), t.text());
}}}

unit_test(for_tag) {
    string_template const t(
        "{% for k, v in friends %}\n"
        "    <p>{{ k }} - {{ v }}</p>\n"
        "{% endfor %}\n");
    ensure_equals(t.render_to_string(context),
        "\n"
        "    <p>0 - age: 23, name: joe</p>\n"
        "\n"
        "    <p>1 - age: 55, name: bob</p>\n"
        "\n"
        "    <p>2 - age: 41, name: lou</p>\n"
        "\n");
}}}

unit_test(verbatim_tag) {
    string_template const t(
        "{% verbatim %}{% for k, v in friends %}\n"
        "    <p>{{ k }} - {{ v }}</p>\n"
        "{% endfor %}{% endverbatim %}\n");
    ensure_equals(t.render_to_string(context),
        "{% for k, v in friends %}\n"
        "    <p>{{ k }} - {{ v }}</p>\n"
        "{% endfor %}\n");
}}}
