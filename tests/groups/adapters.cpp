//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>

#include <boost/assign/list_of.hpp>

#include <ajg/testing.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/engines/null_engine.hpp>
#include <ajg/synth/templates/string_template.hpp>

namespace {

namespace s = ajg::synth;

typedef s::default_traits<char>                                                 traits_type;
typedef s::null_engine<traits_type>                                             engine_type;
typedef s::string_template<engine_type>                                         template_type;
typedef engine_type::context_type                                               context_type;
typedef engine_type::value_type                                                 value_type;
typedef traits_type::char_type                                                  char_type;
typedef traits_type::string_type                                                string_type;
typedef ajg::test_group<>                                                       group_type;

group_type group_object("adapter");

} // namespace


AJG_TESTING_BEGIN

unit_test(assignments) {
    using namespace std;
    using namespace boost;

    context_type context;

    string_type const X = traits_type::literal("this");
    string_type const Y = traits_type::literal("that");
    string_type const Two = traits_type::literal("2");
    vector<string_type> some_strings;
    some_strings.push_back(traits_type::literal("foo"));
    some_strings.push_back(traits_type::literal("bar"));
    some_strings.push_back(traits_type::literal("qux"));
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

    context[traits_type::literal("this_value")] = 175;
    context[traits_type::literal("max_value")] = 200;

    context[traits_type::literal("xml_var")] = traits_type::literal("<foo><bar qux='ha'>&</bar></foo>");
    context[traits_type::literal("a_few_lines")] = traits_type::literal("this\nhas\nlines");
    context[traits_type::literal("more_lines")] = traits_type::literal("this\nhas\nlines\n\r\nand\nsome\nmore");
    context[traits_type::literal("a_bin_string")] = traits_type::literal("this \f string \t contains \b binary \0 indeed");

    string_type const string_array[5] = { traits_type::literal("sa0"), traits_type::literal("sa1"), traits_type::literal("sa2"), traits_type::literal("sa3"), traits_type::literal("sa4") };
    string_type const tag_list[5] = { traits_type::literal("<these>"), traits_type::literal("<are>"), traits_type::literal("<tags />"), traits_type::literal("</are>"), traits_type::literal("</these>") };
    float const float_array[6] = { 1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f };

    /// float const* float_pointer = float_array;
    /// context[traits_type::literal("a_float_pointer")] = value(float_pointer, 6);

    scoped_ptr<vector<int> > scoped_(new vector<int>(3, 3));
    auto_ptr<vector<int> > auto_ptr_(new vector<int>(4, 4));

    context[traits_type::literal("X")] = traits_type::literal("this"); // X;
    context[traits_type::literal("Y")] = boost::ref(Y);
    context[traits_type::literal("Z")] = Two;
    context[traits_type::literal("some_strings")] = some_strings;
 // TODO: context[traits_type::literal("a_set"] = set<string_type, string_type>();
    context[traits_type::literal("an_int_vector")] = doodoo;
    context[traits_type::literal("a_shared_ptr")] = boost::shared_ptr<vector<int> >(new vector<int>(6, 6));
    context[traits_type::literal("a_scoped_ptr")] = scoped_;
    context[traits_type::literal("a_string_array")] = string_array;
    context[traits_type::literal("tag_list")] = tag_list;
    context[traits_type::literal("a_float_array")] = float_array;
    context[traits_type::literal("an_auto_ptr")] = auto_ptr_;
    context[traits_type::literal("a_char")] = char_type('z');
    context[traits_type::literal("an_int")] = 12;

    scoped_array<int> a_scoped_array(new int[5]);
    a_scoped_array[0] = 666;
    a_scoped_array[1] = 667;
    a_scoped_array[2] = 668;
    a_scoped_array[3] = 669;
    a_scoped_array[4] = 670;
    context[traits_type::literal("a_scoped_array")] = value_type(a_scoped_array, 5);

    map<int, int> a_map = assign::map_list_of(1, 2)(3, 4);
    context[traits_type::literal("a_deque")] = deque<char_type>();
    context[traits_type::literal("a_map")] = a_map;

    vector<vector<vector<int> > > _vectors;
    context[traits_type::literal("some_vectors")] = _vectors;
    context[traits_type::literal("shared_array")] = value_type(shared_array<string_type>(), 0);

    context[traits_type::literal("a_complex_float")] = complex<float>();
    context[traits_type::literal("a_pair")] = make_pair(1, 2.5);
    context[traits_type::literal("a_boost_array")] = boost::array<double, 2>();
    context[traits_type::literal("an_optional_float")] = optional<double>(4.5);

    // FIXME:
    // auto_ptr<vector<bool> > bools_(new vector<bool>(2, false));
    // vector<bool> *const bools = bools_.get();
    // context[traits_type::literal("a_pointer")] = bools;

    context[traits_type::literal("a_true")] = true;
    context[traits_type::literal("a_false")] = false;

    string_type sss = traits_type::literal("This should be a string.");
    char_type const *const ccc = sss.c_str();
    context[traits_type::literal("char_pointer")] = ccc;
}}}
