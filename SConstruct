##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

import os
import re
import subprocess
import sys
from distutils import sysconfig

debug = int(ARGUMENTS.get('debug', 0))
group = str(ARGUMENTS.get('group', ''))

def run():
    cxx = ARGUMENTS.get('CXX', os.environ.get('CXX', 'c++'))
    env = make_environment(cxx)
    create_targets(env)

def make_environment(cxx):
    return Environment(
        CXX      = cxx,
        CPPPATH  = ['.'],
        CPPFLAGS = get_cpp_flags(cxx),
    )

def create_targets(env):
    test_harness = env.Clone()
    test_harness.Program(
        target = 'test',
        source = ['tests/harness.cpp'] + find_test_sources(),
    )

    command_line_tool = env.Clone()
    command_line_tool.Program(
        target = 'synth',
        source = ['ajg/synth/bindings/command_line/tool.cpp'],
        LIBS   = ['boost_program_options'],
    )

    python_module = env.Clone()
    python_module.LoadableModule(
        target    = 'python-synth.so',
        source    = ['ajg/synth/bindings/python/module.cpp'],
        CPPPATH   = ['.', sysconfig.get_python_inc()],
        LIBPATH   = [sysconfig.get_config_var('LIBDIR')],
        LIBPREFIX = '',
        LIBS      = ['boost_python', 'python' + sysconfig.get_config_var('VERSION')],
    )

    return [test_harness, command_line_tool, python_module]

def find_test_sources():
    if group:
        return ['tests/groups/%s.cpp' % group]
    else:
        return Glob('tests/*.cpp')

def find_cxx_version(cxx):
    try:
        return subprocess.check_output([cxx, '--version'])
    except OSError as e:
        sys.exit('Unable to find compiler (%s) version: ' % cxx + e.strerror)

def get_cpp_flags(cxx):
    cpp_flags = [
        # TODO: '-Wall',
        # TODO: '-Wextra',
        # TODO: '-pedantic',
        '-Wno-unused-function',
        '-Wno-unused-value',
    ]

    cxx_version = find_cxx_version(cxx)
    cxx_template_depth = 512

    if 'clang' in cxx_version:
        cpp_flags += ['-Wno-unsequenced']
        cpp_flags += ['-ferror-limit=1']
        cpp_flags += ['-ftemplate-backtrace-limit=1']
        cpp_flags += ['-ftemplate-depth=' + str(cxx_template_depth)]
        cpp_flags += ['-DTEMPLATE_DEPTH=' + str(cxx_template_depth)]

    elif 'g++' in cxx_version:
        cpp_flags += ['-Wno-sequence-point']
        cpp_flags += ['-Wfatal-errors']

        triple = re.search(r'\s(\d+)[.](\d+)[.](\d+)\s', cxx_version)
        if triple:
            major = int(triple.group(1))
            minor = int(triple.group(2))
            patch = int(triple.group(3))
            gcc_version = (major, minor, patch)

            if gcc_version < (4, 5):
                cpp_flags += ['-ftemplate-depth-' + str(cxx_template_depth)] # Note the dash
                cpp_flags += ['-DTEMPLATE_DEPTH=' + str(cxx_template_depth)]
            else:
                cpp_flags += ['-ftemplate-depth=' + str(cxx_template_depth)]
                cpp_flags += ['-DTEMPLATE_DEPTH=' + str(cxx_template_depth)]

            if gcc_version >= (4, 8):
                cpp_flags += ['-fmax-errors=1']
                cpp_flags += ['-ftemplate-backtrace-limit=1']

    if debug:
        cpp_flags += ['-g']
    else:
        cpp_flags += ['-O3', '-DNDEBUG']

    return cpp_flags

run()
