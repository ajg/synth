//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <cstdlib>

#include <ajg/testing.hpp>

int main(int const argc, char const *const argv[]) {
    bool success = true;
    ajg::test_suite suite("synth");

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
