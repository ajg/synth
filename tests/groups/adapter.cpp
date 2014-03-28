//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>

#include <boost/assign/list_of.hpp>

#include <ajg/testing.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/engines/null_engine.hpp>

namespace {

namespace s = ajg::synth;

typedef char                                       char_type;
typedef s::null_engine                             engine_type;
typedef s::string_template<char_type, engine_type> template_type;
typedef template_type::context_type                context_type;
typedef template_type::string_type                 string_type;
typedef template_type::value_type                  value_type;
typedef ajg::test_group<>                          group_type;

group_type group_object("adapter tests");

} // namespace


AJG_TESTING_BEGIN

#define CHAR   // TODO: Widen/narrow as necessary.
#define STRING ajg::synth::detail::text

unit_test(assignments) {
    using namespace std;
    using namespace boost;

    context_type context;

    string_type const X = STRING("this");
    string_type const Y = STRING("that");
    string_type const Two = STRING("2");
    vector<string_type> some_strings;
    some_strings.push_back(STRING("foo"));
    some_strings.push_back(STRING("bar"));
    some_strings.push_back(STRING("qux"));
    vector<int> doodoo;
    doodoo.push_back(1);
    doodoo.push_back(2);
    doodoo.push_back(3);
    doodoo.push_back(4);
    doodoo.push_back(5);
    doodoo.push_back(6);
    doodoo.push_back(7);
    doodoo.push_back(8);
    doodoo.push_back(9);

    context[STRING("this_value")] = 175;
    context[STRING("max_value")] = 200;

    context[STRING("xml_var")] = STRING("<foo><bar qux='ha'>&</bar></foo>");
    context[STRING("a_few_lines")] = STRING("this\nhas\nlines");
    context[STRING("more_lines")] = STRING("this\nhas\nlines\n\r\nand\nsome\nmore");
    context[STRING("a_bin_string")] = STRING("this \f string \t contains \b binary \0 indeed");

    string_type const string_array[5] = { STRING("sa0"), STRING("sa1"), STRING("sa2"), STRING("sa3"), STRING("sa4") };
    string_type const tag_list[5] = { STRING("<these>"), STRING("<are>"), STRING("<tags />"), STRING("</are>"), STRING("</these>") };
    float const float_array[6] = { 1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f };

    /// float const* float_pointer = float_array;
    /// context[STRING("a_float_pointer")] = value(float_pointer, 6);

    scoped_ptr<vector<int> > scoped_(new vector<int>(3, 3));
    auto_ptr<vector<int> > auto_ptr_(new vector<int>(4, 4));

    context[STRING("X")] = STRING("this"); // X;
    context[STRING("Y")] = boost::ref(Y);
    context[STRING("Z")] = Two;
    context[STRING("some_strings")] = some_strings;
 // TODO: context[STRING("a_set"] = set<string_type, string_type>();
    context[STRING("an_int_vector")] = doodoo;
    context[STRING("a_shared_ptr")] = boost::shared_ptr<vector<int> >(new vector<int>(6, 6));
    context[STRING("a_scoped_ptr")] = scoped_;
    context[STRING("a_string_array")] = string_array;
    context[STRING("tag_list")] = tag_list;
    context[STRING("a_float_array")] = float_array;
    context[STRING("an_auto_ptr")] = auto_ptr_;
    context[STRING("a_char")] = CHAR('z');
    context[STRING("an_int")] = 12;

    scoped_array<int> a_scoped_array(new int[5]);
    a_scoped_array[0] = 666;
    a_scoped_array[1] = 667;
    a_scoped_array[2] = 668;
    a_scoped_array[3] = 669;
    a_scoped_array[4] = 670;
    context[STRING("a_scoped_array")] = value_type(a_scoped_array, 5);

    map<int, int> a_map = assign::map_list_of(1, 2)(3, 4);
    context[STRING("a_deque")] = deque<char_type>();
    context[STRING("a_map")] = a_map;

    vector<vector<vector<int> > > _vectors;
    context[STRING("some_vectors")] = _vectors;
    context[STRING("shared_array")] = value_type(shared_array<string_type>(), 0);

    context[STRING("a_complex_float")] = complex<float>();
    context[STRING("a_pair")] = make_pair(1, 2.5);
    context[STRING("a_boost_array")] = boost::array<double, 2>();
    context[STRING("an_optional_float")] = optional<double>(4.5);

    // FIXME:
    // auto_ptr<vector<bool> > bools_(new vector<bool>(2, false));
    // vector<bool> *const bools = bools_.get();
    // context[STRING("a_pointer")] = bools;

    context[STRING("a_true")] = true;
    context[STRING("a_false")] = false;

    string_type sss = STRING("This should be a string.");
    char_type const *const ccc = sss.c_str();
    context[STRING("char_pointer")] = ccc;
}}}
