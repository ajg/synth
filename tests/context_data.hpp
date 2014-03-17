//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef TESTS_CONTEXT_DATA_HPP_INCLUDED
#define TESTS_CONTEXT_DATA_HPP_INCLUDED

#include <map>
#include <string>

#include <boost/mpl/void.hpp>

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
    }

    Context context;
    Options options;
    std::map<std::string, std::string> friends[3];
};

} // namespace tests

#endif // TESTS_CONTEXT_DATA_HPP_INCLUDED
