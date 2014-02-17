##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

import os
import subprocess
from distutils import sysconfig

# TODO: Add environments with '-std=c++11' and eventually '-std=c++14'

cxx = ARGUMENTS.get('CXX', os.environ.get('CXX', 'c++'))
cxx_version = subprocess.check_output([cxx, '--version'])
error_limit = ''

if 'clang' in cxx_version:
    error_limit = '-ferror-limit=1'
elif 'g++' in cxx_version:
    error_limit = '-fmax-errors=1'

env = Environment(
    CXX      = cxx,
    CPPPATH  = ['.'],
    CPPFLAGS = [
        # TODO: '-Wall',
        # TODO: '-Wextra',
        # TODO: '-pedantic',
        error_limit,
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

python_synthesis = env.Clone()
python_synthesis.LoadableModule(
    target    = 'synthesis.so',
    source    = ['ajg/synthesis/bindings/python/module.cpp'],
    CPPPATH   = ['.', sysconfig.get_python_inc()],
    LIBPATH   = [sysconfig.get_config_var('LIBDIR')],
    LIBPREFIX = '',
    LIBS      = [
        'boost_python',
        'python' + sysconfig.get_config_var('VERSION'),
    ],
)
