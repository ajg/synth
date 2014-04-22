//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <string>

#include <ajg/testing.hpp>

#include <ajg/synth/templates.hpp>
#include <ajg/synth/engines/null_engine.hpp>


#ifndef AJG_SYNTH_NO_WCHAR_T

inline void wensure_equals(std::wstring const& expect, std::wstring const& actual) {
    tut::ensure_equals(actual.length(), expect.length());
    tut::ensure(actual == expect); // XXX: tut's ensure_equals doesn't play well with wchar_t.
}

#endif

template <class Char> // TODO: Move close to detail::read_file.
std::basic_string<Char> read_to_string(char const* const path) {
    FILE* const file = (std::fopen)(path, "rb");
    std::basic_ostringstream<Char> stream;
    ajg::synth::detail::read_file(file, stream);
    (std::fclose)(file); // FIXME: Not exception safe, but unlikely to be a problem.
    return stream.str();
}


namespace {

using namespace ajg::synth;

typedef ajg::test_group<> group_type;

group_type group_object("templates");

} // namespace

AJG_TESTING_BEGIN

typedef null_engine<default_traits<char> > char_engine;

unit_test(char_template::text char) {
    char_template<char_engine> const t("foo bar qux");
    ensure_equals(t.text(), "foo bar qux");
}}}

unit_test(char_template::text char) {
    char const *const s = "foo bar qux";
    char_template<char_engine> const t(s);
    ensure_equals(t.text(), "foo bar qux");
}}}

unit_test(path_template::text char) {
    path_template<char_engine> const t("tests/templates/tmpl/variables.tmpl");
    ensure_equals(t.text(), read_to_string<char>("tests/templates/tmpl/variables.tmpl"));
}}}

unit_test(stream_template::text char) {
    std::istringstream stream("foo bar qux");
    stream_template<char_engine> const t(stream);
    ensure_equals(t.text(), "foo bar qux");
}}}

unit_test(string_template::text char) {
    string_template<char_engine> const t("foo bar qux");
    ensure_equals(t.text(), "foo bar qux");
}}}

#ifndef AJG_SYNTH_NO_WCHAR_T

typedef null_engine<default_traits<wchar_t> > wchar_t_engine;

unit_test(char_template::text wchar_t) {
    char_template<wchar_t_engine> const t(L"foo bar qux");
    wensure_equals(t.text(), L"foo bar qux");
}}}

unit_test(char_template::text wchar_t) {
    wchar_t const *const s = L"foo bar qux";
    char_template<wchar_t_engine> const t(s);
    wensure_equals(t.text(), L"foo bar qux");
}}}

unit_test(path_template::text wchar_t) {
    path_template<wchar_t_engine> const t(L"tests/templates/tmpl/variables.tmpl");
    wensure_equals(t.text(), read_to_string<wchar_t>("tests/templates/tmpl/variables.tmpl"));
}}}

unit_test(stream_template::text wchar_t) {
    std::wistringstream stream(L"foo bar qux");
    stream_template<wchar_t_engine> const t(stream);
    wensure_equals(t.text(), L"foo bar qux");
}}}

unit_test(string_template::text wchar_t) {
    string_template<wchar_t_engine> const t(L"foo bar qux");
    wensure_equals(t.text(), L"foo bar qux");
}}}

#endif
