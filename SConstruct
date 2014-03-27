##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

import os
import re
import subprocess
from distutils import sysconfig

debug = int(ARGUMENTS.get('debug', 0))
group = str(ARGUMENTS.get('group', ''))

def construct():
    cpp = ARGUMENTS.get('cpp', os.environ.get('CXX', 'c++'))
    env = make_environment(cpp)
    create_targets(env)

def make_environment(cpp):
    return Environment(
        CXX      = cpp,
        CPPPATH  = ['.'],
        CPPFLAGS = [
            # TODO: '-Wall',
            # TODO: '-Wextra',
            # TODO: '-pedantic',
            '-Wno-unused-function',
            '-Wno-unused-value',
        ] + get_conditional_cpp_flags(cpp),
    )

def create_targets(env):
    test = env.Clone()
    test.Program(
        target = 'test',
        source = ['tests/test.cpp'] + get_test_sources(),
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

def get_test_sources():
    if group:
        return ['tests/%s_tests.cpp' % group]
    else:
        return Glob('tests/*_tests.cpp')

def get_conditional_cpp_flags(cpp):
    cpp_flags = []
    cpp_version = subprocess.check_output([cpp, '--version'])
    cpp_template_depth = 1024

    if 'clang' in cpp_version:
        cpp_flags += ['-Wno-unsequenced']
        cpp_flags += ['-ferror-limit=1']
        cpp_flags += ['-ftemplate-backtrace-limit=1']
        cpp_flags += ['-ftemplate-depth=' + str(cpp_template_depth)]
        cpp_flags += ['-DTEMPLATE_DEPTH=' + str(cpp_template_depth)]

    elif 'g++' in cpp_version:
        cpp_flags += ['-Wno-sequence-point']
        cpp_flags += ['-Wfatal-errors']

        triple = re.search(r'\s(\d+)[.](\d+)[.](\d+)\s', cpp_version)
        if triple:
            major = int(triple.group(1))
            minor = int(triple.group(2))
            patch = int(triple.group(3))
            gcc_version = (major, minor, patch)

            if gcc_version < (4, 5):
                cpp_flags += ['-ftemplate-depth-' + str(cpp_template_depth)] # Note the dash
                cpp_flags += ['-DTEMPLATE_DEPTH=' + str(cpp_template_depth)]
            else:
                cpp_flags += ['-ftemplate-depth=' + str(cpp_template_depth)]
                cpp_flags += ['-DTEMPLATE_DEPTH=' + str(cpp_template_depth)]

            if gcc_version >= (4, 8):
                cpp_flags += ['-fmax-errors=1']
                cpp_flags += ['-ftemplate-backtrace-limit=1']

    if debug:
        cpp_flags += ['-g']
    else:
        cpp_flags += ['-O3', '-DNDEBUG']

    return cpp_flags

construct()
