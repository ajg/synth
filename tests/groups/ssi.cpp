//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <ajg/synth/support.hpp>
#include <ajg/synth/testing.hpp>
#include <ajg/synth/templates.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/engines/ssi.hpp>
#include <ajg/synth/detail/find.hpp>
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

} // namespace

AJG_SYNTH_TEST_UNIT(sanity check) {
    string_template_type const t("");
    MUST_EQUAL(t.render_to_string(context), "");
}}}

AJG_SYNTH_TEST_UNIT(plain string) {
    string_template_type const t("ABC");
    MUST_EQUAL(t.render_to_string(context), "ABC");
}}}

AJG_SYNTH_TEST_UNIT(html tags) {
    string_template_type const t("<foo>\nA foo <bar /> element.\n</foo>");
    MUST_EQUAL(t.render_to_string(context), t.str());
}}}

AJG_SYNTH_TEST_UNIT(html comment) {
    string_template_type const t("<!-- A comment -->");
    MUST_EQUAL(t.render_to_string(context), t.str());
}}}

AJG_SYNTH_TEST_UNIT(environment variable) {
    std::string const name = AJG_SYNTH_IF_WINDOWS("Path", "PATH");
    char const *const path = std::getenv(name.c_str());
    MUST_NOT(path == 0);

    string_template_type const t("<!--#echo var='" + name + "' -->");
    MUST_EQUAL(t.render_to_string(context), path);
}}}

AJG_SYNTH_TEST_UNIT(non-extant variable) {
    MUST(std::getenv("non_extant_var") == 0);
    string_template_type const t("<!--#echo var='non_extant_var' -->");
    MUST_EQUAL(t.render_to_string(context), t.options().default_value.to_string());
}}}

AJG_SYNTH_TEST_UNIT(print environment) {
    char const *const path = std::getenv("PATH");
    MUST_NOT(path == 0);

    string_template_type const t("<!--#printenv -->");
    MUST_NOT_EQUAL(t.render_to_string(context).find(path), string_type::npos);
}}}

AJG_SYNTH_TEST_UNIT(simple set_tag) {
    string_template_type const t(
        "<!--#set var='foo' value='A' -->"
        "<!--#echo var='foo' -->");
    MUST_EQUAL(t.render_to_string(context), "A");
}}}

AJG_SYNTH_TEST_UNIT(multiple set_tag) {
    string_template_type const t(
        "<!--#set var='foo' value='A' -->"
        "<!--#set var='bar' value='B' -->"
        "<!--#set var='qux' value='C' -->"
        "<!--#echo var='foo' var='bar' var='qux' -->");
    MUST_EQUAL(t.render_to_string(context), "ABC");
}}}

AJG_SYNTH_TEST_UNIT(substitution in set_tag) {
    string_template_type const t(
        "<!--#set var='foo' value='A' -->"
        "<!--#set var='bar' value='${foo}_B' -->"
        "<!--#set var='qux' value='${bar}_C' -->"
        "<!--#echo var='qux' -->");
    MUST_EQUAL(t.render_to_string(context), "A_B_C");
}}}

AJG_SYNTH_TEST_UNIT(escaped dollar sign) {
    string_template_type const t(
        "<!--#set var='foo' value='\\$A' -->"
        "<!--#echo var='foo' -->");
    MUST_EQUAL(t.render_to_string(context), "$A");
}}}

AJG_SYNTH_TEST_UNIT(if_tag: true) {
    string_template_type t("<!--#if expr='1' -->true<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "true");
}}}

AJG_SYNTH_TEST_UNIT(if_tag: false) {
    string_template_type t("<!--#if expr='``' -->false<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "");
}}}

AJG_SYNTH_TEST_UNIT(boolean: unparenthesized) {
    string_template_type t("<!--#if expr='1 && 1' -->true<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "true");
}}}

AJG_SYNTH_TEST_UNIT(boolean: true and true) {
    string_template_type t("<!--#if expr='(1 && 1)' -->true<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "true");
}}}

AJG_SYNTH_TEST_UNIT(boolean: missing operand) {
    string_template_type t("<!--#if expr='(1 && )' -->false<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "");
}}}

AJG_SYNTH_TEST_UNIT(boolean: false and true) {
    string_template_type t("<!--#if expr='(`` && 1)' -->false<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "");
}}}

AJG_SYNTH_TEST_UNIT(boolean: false and false) {
    string_template_type t("<!--#if expr='(`` && ``)' -->false<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "");
}}}

AJG_SYNTH_TEST_UNIT(disjunctions) {
    string_template_type t("<!--#if expr='(1 && 1 && 1)' -->true<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "true");
}}}

AJG_SYNTH_TEST_UNIT(conjunctions) {
    string_template_type t("<!--#if expr='(`` || `` || ``)' -->false<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "");
}}}

AJG_SYNTH_TEST_UNIT(string comparison) {
    string_template_type t("<!--#if expr='(`a` < `b`)' -->true<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "true");
}}}

AJG_SYNTH_TEST_UNIT(lexicographical string comparison) {
    string_template_type t("<!--#if expr='(`100` < `20`)' -->true<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "true");
}}}

AJG_SYNTH_TEST_UNIT(regex expression match) {
    string_template_type t("<!--#if expr='(`a` = /a/)' -->true<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "true");
}}}

AJG_SYNTH_TEST_UNIT(regex expression no match) {
    string_template_type t("<!--#if expr='(`a` = /b/)' -->false<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "");
}}}

AJG_SYNTH_TEST_UNIT(regex substitution) {
    string_template_type t("<!--#if expr='(`foo` = /(o+)/)' --><!--#echo var='1' --><!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "oo");
}}}

AJG_SYNTH_TEST_UNIT(if_elif_tag) {
    string_template_type t("<!--#if expr='' -->foo"
        "<!--#elif expr='1' -->bar<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "bar");
}}}

AJG_SYNTH_TEST_UNIT(if_else_tag) {
    string_template_type t("<!--#if expr='' -->foo"
        "<!--#else -->bar<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "bar");
}}}

AJG_SYNTH_TEST_UNIT(if_elif_else_tag) {
    string_template_type t("<!--#if expr='' -->foo"
        "<!--#elif expr='' -->bar<!--#else -->qux<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), "qux");
}}}

AJG_SYNTH_TEST_UNIT(invalid if_tag: no expr) {
    string_template_type const t("<!--#if -->foo<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), t.options().error_value.to_string());
}}}

AJG_SYNTH_TEST_UNIT(invalid if_tag: multiple expr) {
    string_template_type const t("<!--#if expr='1' expr='1' -->foo<!--#endif -->");
    MUST_EQUAL(t.render_to_string(context), t.options().error_value.to_string());
}}}


/* TODO:
AJG_SYNTH_TEST_UNIT(magic variables) {
    // std::cout << std::endl << string_template_type("<!--#echo var='DATE_LOCAL' -->").render_to_string(context) << std::endl;
}}}*/


AJG_SYNTH_TEST_UNIT(multiple config_tag attributes) {
    string_template_type const t("<!--#config sizefmt='bytes' "
        "timefmt='%Y' echomsg='' errmsg='Error' -->");
    MUST_EQUAL(t.render_to_string(context), "");
}}}

AJG_SYNTH_TEST_UNIT(malformed tag) {
    MUST_THROW(s::parsing_error, string_template_type("<!--#e_cho -->"));
}}}

AJG_SYNTH_TEST_UNIT(invalid config_tag) {
    string_template_type const t("<!--#config foo='bar' -->");
    MUST_EQUAL(t.render_to_string(context), t.options().error_value.to_string());
}}}

AJG_SYNTH_TEST_UNIT(invalid config_tag sizefmt) {
    string_template_type const t("<!--#config sizefmt='foo' -->");
    MUST_EQUAL(t.render_to_string(context), t.options().error_value.to_string());
}}}

AJG_SYNTH_TEST_UNIT(fsize_tag bytes) {
    string_template_type const t("<!--#fsize file='tests/templates/ssi/1338' -->");
    MUST_EQUAL(t.render_to_string(context), "1338");
}}}

AJG_SYNTH_TEST_UNIT(fsize_tag abbrev) {
    string_template_type const t(
        "<!--#config sizefmt='abbrev' -->"
        "<!--#fsize file='tests/templates/ssi/1338' -->");
    MUST_EQUAL(t.render_to_string(context), "1.3 KB");
}}}

AJG_SYNTH_TEST_UNIT(flastmod_tag) {
    string_template_type const t("<!--#flastmod file='tests/templates/ssi/example.shtml' -->");
    string_type const time_format = t.options().format(text::literal("timefmt"));
    MUST_EQUAL(t.render_to_string(context), traits_type::format_time(time_format,
        traits_type::to_time(s::detail::stat_file("tests/templates/ssi/example.shtml").st_mtime)));
}}}

AJG_SYNTH_TEST_UNIT(flastmod_tag custom) {
    string_type const format("%H:%M:%S-%d/%m/%y");
    string_template_type const t(
        "<!--#config timefmt='" + format + "' -->"
        "<!--#flastmod file='tests/templates/ssi/example.shtml' -->");
    MUST_EQUAL(t.render_to_string(context), traits_type::format_time(format,
        traits_type::to_time(s::detail::stat_file("tests/templates/ssi/example.shtml").st_mtime)));
}}}

AJG_SYNTH_TEST_UNIT(tag with error) {
    string_template_type const t("<!--#fsize file='non-extant' -->");
    MUST_EQUAL(t.render_to_string(context), t.options().error_value.to_string());
}}}

AJG_SYNTH_TEST_UNIT(file template) {
    path_template_type const t("tests/templates/ssi/variables.shtml", options);
    MUST_EQUAL(t.render_to_string(context), "foo: A\nbar: B\nqux: C\n");
}}}

AJG_SYNTH_TEST_UNIT(include_tag) {
    string_template_type const t("<!--#include file='tests/templates/ssi/example.shtml' -->");
    MUST_EQUAL(t.render_to_string(context), "\n\n\n============\nfoo: A\nbar: B\nqux: C\n\n============\n");
}}}

AJG_SYNTH_TEST_UNIT(exec_tag) {
    std::string const command = AJG_SYNTH_IF_WINDOWS("dir", "ls");
    string_template_type const t("<!--#exec cmd='" + command + " \"tests/templates/ssi\"' -->");
    MUST_NOT_EQUAL(t.render_to_string(context).find("example.shtml"), string_type::npos);
}}}
