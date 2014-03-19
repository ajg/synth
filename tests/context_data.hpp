//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef TESTS_CONTEXT_DATA_HPP_INCLUDED
#define TESTS_CONTEXT_DATA_HPP_INCLUDED

#include <map>
#include <string>

#include <boost/mpl/void.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace tests {

template <class Context, class Options = boost::mpl::void_>
struct context_data {
    context_data() {
        context["foo"] = "A";
        context["bar"] = "B";
        context["qux"] = "C";

        context["true_var"] = true;
        context["false_var"] = false;

        std::map<std::string, std::string> joe, bob, lou;
        joe["name"] = "joe";
        joe["age"]  = "23";
        bob["name"] = "bob";
        bob["age"]  = "55";
        lou["name"] = "lou";
        lou["age"]  = "41";
        friends[0] = joe;
        friends[1] = bob;
        friends[2] = lou;

        context["friends"] = friends;

        using namespace boost::gregorian;
        using namespace boost::posix_time;

        ptime const past(date(2002, Jan, 10), time_duration(1, 2, 3));
        ptime const future(date(2202, Feb, 11), time_duration(3, 2, 1));

        context["past"]        = past;
        context["before_past"] = past - hours(36);
        context["after_past"]  = past + hours(1200) + minutes(20);
        context["future"]      = future;

        std::map<std::string, std::string> mumbai, calcutta, nyc, chicago, tokyo;
        mumbai["name"]       = "Mumbai";
        mumbai["population"] = "19,000,000";
        mumbai["country"]    = "India";
        calcutta["name"]       = "Calcutta";
        calcutta["population"] = "15,000,000";
        calcutta["country"]    = "India";
        nyc["name"]       = "New York";
        nyc["population"] = "20,000,000";
        nyc["country"]    = "USA";
        chicago["name"]       = "Chicago";
        chicago["population"] = "7,000,000";
        chicago["country"]    = "USA";
        tokyo["name"]       = "Tokyo";
        tokyo["population"] = "33,000,000";
        tokyo["country"]    = "Japan";

        cities[0] = mumbai;
        cities[1] = calcutta;
        cities[2] = nyc;
        cities[3] = chicago;
        cities[4] = tokyo;
        context["cities"] = cities;
    }

    Context context;
    Options options;
    std::map<std::string, std::string> friends[3];
    std::map<std::string, std::string> cities[5];
};

} // namespace tests

#endif // TESTS_CONTEXT_DATA_HPP_INCLUDED
