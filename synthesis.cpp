//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <cstdlib>

#include <ajg/testing.hpp>

int main(int const argc, char const *const argv[]) {
    bool success = true;
    ajg::test_suite suite("synthesis");

    if (argc <= 1) {
        // Run entire suite.
        success = suite.run();
    }
    else {
        // Run specific groups.
        for (int i = 1; i < argc; ++i) {
            success = success && suite.run(argv[i]);
        }
    }

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}




#if 0

#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>

#include <boost/assign/list_of.hpp>


    string_type const X = T("this");
    string_type const Y = T("that");
    string_type const Two = T("2");
    vector<string_type> some_strings;
    some_strings.push_back(T("foo"));
    some_strings.push_back(T("bar"));
    some_strings.push_back(T("qux"));
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

    context[T("this_value")] = 175;
    context[T("max_value")] = 200;

    context[T("xml_var")] = T("<foo><bar qux='ha'>&</bar></foo>");
    context[T("a_few_lines")] = T("this\nhas\nlines");
    context[T("more_lines")] = T("this\nhas\nlines\n\r\nand\nsome\nmore");
    context[T("a_bin_string")] = T("this \f string \t contains \b binary \0 indeed");

    string_type const string_array[5] = { T("sa0"), T("sa1"), T("sa2"), T("sa3"), T("sa4") };
    string_type const tag_list[5] = { T("<these>"), T("<are>"), T("<tags />"), T("</are>"), T("</these>") };
    float const float_array[6] = { 1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f };

    /// float const* float_pointer = float_array;
    /// context[T("a_float_pointer")] = value(float_pointer, 6);

    scoped_ptr<vector<int> > scoped_(new vector<int>(3, 3));
    auto_ptr<vector<int> > auto_ptr_(new vector<int>(4, 4));

    context[T("X")] = T("this"); // X;
    context[T("Y")] = boost::ref(Y);
    context[T("Z")] = Two;
    context[T("some_strings")] = some_strings;
 // TODO: context[T("a_set"] = set<string_type, string_type>();
    context[T("an_int_vector")] = doodoo;
    context[T("a_shared_ptr")] = boost::shared_ptr<vector<int> >(new vector<int>(6, 6));
    context[T("a_scoped_ptr")] = scoped_;
    context[T("a_string_array")] = string_array;
    context[T("tag_list")] = tag_list;
    context[T("a_float_array")] = float_array;
    context[T("an_auto_ptr")] = auto_ptr_;
    context[T("a_char")] = T('z');
    context[T("an_int")] = 12;

    scoped_array<int> a_scoped_array(new int[5]);
    a_scoped_array[0] = 666;
    a_scoped_array[1] = 667;
    a_scoped_array[2] = 668;
    a_scoped_array[3] = 669;
    a_scoped_array[4] = 670;
    context[T("a_scoped_array")] = value(a_scoped_array, 5);

    map<int, int> a_map = assign::map_list_of(1, 2)(3, 4);
    context[T("a_deque")] = deque<char_t>();
    context[T("a_map")] = a_map;

    vector<vector<vector<int> > > _vectors;
    context[T("some_vectors")] = _vectors;
    context[T("shared_array")] = value(shared_array<string_type>(), 0);

    context[T("a_complex_float")] = complex<float>();
    context[T("a_pair")] = make_pair(1, 2.5);
    context[T("a_boost_array")] = array<double, 2>();
    context[T("an_optional_float")] = optional<double>(4.5);

    auto_ptr<vector<bool> > bools_(new vector<bool>(2, false));
    vector<bool>* const bools = bools_.get();
    context[T("a_pointer")] = bools;

    context[T("a_true")] = true;
    context[T("a_false")] = false;

    char_t const *const ccc = T("This should be a string.");
    context[T("char_pointer")] = ccc;


    template_type::array_type
        list1 = assign::list_of<value>(T("Lawrence"))(T("Topeka")),
        list2 = assign::list_of<value>(T("Kansas"))(list1)(T("Illinois1"))(T("Illinois2")),
        list3 = assign::list_of<value>(T("States"))(list2),
        list4 = assign::list_of<value>(T("Parent"))(list3);
    context[T("places")] = list4;

#endif
