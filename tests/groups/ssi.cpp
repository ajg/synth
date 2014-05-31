//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <ajg/synth/support.hpp>
#include <ajg/synth/testing.hpp>
#include <ajg/synth/templates.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/engines/ssi.hpp>
#include <ajg/synth/detail/filesystem.hpp>

#include <tests/data/kitchen_sink.hpp>

namespace {

namespace s = ajg::synth;

typedef s::default_traits<char>                                                 traits_type;
typedef s::engines::ssi::engine<traits_type>                                    engine_type;

typedef s::templates::path_template<engine_type>                                path_template_type;
typedef s::templates::string_template<engine_type>                              string_template_type;

typedef engine_type::traits_type                                                traits_type;
typedef engine_type::context_type                                               context_type;
typedef engine_type::options_type                                               options_type;

typedef traits_type::char_type                                                  char_type;
typedef traits_type::string_type                                                string_type;

struct data_type : tests::data::kitchen_sink<engine_type> {};

AJG_SYNTH_TEST_GROUP_WITH_DATA("ssi", data_type);

options_type const default_options;

} // namespace

unit_test(sanity check) {
    string_template_type const t("");
    ensure_equals(t.render_to_string(context), "");
}}}

unit_test(plain string) {
    string_template_type const t("ABC");
    ensure_equals(t.render_to_string(context), "ABC");
}}}

unit_test(html tags) {
    string_template_type const t("<foo>\nA foo <bar /> element.\n</foo>");
    ensure_equals(t.render_to_string(context), t.str());
}}}

unit_test(html comment) {
    string_template_type const t("<!-- A comment -->");
    ensure_equals(t.render_to_string(context), t.str());
}}}

unit_test(environment variable) {
    char const *const path = std::getenv("PATH");
    ensure("PATH environment variable is set", path != 0);

    string_template_type const t("<!--#echo var='PATH' -->");
    ensure_equals(t.render_to_string(context), path);
}}}

unit_test(non-extant variable) {
    ensure(std::getenv("non_extant_var") == 0);
    string_template_type const t("<!--#echo var='non_extant_var' -->");
    ensure_equals(t.render_to_string(context), default_options.echo_message);
}}}

unit_test(print environment) {
    char const *const path = std::getenv("PATH");
    ensure("PATH environment variable is set", path != 0);

    string_template_type const t("<!--#printenv -->");
    ensure(t.render_to_string(context).find(path) != string_type::npos);
}}}

unit_test(simple set_tag) {
    string_template_type const t(
        "<!--#set var='foo' value='A' -->"
        "<!--#echo var='foo' -->");
    ensure_equals(t.render_to_string(context), "A");
}}}

unit_test(multiple set_tag) {
    string_template_type const t(
        "<!--#set var='foo' value='A' -->"
        "<!--#set var='bar' value='B' -->"
        "<!--#set var='qux' value='C' -->"
        "<!--#echo var='foo' var='bar' var='qux' -->");
    ensure_equals(t.render_to_string(context), "ABC");
}}}

unit_test(substitution in set_tag) {
    string_template_type const t(
        "<!--#set var='foo' value='A' -->"
        "<!--#set var='bar' value='${foo}_B' -->"
        "<!--#set var='qux' value='${bar}_C' -->"
        "<!--#echo var='qux' -->");
    ensure_equals(t.render_to_string(context), "A_B_C");
}}}

unit_test(escaped dollar sign) {
    string_template_type const t(
        "<!--#set var='foo' value='\\$A' -->"
        "<!--#echo var='foo' -->");
    ensure_equals(t.render_to_string(context), "$A");
}}}

unit_test(if_tag: true) {
    string_template_type t("<!--#if expr='1' -->true<!--#endif -->");
    ensure_equals(t.render_to_string(context), "true");
}}}

unit_test(if_tag: false) {
    string_template_type t("<!--#if expr='``' -->false<!--#endif -->");
    ensure_equals(t.render_to_string(context), "");
}}}

unit_test(boolean: unparenthesized) {
    string_template_type t("<!--#if expr='1 && 1' -->true<!--#endif -->");
    ensure_equals(t.render_to_string(context), "true");
}}}

unit_test(boolean: true and true) {
    string_template_type t("<!--#if expr='(1 && 1)' -->true<!--#endif -->");
    ensure_equals(t.render_to_string(context), "true");
}}}

unit_test(boolean: missing operand) {
    string_template_type t("<!--#if expr='(1 && )' -->false<!--#endif -->");
    ensure_equals(t.render_to_string(context), "");
}}}

unit_test(boolean: false and true) {
    string_template_type t("<!--#if expr='(`` && 1)' -->false<!--#endif -->");
    ensure_equals(t.render_to_string(context), "");
}}}

unit_test(boolean: false and false) {
    string_template_type t("<!--#if expr='(`` && ``)' -->false<!--#endif -->");
    ensure_equals(t.render_to_string(context), "");
}}}

unit_test(disjunctions) {
    string_template_type t("<!--#if expr='(1 && 1 && 1)' -->true<!--#endif -->");
    ensure_equals(t.render_to_string(context), "true");
}}}

unit_test(conjunctions) {
    string_template_type t("<!--#if expr='(`` || `` || ``)' -->false<!--#endif -->");
    ensure_equals(t.render_to_string(context), "");
}}}

unit_test(string comparison) {
    string_template_type t("<!--#if expr='(`a` < `b`)' -->true<!--#endif -->");
    ensure_equals(t.render_to_string(context), "true");
}}}

unit_test(lexicographical string comparison) {
    string_template_type t("<!--#if expr='(`100` < `20`)' -->true<!--#endif -->");
    ensure_equals(t.render_to_string(context), "true");
}}}

unit_test(regex expression match) {
    string_template_type t("<!--#if expr='(`a` = /a/)' -->true<!--#endif -->");
    ensure_equals(t.render_to_string(context), "true");
}}}

unit_test(regex expression no match) {
    string_template_type t("<!--#if expr='(`a` = /b/)' -->false<!--#endif -->");
    ensure_equals(t.render_to_string(context), "");
}}}

unit_test(regex substitution) {
    string_template_type t("<!--#if expr='(`foo` = /(o+)/)' --><!--#echo var='1' --><!--#endif -->");
    ensure_equals(t.render_to_string(context), "oo");
}}}

unit_test(if_elif_tag) {
    string_template_type t("<!--#if expr='' -->foo"
        "<!--#elif expr='1' -->bar<!--#endif -->");
    ensure_equals(t.render_to_string(context), "bar");
}}}

unit_test(if_else_tag) {
    string_template_type t("<!--#if expr='' -->foo"
        "<!--#else -->bar<!--#endif -->");
    ensure_equals(t.render_to_string(context), "bar");
}}}

unit_test(if_elif_else_tag) {
    string_template_type t("<!--#if expr='' -->foo"
        "<!--#elif expr='' -->bar<!--#else -->qux<!--#endif -->");
    ensure_equals(t.render_to_string(context), "qux");
}}}

unit_test(invalid if_tag: no expr) {
    string_template_type const t("<!--#if -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(context), default_options.error_message);
}}}

unit_test(invalid if_tag: multiple expr) {
    string_template_type const t("<!--#if expr='1' expr='1' -->foo<!--#endif -->");
    ensure_equals(t.render_to_string(context), default_options.error_message);
}}}


/* TODO:
unit_test(magic variables) {
    // std::cout << std::endl << string_template_type("<!--#echo var='DATE_LOCAL' -->").render_to_string(context) << std::endl;
}}}*/


unit_test(multiple config_tag attributes) {
    string_template_type const t("<!--#config sizefmt='bytes' "
        "timefmt='%Y' echomsg='' errmsg='Error' -->");
    ensure_equals(t.render_to_string(context), "");
}}}

unit_test(malformed tag) {
    ensure_throws(s::parsing_error, string_template_type("<!--#e_cho -->"));
}}}

unit_test(invalid config_tag) {
    string_template_type const t("<!--#config foo='bar' -->", default_options);
    ensure_equals(t.render_to_string(context), default_options.error_message);
}}}

unit_test(invalid config_tag sizefmt) {
    string_template_type const t("<!--#config sizefmt='foo' -->", default_options);
    ensure_equals(t.render_to_string(context), default_options.error_message);
}}}

unit_test(fsize_tag bytes) {
    string_template_type const t("<!--#fsize file='tests/templates/ssi/1338' -->");
    ensure_equals(t.render_to_string(context), "1338");
}}}

unit_test(fsize_tag abbrev) {
    string_template_type const t(
        "<!--#config sizefmt='abbrev' -->"
        "<!--#fsize file='tests/templates/ssi/1338' -->");
    ensure_equals(t.render_to_string(context), "1.3 KB");
}}}

unit_test(flastmod_tag) {
    string_template_type const t(
        "<!--#flastmod file='tests/templates/ssi/example.shtml' -->", default_options);
    ensure_equals(t.render_to_string(context), traits_type::format_time(default_options.time_format,
        traits_type::to_time(s::detail::stat_file("tests/templates/ssi/example.shtml").st_mtime)));
}}}

unit_test(flastmod_tag custom) {
    string_type const format("%H:%M:%S-%d/%m/%y");
    string_template_type const t(
        "<!--#config timefmt='" + format + "' -->"
        "<!--#flastmod file='tests/templates/ssi/example.shtml' -->");
    ensure_equals(t.render_to_string(context), traits_type::format_time(format,
        traits_type::to_time(s::detail::stat_file("tests/templates/ssi/example.shtml").st_mtime)));
}}}

unit_test(tag with error) {
    string_template_type const t("<!--#fsize file='non-extant' -->", default_options);
    ensure_equals(t.render_to_string(context), default_options.error_message);
}}}

unit_test(file template) {
    path_template_type const t("tests/templates/ssi/variables.shtml", options.directories, options);
    ensure_equals(t.render_to_string(context), "foo: A\nbar: B\nqux: C\n");
}}}

unit_test(include_tag) {
    string_template_type const t("<!--#include file='tests/templates/ssi/example.shtml' -->");
    ensure_equals(t.render_to_string(context), "\n\n\n============\nfoo: A\nbar: B\nqux: C\n\n============\n");
}}}

unit_test(exec_tag) {
    std::string const command = AJG_SYNTH_IF_WINDOWS("dir", "ls");
    string_template_type const t("<!--#exec cmd='" + command + " \"tests/templates/ssi\"' -->");
    ensure(t.render_to_string(context).find("example.shtml") != string_type::npos);
}}}
