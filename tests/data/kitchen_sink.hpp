//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef TESTS_DATA_KITCHEN_SINK_HPP_INCLUDED
#define TESTS_DATA_KITCHEN_SINK_HPP_INCLUDED

#include <map>
#include <list>
#include <deque>
#include <string>
#include <vector>

#include <boost/assign/list_of.hpp>

#include <ajg/synth/detail/text.hpp>

namespace tests {
namespace data {
namespace {
using boost::assign::list_of;
}

template <class Engine>
struct kitchen_sink {
  public:

    typedef kitchen_sink                                                        kitchen_sink_type;
    typedef Engine                                                              engine_type;

    typedef typename engine_type::value_type                                    value_type;
    typedef typename engine_type::traits_type                                   traits_type;
    typedef typename engine_type::context_type                                  context_type;
    typedef typename engine_type::options_type                                  options_type;

    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::date_type                                     date_type;
    typedef typename traits_type::time_type                                     time_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::duration_type                                 duration_type;

  protected:

    typedef ajg::synth::detail::text<string_type>                               text;

  public:

    kitchen_sink() : context(std::map<string_type, value_type>()) {
        context.set(text::literal("foo"), "A");
        context.set(text::literal("bar"), "B");
        context.set(text::literal("qux"), "C");

        context.set(text::literal("true_var"), true);
        context.set(text::literal("false_var"), false);

        std::map<string_type, string_type> joe, bob, lou;
        joe[text::literal("name")] = "joe";
        joe[text::literal("age")]  = "23";
        bob[text::literal("name")] = "bob";
        bob[text::literal("age")]  = "55";
        lou[text::literal("name")] = "lou";
        lou[text::literal("age")]  = "41";
        friends[0] = joe;
        friends[1] = bob;
        friends[2] = lou;

        context.set("friends", friends);

        time_type const past(traits_type::to_date(2002, 1, 10), traits_type::to_duration(1, 2, 3));
        time_type const future(traits_type::to_date(2202, 2, 11), traits_type::to_duration(3, 2, 1));

        context.set(text::literal("past"),        past);
        context.set(text::literal("before_past"), past - traits_type::to_duration(36, 0, 0));
        context.set(text::literal("after_past"),  past + traits_type::to_duration(1200, 20, 0));
        context.set(text::literal("future"),      future);

        std::map<string_type, string_type> mumbai, calcutta, nyc, chicago, tokyo;
        mumbai[text::literal("name")]       = "Mumbai";
        mumbai[text::literal("population")] = "19,000,000";
        mumbai[text::literal("country")]    = "India";
        calcutta[text::literal("name")]       = "Calcutta";
        calcutta[text::literal("population")] = "15,000,000";
        calcutta[text::literal("country")]    = "India";
        nyc[text::literal("name")]       = "New York";
        nyc[text::literal("population")] = "20,000,000";
        nyc[text::literal("country")]    = "USA";
        chicago[text::literal("name")]       = "Chicago";
        chicago[text::literal("population")] = "7,000,000";
        chicago[text::literal("country")]    = "USA";
        tokyo[text::literal("name")]       = "Tokyo";
        tokyo[text::literal("population")] = "33,000,000";
        tokyo[text::literal("country")]    = "Japan";

        cities[0] = mumbai;
        cities[1] = calcutta;
        cities[2] = nyc;
        cities[3] = chicago;
        cities[4] = tokyo;
        context.set(text::literal("cities"), cities);

        std::deque<value_type> const list1 = list_of<value_type>(text::literal("Lawrence"))(text::literal("Topeka"));
        std::deque<value_type> const list2 = list_of<value_type>(text::literal("Kansas"))(list1)(text::literal("Illinois1"))(text::literal("Illinois2"));
        std::deque<value_type> const list3 = list_of<value_type>(text::literal("States"))(list2);
        std::deque<value_type> const list4 = list_of<value_type>(text::literal("Parent"))(list3);
        context.set(text::literal("places"), list4);
        context.set(text::literal("csrf_token"), "ABCDEF123456");
        context.set(text::literal("xml_var"), "<foo><bar><qux /></bar></foo>");
        context.set(text::literal("haiku"), "Haikus are easy,\nBut sometimes they don't make sense.\nRefrigerator.\n");

        states[text::literal("CA")] = text::literal("California");
        states[text::literal("FL")] = text::literal("Florida");
        states[text::literal("NY")] = text::literal("New York");
        context.set(text::literal("states"), states);

        numbers.push_back(1);
        numbers.push_back(2);
        numbers.push_back(3);
        numbers.push_back(4);
        numbers.push_back(5);
        numbers.push_back(6);
        numbers.push_back(7);
        numbers.push_back(8);
        numbers.push_back(9);
        context.set(text::literal("numbers"), numbers);

        heterogenous.push_back(42);
        heterogenous.push_back(42);
        heterogenous.push_back(text::literal("foo"));
        heterogenous.push_back(text::literal("foo"));
        context.set(text::literal("heterogenous"), heterogenous);

        tags.push_front("<Z>");
        tags.push_front("<Y>");
        tags.push_front("<X>");
        context.set(text::literal("tags"), tags);

        context.set(text::literal("variable_path"), text::literal("tests/templates/django/variables.tpl"));
    }

    context_type                        context;
    options_type                        options;
    std::map<string_type, std::string>  friends[3];
    std::map<string_type, std::string>  cities[5];
    std::map<string_type, string_type>  states;
    std::vector<int>                    numbers;
    std::deque<char const*>             tags;
    std::list<value_type>               heterogenous;
}; // kitchen_sink

}} // namespace tests::data

#endif // TESTS_DATA_KITCHEN_SINK_HPP_INCLUDED
