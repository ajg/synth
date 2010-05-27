
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <cstdlib>

#include <chemical/testing.hpp>
#include <chemical/synthesis/template.hpp>
#include <chemical/synthesis/adapters.hpp>
#include <chemical/synthesis/engines/ssi.hpp>

typedef chemical::test_group<> group_type;
group_type group_object("ssi tests");
CHEMICAL_TESTING_BEGIN

typedef char char_t;
namespace cs = chemical::synthesis;
typedef cs::ssi::engine<> engine_type;
typedef cs::file_template<char_t, engine_type> file_template;
typedef cs::string_template<char_t, engine_type> string_template;
typedef string_template::string_type string_type;

static string_template::options_type const default_options;

unit_test(html tags) {
    string_template const t("<foo>\nA foo <bar /> element.\n</foo>");
    ensure_equals(t.render_to_string(), t.text());
}}}

unit_test(html comment) {
    string_template const t("<!-- A comment -->");
    ensure_equals(t.render_to_string(), t.text());
}}}

unit_test(environment variable) {
    char const * const path = std::getenv("PATH");
    ensure("PATH environment variable is set", path != 0);

    string_template const t("<!--#echo var='PATH' -->");
    ensure_equals(t.render_to_string(), path);
}}}

unit_test(non-extant variable) {
    ensure(std::getenv("non_extant_var") == 0);
    string_template const t("<!--#echo var='non_extant_var' -->");
    ensure_equals(t.render_to_string(), default_options.echo_message);
}}}

unit_test(print environment) {
    char const * const path = std::getenv("PATH");
    ensure("PATH environment variable is set", path != 0);

    string_template const t("<!--#printenv -->");
    ensure(t.render_to_string().find(path) != string_type::npos);
}}}

unit_test(simple set directive) {
    string_template const t(
        "<!--#set var='foo' value='A' -->"
        "<!--#echo var='foo' -->");
    ensure_equals(t.render_to_string(), "A");
}}}

unit_test(multiple set directive) {
    string_template const t(
        "<!--#set var='foo' value='A' -->"
        "<!--#set var='bar' value='B' -->"
        "<!--#set var='qux' value='C' -->"
        "<!--#echo var='foo' var='bar' var='qux' -->");
    ensure_equals(t.render_to_string(), "ABC");
}}}

unit_test(substitution in set directive) {
    string_template const t(
        "<!--#set var='foo' value='A' -->"
        "<!--#set var='bar' value='${foo}_B' -->"
        "<!--#set var='qux' value='${bar}_C' -->"
        "<!--#echo var='qux' -->");
    ensure_equals(t.render_to_string(), "A_B_C");
}}}


unit_test(if directive a) {
    string_template t("<!--#if expr='1' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), "foo");
}}}

unit_test(if directive b) {
    string_template t("<!--#if expr='``' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), "");
}}}

unit_test(if directive c) {
    string_template t("<!--#if expr='1 && 1' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), "foo");
}}}

unit_test(if directive d) {
    string_template t("<!--#if expr='(1 && 1)' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), "foo");
}}}

unit_test(if directive e) {
    string_template t("<!--#if expr='(1 && )' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), "");
}}}

unit_test(if directive f) {
    string_template t("<!--#if expr='(`` && 1)' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), "");
}}}

unit_test(if directive g) {
    string_template t("<!--#if expr='(`` && ``)' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), "");
}}}

unit_test(if directive h) {
    string_template t("<!--#if expr='(1 && 1 && 1)' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), "foo");
}}}

unit_test(if directive i) {
    string_template t("<!--#if expr='(`` || `` || ``)' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), "");
}}}

unit_test(if directive i) {
    string_template t("<!--#if expr='(`a` < `b`)' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), "foo");
}}}

unit_test(if elif directive) {
    string_template t("<!--#if expr='' -->foo"
        "<!--#elif expr='1' -->bar<!--#endif -->");
    ensure_equals(t.render_to_string(), "bar");
}}}

unit_test(if else directive) {
    string_template t("<!--#if expr='' -->foo"
        "<!--#else -->bar<!--#endif -->");
    ensure_equals(t.render_to_string(), "bar");
}}}

unit_test(if elif else directive) {
    string_template t("<!--#if expr='' -->foo"
        "<!--#elif expr='' -->bar<!--#else -->qux<!--#endif -->");
    ensure_equals(t.render_to_string(), "qux");
}}}

unit_test(invalid if - no expr) {
    string_template const t("<!--#if -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), default_options.error_message);
}}}

unit_test(invalid if - multiple expr) {
    string_template const t("<!--#if expr='1' expr='1' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(), default_options.error_message);
}}}


/*
unit_test(magic variables) {
    // std::cout << std::endl << string_template("<!--#echo var='DATE_LOCAL' -->").render_to_string() << std::endl;
}}}*/


unit_test(multiple config attributes) {
    string_template const t("<!--#config sizefmt='bytes' "
        "timefmt='%Y' echomsg='' errmsg='Error' -->");
    ensure_equals(t.render_to_string(), "");
}}}

unit_test(invalid directive) {
    ensure_throws(cs::parsing_error,
        string_template("<!--#e_cho -->"));
}}}

unit_test(invalid config) {
    string_template const t("<!--#config foo='bar' -->");
    ensure_equals(t.render_to_string(), default_options.error_message);
}}}

unit_test(invalid config sizefmt) {
    string_template const t("<!--#config sizefmt='foo' -->");
    ensure_equals(t.render_to_string(), default_options.error_message);
}}}

unit_test(fsize directive bytes) {
    string_template const t(
        "<!--#fsize file='LICENSE_1_0.txt' -->");
    ensure_equals(t.render_to_string(), "1338");
}}}

unit_test(fsize directive abbrev) {
    string_template const t(
        "<!--#config sizefmt='abbrev' -->"
        "<!--#fsize file='LICENSE_1_0.txt' -->");
    ensure_equals(t.render_to_string(), "1.3 KB");
}}}

unit_test(flastmod directive) {
    string_template const t(
        "<!--#flastmod file='LICENSE_1_0.txt' -->");
    ensure_equals(t.render_to_string(), cs::detail::format_time(default_options.time_format,
        boost::posix_time::from_time_t(cs::detail::stat_file("LICENSE_1_0.txt").st_mtime)));
}}}

unit_test(flastmod directive custom) {
    string_type const format("%H:%M:%S-%d/%m/%y");
    string_template const t(
        "<!--#config timefmt='" + format + "' -->"
        "<!--#flastmod file='LICENSE_1_0.txt' -->");
    ensure_equals(t.render_to_string(), cs::detail::format_time(format,
        boost::posix_time::from_time_t(cs::detail::stat_file("LICENSE_1_0.txt").st_mtime)));
}}}

unit_test(directive with error) {
    string_template const t("<!--#fsize file='non-extant' -->");
    ensure_equals(t.render_to_string(), default_options.error_message);
}}}

unit_test(file template) {
    std::string const m = default_options.echo_message;
    file_template const t("samples/variables.shtml");
    ensure_equals(t.render_to_string(), m + m + m);
}}}

unit_test(include directive) {
    string_template const t(
        "<!--#include file='samples/example.shtml' -->");
    ensure_equals(t.render_to_string(), "\n\n\n"
        "============\nABC\n============");
}}}

unit_test(exec directive) {
    std::string const command = CHEMICAL_WIN32_DIVERGE("dir", "ls");
    string_template const t("<!--#exec cmd='" + command + "' -->");
    ensure(t.render_to_string().find("LICENSE_1_0.txt") != string_type::npos);
}}}
