//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef TESTS_DATA_KITCHEN_SINK_HPP_INCLUDED
#define TESTS_DATA_KITCHEN_SINK_HPP_INCLUDED

#include <map>
#include <string>

#include <boost/mpl/void.hpp>
#include <boost/assign/list_of.hpp>

#include <ajg/synth/engines/detail.hpp>

namespace tests {
namespace data {

using boost::assign::list_of;
using ajg::synth::detail::text;

template <class Context, class Traits, class Options = boost::mpl::void_>
struct kitchen_sink {
    typedef Context                                             context_type;
    typedef Traits                                              traits_type;
    typedef Options                                             options_type;
    typedef typename traits_type::string_type                   string_type;
    typedef typename traits_type::sequence_type                 sequence_type;
    typedef typename traits_type::date_type                     date_type;
    typedef typename traits_type::datetime_type                 datetime_type;
    typedef typename traits_type::duration_type                 duration_type;
    typedef typename context_type::mapped_type                  value_type;

    kitchen_sink() {
        context[text("foo")] = "A";
        context[text("bar")] = "B";
        context[text("qux")] = "C";

        context["true_var"] = true;
        context["false_var"] = false;

        std::map<string_type, string_type> joe, bob, lou;
        joe[text("name")] = "joe";
        joe[text("age")]  = "23";
        bob[text("name")] = "bob";
        bob[text("age")]  = "55";
        lou[text("name")] = "lou";
        lou[text("age")]  = "41";
        friends[0] = joe;
        friends[1] = bob;
        friends[2] = lou;

        context["friends"] = friends;

        datetime_type const past(date_type(2002, 1, 10), duration_type(1, 2, 3));
        datetime_type const future(date_type(2202, 2, 11), duration_type(3, 2, 1));

        context[text("past")]        = past;
        context[text("before_past")] = past - duration_type(36, 0, 0);
        context[text("after_past")]  = past + duration_type(1200, 20, 0);
        context[text("future")]      = future;

        std::map<string_type, string_type> mumbai, calcutta, nyc, chicago, tokyo;
        mumbai[text("name")]       = "Mumbai";
        mumbai[text("population")] = "19,000,000";
        mumbai[text("country")]    = "India";
        calcutta[text("name")]       = "Calcutta";
        calcutta[text("population")] = "15,000,000";
        calcutta[text("country")]    = "India";
        nyc[text("name")]       = "New York";
        nyc[text("population")] = "20,000,000";
        nyc[text("country")]    = "USA";
        chicago[text("name")]       = "Chicago";
        chicago[text("population")] = "7,000,000";
        chicago[text("country")]    = "USA";
        tokyo[text("name")]       = "Tokyo";
        tokyo[text("population")] = "33,000,000";
        tokyo[text("country")]    = "Japan";

        cities[0] = mumbai;
        cities[1] = calcutta;
        cities[2] = nyc;
        cities[3] = chicago;
        cities[4] = tokyo;
        context[text("cities")] = cities;

        sequence_type
            list1 = list_of<value_type>(text("Lawrence"))(text("Topeka")),
            list2 = list_of<value_type>(text("Kansas"))(list1)(text("Illinois1"))(text("Illinois2")),
            list3 = list_of<value_type>(text("States"))(list2),
            list4 = list_of<value_type>(text("Parent"))(list3);
        context[text("places")] = list4;

        context[text("csrf_token")] = "ABCDEF123456";

        context[text("xml_var")] = "<foo><bar><qux /></bar></foo>";

        std::vector<int> numbers;
        numbers.push_back(1);
        numbers.push_back(2);
        numbers.push_back(3);
        numbers.push_back(4);
        numbers.push_back(5);
        numbers.push_back(6);
        numbers.push_back(7);
        numbers.push_back(8);
        numbers.push_back(9);
        context[text("numbers")] = numbers;
    }

    Context context;
    Options options;
    std::map<std::string, std::string> friends[3];
    std::map<std::string, std::string> cities[5];
}; // kitchen_sink

}} // namespace tests::data

#endif // TESTS_DATA_KITCHEN_SINK_HPP_INCLUDED
