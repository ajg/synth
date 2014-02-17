//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <string>

#include <ajg/testing.hpp>
#include <ajg/synthesis/engines/django.hpp>

namespace {
typedef char char_t;
namespace s = ajg::synthesis;
typedef s::django::engine<> engine_type;
typedef s::file_template<char_t, engine_type> file_template;
typedef s::string_template<char_t, engine_type> string_template;

typedef ajg::test_group<> group_type;
group_type group_object("django tests");

} // namespace

AJG_TESTING_BEGIN

unit_test(plain text) {
    string_template const t("ABC");
    ensure_equals(t.render_to_string(), "ABC");
}}}

unit_test(html tags) {
    string_template const t("<foo>\nA foo <bar /> element.\n</foo>");
    ensure_equals(t.render_to_string(), t.text());
}}}
