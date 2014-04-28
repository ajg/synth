//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <string>

#include <ajg/synth/testing.hpp>
#include <ajg/synth/templates.hpp>
#include <ajg/synth/engines/null.hpp>
#include <ajg/synth/detail/filesystem.hpp>

namespace {

namespace s = ajg::synth;

using s::detail::read_path_to_string;

typedef s::engines::null::engine<s::default_traits<char> > char_engine;

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T

using s::wensure_equals;

typedef s::engines::null::engine<s::default_traits<wchar_t> > wchar_t_engine;

#endif

AJG_SYNTH_TEST_GROUP("templates");

} // namespace

unit_test(char_template::text char array) {
    s::templates::char_template<char_engine> const t("foo bar qux");
    ensure_equals(t.text(), "foo bar qux");
}}}

unit_test(char_template::text char pointer) {
    char const *const s = "foo bar qux";
    s::templates::char_template<char_engine> const t(s);
    ensure_equals(t.text(), "foo bar qux");
}}}

unit_test(path_template::text char) {
    s::templates::path_template<char_engine> const t("tests/templates/tmpl/variables.tmpl");
    ensure_equals(t.text(), read_path_to_string<char>("tests/templates/tmpl/variables.tmpl"));
}}}

unit_test(stream_template::text char) {
    std::istringstream stream("foo bar qux");
    s::templates::stream_template<char_engine> const t(stream);
    ensure_equals(t.text(), "foo bar qux");
}}}

unit_test(string_template::text char) {
    s::templates::string_template<char_engine> const t("foo bar qux");
    ensure_equals(t.text(), "foo bar qux");
}}}

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T

unit_test(char_template::text wchar_t array) {
    s::templates::char_template<wchar_t_engine> const t(L"foo bar qux");
    wensure_equals(t.text(), L"foo bar qux");
}}}

unit_test(char_template::text wchar_t pointer) {
    wchar_t const *const s = L"foo bar qux";
    s::templates::char_template<wchar_t_engine> const t(s);
    wensure_equals(t.text(), L"foo bar qux");
}}}

unit_test(path_template::text wchar_t) {
    s::templates::path_template<wchar_t_engine> const t(L"tests/templates/tmpl/variables.tmpl");
    wensure_equals(t.text(), read_path_to_string<wchar_t>("tests/templates/tmpl/variables.tmpl"));
}}}

unit_test(stream_template::text wchar_t) {
    std::wistringstream stream(L"foo bar qux");
    s::templates::stream_template<wchar_t_engine> const t(stream);
    wensure_equals(t.text(), L"foo bar qux");
}}}

unit_test(string_template::text wchar_t) {
    s::templates::string_template<wchar_t_engine> const t(L"foo bar qux");
    wensure_equals(t.text(), L"foo bar qux");
}}}

#endif
