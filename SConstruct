##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

import os
import subprocess
from distutils import sysconfig

# TODO: Add environments with '-std=c++11' and eventually '-std=c++14'

debug = int(ARGUMENTS.get('debug', 0))

cxx = ARGUMENTS.get('CXX', os.environ.get('CXX', 'c++'))
cxx_version = subprocess.check_output([cxx, '--version'])

env = Environment(
    CXX      = cxx,
    CPPPATH  = ['.'],
    CPPFLAGS = [
        # TODO: '-Wall',
        # TODO: '-Wextra',
        # TODO: '-pedantic',
        '-ftemplate-depth=256',
        '-Wno-unsequenced',
        '-Wno-unused-function',
        '-Wno-unused-value',
    ],
)

if 'clang' in cxx_version:
    env.Append(CPPFLAGS = ['-ferror-limit=1'])
elif 'g++' in cxx_version:
    env.Append(CPPFLAGS = ['-fmax-errors=1'])

if debug:
    env.Append(CPPFLAGS = ['-g'])
else:
    env.Append(CPPFLAGS = ['-O3', '-DNDEBUG'])

test = env.Clone()
test.Program(
    target = 'test',
    source = ['tests/test.cpp'] + Glob('tests/*_tests.cpp'),
)

command_line = env.Clone()
command_line.Program(
    target = 'synth',
    source = ['ajg/synth/bindings/command_line/program.cpp'],
    LIBS   = ['boost_program_options'],
)

python = env.Clone()
python.LoadableModule(
    target    = 'python-synth.so',
    source    = ['ajg/synth/bindings/python/module.cpp'],
    CPPPATH   = ['.', sysconfig.get_python_inc()],
    LIBPATH   = [sysconfig.get_config_var('LIBDIR')],
    LIBPREFIX = '',
    LIBS      = ['boost_python', 'python' + sysconfig.get_config_var('VERSION')],
)
