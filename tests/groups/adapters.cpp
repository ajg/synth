//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>

#include <boost/assign/list_of.hpp>

#include <ajg/synth/testing.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/engines/null.hpp>
#include <ajg/synth/templates/string_template.hpp>
#include <ajg/synth/detail/text.hpp>

namespace {

namespace s = ajg::synth;

typedef s::default_traits<char>                                                 traits_type;
typedef s::engines::null::engine<traits_type>                                   engine_type;
typedef s::templates::string_template<engine_type>                              template_type;

typedef engine_type::context_type                                               context_type;
typedef engine_type::value_type                                                 value_type;

typedef traits_type::char_type                                                  char_type;
typedef traits_type::string_type                                                string_type;
typedef s::detail::text<string_type>                                            text;

AJG_SYNTH_TEST_GROUP("adapter");

} // namespace

AJG_SYNTH_TEST_UNIT(assignments) {
    using namespace std;   // TODO: Remove.
    using namespace boost; // TODO: Remove.

    context_type context((std::map<string_type, value_type>()));

    string_type const X = text::literal("this");
    string_type const Y = text::literal("that");
    string_type const Two = text::literal("2");
    vector<string_type> some_strings;
    some_strings.push_back(text::literal("foo"));
    some_strings.push_back(text::literal("bar"));
    some_strings.push_back(text::literal("qux"));
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

    context.set(text::literal("this_value"), 175);
    context.set(text::literal("max_value"), 200);

    context.set(text::literal("xml_var"), text::literal("<foo><bar qux='ha'>&</bar></foo>"));
    context.set(text::literal("a_few_lines"), text::literal("this\nhas\nlines"));
    context.set(text::literal("more_lines"), text::literal("this\nhas\nlines\n\r\nand\nsome\nmore"));
    context.set(text::literal("a_bin_string"), text::literal("this \f string \t contains \b binary \0 indeed"));

    string_type const string_array[5] = { text::literal("sa0"), text::literal("sa1"), text::literal("sa2"), text::literal("sa3"), text::literal("sa4") };
    string_type const tag_list[5] = { text::literal("<these>"), text::literal("<are>"), text::literal("<tags />"), text::literal("</are>"), text::literal("</these>") };
    float const float_array[6] = { 1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f };

    /// float const* float_pointer = float_array;
    /// context.set(text::literal("a_float_pointer"), value(float_pointer, 6));

    scoped_ptr<vector<int> > scoped_(new vector<int>(3, 3));
    auto_ptr<vector<int> > auto_ptr_(new vector<int>(4, 4));

    context.set(text::literal("X"), text::literal("this")); // X;
    context.set(text::literal("Y"), boost::ref(Y));
    context.set(text::literal("Z"), Two);
    context.set(text::literal("some_strings"), some_strings);
    // TODO: context.set(text::literal("a_set", set<string_type, string_type>());
    context.set(text::literal("an_int_vector"), doodoo);
    context.set(text::literal("a_shared_ptr"), boost::shared_ptr<vector<int> >(new vector<int>(6, 6)));
    context.set(text::literal("a_scoped_ptr"), scoped_);
    context.set(text::literal("a_string_array"), string_array);
    context.set(text::literal("tag_list"), tag_list);
    context.set(text::literal("a_float_array"), float_array);
    context.set(text::literal("an_auto_ptr"), auto_ptr_);
    context.set(text::literal("a_char"), char_type('z'));
    context.set(text::literal("an_int"), 12);

    scoped_array<int> a_scoped_array(new int[5]);
    a_scoped_array[0] = 666;
    a_scoped_array[1] = 667;
    a_scoped_array[2] = 668;
    a_scoped_array[3] = 669;
    a_scoped_array[4] = 670;
    context.set(text::literal("a_scoped_array"), value_type(a_scoped_array, 5));

    map<int, int> a_map = assign::map_list_of(1, 2)(3, 4);
    context.set(text::literal("a_deque"), deque<char_type>());
    context.set(text::literal("a_map"), a_map);

    vector<vector<vector<int> > > _vectors;
    context.set(text::literal("some_vectors"), _vectors);
    context.set(text::literal("shared_array"), value_type(shared_array<string_type>(), 0));

    context.set(text::literal("a_complex_float"), complex<float>());
    context.set(text::literal("a_pair"), make_pair(1, 2.5));
    context.set(text::literal("a_boost_array"), boost::array<double, 2>());
    context.set(text::literal("an_optional_float"), optional<double>(4.5));

    // FIXME:
    // auto_ptr<vector<bool> > bools_(new vector<bool>(2, false));
    // vector<bool> *const bools = bools_.get();
    // context.set(text::literal("a_pointer"), bools);

    context.set(text::literal("a_true"), true);
    context.set(text::literal("a_false"), false);

    string_type sss = text::literal("This should be a string.");
    char_type const *const ccc = sss.c_str();
    context.set(text::literal("char_pointer"), ccc);
}}}
