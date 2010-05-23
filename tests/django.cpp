
//  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <chemical/testing.hpp>
#include <chemical/synthesis/engines/django.hpp>

typedef char char_t;
namespace cs = chemical::synthesis;
typedef cs::django::engine<> engine_type;
typedef cs::file_template<char_t, engine_type> file_template;
typedef cs::string_template<char_t, engine_type> string_template;

typedef chemical::test_group<> group_type;
group_type group_object("django tests");
CHEMICAL_TESTING_BEGIN


unit_test(addition) {
    string_template t("");
    string_template::context_type context;
    // ensure_equals(t.render_to_string(context), "ABC");
}}}

unit_test(subtraction) {
    ensure_equals("2-2=?", 2-2, 0);
}}}


