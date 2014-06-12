//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <stdexcept>

#include <ajg/synth/testing.hpp>
#include <ajg/synth/templates.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/engines/tmpl.hpp>

#include <tests/data/kitchen_sink.hpp>

namespace {
namespace s = ajg::synth;

typedef s::default_traits<char>                                                 traits_type;
typedef s::engines::tmpl::engine<traits_type>                                   engine_type;

typedef s::templates::path_template<engine_type>                                path_template_type;
typedef s::templates::string_template<engine_type>                              string_template_type;

typedef engine_type::traits_type                                                traits_type;
typedef engine_type::context_type                                               context_type;
typedef engine_type::options_type                                               options_type;

typedef traits_type::char_type                                                  char_type;
typedef traits_type::string_type                                                string_type;

struct data_type : tests::data::kitchen_sink<engine_type> {};

AJG_SYNTH_TEST_GROUP_WITH_DATA("tmpl", data_type);

} // namespace

AJG_SYNTH_TEST_UNIT(sanity check) {
    string_template_type const t("");
    MUST_EQUAL(t.render_to_string(context), "");
}}}

AJG_SYNTH_TEST_UNIT(plain text) {
    string_template_type const t("ABC");
    MUST_EQUAL(t.render_to_string(context), "ABC");
}}}

AJG_SYNTH_TEST_UNIT(single tag) {
    string_template_type t("<TMPL_VAR foo>");
    MUST_EQUAL(t.render_to_string(context), "A");
}}}

AJG_SYNTH_TEST_UNIT(malformed tag) {
    MUST_THROW(s::parsing_error, string_template_type("<TMPL_ VAR foo>"));
}}}

AJG_SYNTH_TEST_UNIT(alternative tag) {
    string_template_type t("<!--TMPL_VAR foo -->");
    MUST_EQUAL(t.render_to_string(context), "A");
}}}

AJG_SYNTH_TEST_UNIT(shortcut variables) {
    string_template_type t("<TMPL_VAR foo><TMPL_VAR BAR><TMPL_VAR qUx>");
    MUST_EQUAL(t.render_to_string(context), "ABC");
}}}

AJG_SYNTH_TEST_UNIT(qualified variables) {
    string_template_type t("<TMPL_VAR NAME='foo'><TMPL_VAR NAME='BAR'><TMPL_VAR NAME='qUx'>");
    MUST_EQUAL(t.render_to_string(context), "ABC");
}}}

AJG_SYNTH_TEST_UNIT(default variables) {
    string_template_type t(
        "<TMPL_VAR foo>\n"
        "<TMPL_VAR foo DEFAULT=B>\n"
        "<TMPL_VAR DEFAULT=B foo>\n"
        "<TMPL_VAR non_extant>A\n"
        "<TMPL_VAR non_extant DEFAULT=A>\n"
        "<TMPL_VAR DEFAULT=A non_extant>\n");
    MUST_EQUAL(t.render_to_string(context), "A\nA\nA\nA\nA\nA\n");
}}}

AJG_SYNTH_TEST_UNIT(html tags) {
    string_template_type t("<foo>\nA foo <bar /> element.\n</foo>");
    MUST_EQUAL(t.render_to_string(context), t.str());
}}}

AJG_SYNTH_TEST_UNIT(comment tag) {
    string_template_type t("<TMPL_COMMENT> A comment </TMPL_COMMENT>");
    MUST_EQUAL(t.render_to_string(context), "");
}}}

AJG_SYNTH_TEST_UNIT(single quotes) {
    string_template_type t("A<TMPL_VAR NAME='bar'>C");
    MUST_EQUAL(t.render_to_string(context), "ABC");
}}}

AJG_SYNTH_TEST_UNIT(double quotes) {
    string_template_type t("A<TMPL_VAR NAME=\"bar\">C");
    MUST_EQUAL(t.render_to_string(context), "ABC");
}}}

AJG_SYNTH_TEST_UNIT(simple if tag) {
    string_template_type t(
        "<TMPL_IF true_var>Good</TMPL_IF>");
    MUST_EQUAL(t.render_to_string(context), "Good");
}}}

AJG_SYNTH_TEST_UNIT(invalid if tag) {
    string_template_type t("<TMPL_IF>Bad</TMPL_IF>");
    MUST_THROW(std::logic_error, t.render_to_string(context));
}}}

AJG_SYNTH_TEST_UNIT(compound if tag) {
    string_template_type t(
        "<TMPL_IF non_extant>Bad<TMPL_ELSE>Good</TMPL_IF> "
        "<TMPL_IF true_var>Good<TMPL_ELSE>Bad</TMPL_IF> "
        "<TMPL_IF false_var>Bad<TMPL_ELSE>Good</TMPL_IF>");
    MUST_EQUAL(t.render_to_string(context), "Good Good Good");
}}}

AJG_SYNTH_TEST_UNIT(simple unless tag) {
    string_template_type t(
        "<TMPL_UNLESS false_var>Good</TMPL_UNLESS>");
    MUST_EQUAL(t.render_to_string(context), "Good");
}}}

AJG_SYNTH_TEST_UNIT(compound unless tag) {
    string_template_type t(
        "<TMPL_UNLESS non_extant>Good<TMPL_ELSE>Bad</TMPL_UNLESS> "
        "<TMPL_UNLESS true_var>Bad<TMPL_ELSE>Good</TMPL_UNLESS> "
        "<TMPL_UNLESS false_var>Good<TMPL_ELSE>Bad</TMPL_UNLESS>");
    MUST_EQUAL(t.render_to_string(context), "Good Good Good");
}}}

AJG_SYNTH_TEST_UNIT(mismatched tags) {
    MUST_THROW(s::parsing_error, string_template_type("<TMPL_IF true_var>Bad</TMPL_UNLESS>"));
}}}

AJG_SYNTH_TEST_UNIT(loop tag) {
    string_template_type t("<TMPL_LOOP friends><TMPL_VAR name>: <TMPL_VAR age>; </TMPL_LOOP>");
    MUST_EQUAL(t.render_to_string(context), "joe: 23; bob: 55; lou: 41; ");
}}}

AJG_SYNTH_TEST_UNIT(empty loop element) {
    string_template_type t("<TMPL_LOOP friends></TMPL_LOOP>");
    MUST_EQUAL(t.render_to_string(context), "");
}}}

AJG_SYNTH_TEST_UNIT(loop __size__ variable) {
    string_template_type t("<TMPL_LOOP friends><TMPL_VAR __SIZE__> </TMPL_LOOP>");
    MUST_EQUAL(t.render_to_string(context), "3 3 3 ");
}}}

AJG_SYNTH_TEST_UNIT(loop __total__ variable) {
    string_template_type t("<TMPL_LOOP friends><TMPL_VAR __TOTAL__> </TMPL_LOOP>");
    MUST_EQUAL(t.render_to_string(context), "3 3 3 ");
}}}

AJG_SYNTH_TEST_UNIT(loop __first__ variable) {
    string_template_type t("<TMPL_LOOP friends><TMPL_VAR __FIRST__> </TMPL_LOOP>");
    MUST_EQUAL(t.render_to_string(context), "1 0 0 ");
}}}

AJG_SYNTH_TEST_UNIT(loop __last__ variable) {
    string_template_type t("<TMPL_LOOP friends><TMPL_VAR __LAST__> </TMPL_LOOP>");
    MUST_EQUAL(t.render_to_string(context), "0 0 1 ");
}}}

AJG_SYNTH_TEST_UNIT(loop __inner__ variable) {
    string_template_type t("<TMPL_LOOP friends><TMPL_VAR __INNER__> </TMPL_LOOP>");
    MUST_EQUAL(t.render_to_string(context), "0 1 0 ");
}}}

AJG_SYNTH_TEST_UNIT(loop __outer__ variable) {
    string_template_type t("<TMPL_LOOP friends><TMPL_VAR __OUTER__> </TMPL_LOOP>");
    MUST_EQUAL(t.render_to_string(context), "1 0 1 ");
}}}

AJG_SYNTH_TEST_UNIT(loop __odd__ variable) {
    string_template_type t("<TMPL_LOOP friends><TMPL_VAR __ODD__> </TMPL_LOOP>");
    MUST_EQUAL(t.render_to_string(context), "1 0 1 ");
}}}

AJG_SYNTH_TEST_UNIT(loop __even__ variable) {
    string_template_type t("<TMPL_LOOP friends><TMPL_VAR __EVEN__> </TMPL_LOOP>");
    MUST_EQUAL(t.render_to_string(context), "0 1 0 ");
}}}

AJG_SYNTH_TEST_UNIT(loop __counter__ variable) {
    string_template_type t("<TMPL_LOOP friends><TMPL_VAR __COUNTER__> </TMPL_LOOP>");
    MUST_EQUAL(t.render_to_string(context), "1 2 3 ");
}}}

AJG_SYNTH_TEST_UNIT(file template) {
    path_template_type t("tests/templates/tmpl/variables.tmpl");
    MUST_EQUAL(t.render_to_string(context), "foo: A\nbar: B\nqux: C\n");
}}}

AJG_SYNTH_TEST_UNIT(include tag) {
    path_template_type t("tests/templates/tmpl/example.tmpl");
    MUST_EQUAL(t.render_to_string(context),
        "============\nfoo: A\nbar: B\nqux: C\n|\nfoo: A\nbar: B\nqux: C\n\n============\n");
}}}
