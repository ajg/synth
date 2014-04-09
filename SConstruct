##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

import os
import re
import subprocess
import sys

debug = int(ARGUMENTS.get('debug', 0))
group = str(ARGUMENTS.get('group', ''))

def run():
    cxx = ARGUMENTS.get('CXX', os.environ.get('CXX', 'c++'))
    env = make_environment(cxx)
    create_targets(env)

def make_environment(cxx):
    # TODO: VariantDir('build' / 'stage', '?', duplicate=0)
    return Environment(
        CXX      = cxx,
        CPPPATH  = ['.'],
        CPPFLAGS = get_cpp_flags(cxx),
    )

def create_targets(env):
    test_harness = env.Clone()
    test_harness.Append(CPPPATH = ['tests/tut-framework/include'])
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

    return [test_harness, command_line_tool]

def find_test_sources():
    if group:
        return ['tests/groups/%s.cpp' % group]
    else:
        return Glob('tests/groups/*.cpp')

def find_cxx_version(cxx):
    try:
        # NOTE: '--version' alone doesn't always work with g++
        return subprocess.check_output([cxx, '--version', '--verbose'], stderr=subprocess.STDOUT)
    except OSError as e:
        sys.exit('Unable to find compiler (%s) version: ' % cxx + e.strerror)

def get_cpp_flags(cxx):
    cpp_flags = []

    # Common flags:
    cpp_flags += ['-Wall']
    cpp_flags += ['-Wold-style-cast']
    cpp_flags += ['-Woverloaded-virtual']
    cpp_flags += ['-Wsign-promo']
    # TODO: cpp_flags += ['-Wsurprising']
    # TODO: cpp_flags += ['-Weffc++']
    cpp_flags += ['-Wextra', '-Wno-unused-parameter']
    cpp_flags += ['-pedantic', '-Wno-long-long']

    cxx_version = find_cxx_version(cxx)
    cxx_template_depth = 1024

    # Conditional flags:
    if 'clang' in cxx_version:
        cpp_flags += ['-Wuninitialized']
        cpp_flags += ['-Wc++11-narrowing']
        cpp_flags += ['-ferror-limit=1']
        cpp_flags += ['-ftemplate-backtrace-limit=1']
        cpp_flags += ['-ftemplate-depth=' + str(cxx_template_depth)]
        cpp_flags += ['-DTEMPLATE_DEPTH=' + str(cxx_template_depth)]

    elif 'g++' in cxx_version or 'gcc' in cxx_version:
        if not debug:
            cpp_flags += ['-Wuninitialized'] # g++ doesn't support this without -O

        cpp_flags += ['-Wfatal-errors']
        cpp_flags += ['-Wstrict-null-sentinel']

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
                cpp_flags += ['-Wmaybe-uninitialized']
                cpp_flags += ['-Wnarrowing']
                cpp_flags += ['-fmax-errors=1']
                cpp_flags += ['-ftemplate-backtrace-limit=1']


    if debug:
        cpp_flags += ['-g'] # '-fstack-protector-all'
    else:
        cpp_flags += ['-O3', '-DNDEBUG']

    return cpp_flags

run()
