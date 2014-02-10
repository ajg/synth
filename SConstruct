##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

# TODO: Add an environment with '-std=c++11',

env = Environment(
    CPPPATH  = ['.'],
    CPPFLAGS = [
        # TODO: '-Wall',
        # TODO: '-Wextra',
        # TODO: '-pedantic',
        # NOTE: clang-only '-ferror-limit=1',
        '-ftemplate-depth=256',
        '-Wno-unused-function',
        '-Wno-unsequenced',
    ],
)

test = env.Clone()
test.Program(
    target = 'test',
    source = [
        'test.cpp',
        'tests/ssi.cpp',
        'tests/tmpl.cpp',
        'tests/django.cpp',
    ],
)
