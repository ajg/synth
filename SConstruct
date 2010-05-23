
##  (C) Copyright 2010 Alvy J. Guty <plus {dot} ajg {at} gmail {dot} com>
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

env = Environment(CPPPATH  = ['.'],
                  CPPFLAGS = ['-Wall', '-pedantic'])
                  # TODO: '-Wextra'

synthesis = env.Clone()
synthesis.Program(target = "synthesis", source =
    ["synthesis.cpp", "tests/ssi.cpp",
                      "tests/tmpl.cpp",
                      "tests/django.cpp"])
