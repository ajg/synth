//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <ajg/testing.hpp>
#include <ajg/synth/engines/null_engine.hpp>

namespace {

using namespace ajg::synth;

typedef ajg::test_group<> group_type;

group_type group_object("templates");

} // namespace


AJG_TESTING_BEGIN

unit_test(string_template::text char) {
    string_template<char, null_engine> const t("foo bar qux");
    ensure_equals(t.text(), "foo bar qux");
}}}

unit_test(string_template::text wchar_t) {
    string_template<wchar_t, null_engine> const t(L"foo bar qux");
    ensure(t.text() == L"foo bar qux"); // XXX: ensure_equals doesn't play well with wchar_t.
}}}
