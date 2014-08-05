##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

import os
import re
import subprocess
import sys

DEBUG = int(ARGUMENTS.get('debug', 0))
GROUP = str(ARGUMENTS.get('group', ''))
BOOST = str(ARGUMENTS.get('boost', 'local'))

if BOOST not in ('local', 'system'):
    sys.exit('Argument `boost` must be `local` or `system`')

def run():
    cxx = ARGUMENTS.get('CXX', os.environ.get('CXX', 'c++'))
    env = make_environment(cxx)
    create_targets(env)

def make_environment(cxx):
    return Environment(
        CXX      = cxx,
        CPPPATH  = get_cpp_path(),
        CPPFLAGS = get_cpp_flags(cxx),
        # TODO: VariantDir('build' / 'stage', '?', duplicate=0)
    )

def create_targets(env):
    harness = env.Clone()
    harness.Append(CPPPATH = ['external/tut-framework/include'])
    harness.Program(
        target = 'tests/harness.out',
        source = ['tests/harness.cpp'] + find_test_sources(),
    )

    examples = env.Clone()
    examples.Program(
        target = 'examples/all',
        source = ['examples/simple_ssi.cpp', 'examples/simple_ssi_wide.cpp'],
    )

    tool = env.Clone()
    tool.Program(
        target = 'synth',
        source = ['ajg/synth/bindings/command_line/tool.cpp'],
    )

    # Note: For development only; normally use setup.py instead.
    from distutils import sysconfig
    python_module = env.Clone()
    python_module.LoadableModule(
        target    = 'tests/synth.so',
        source    = ['ajg/synth/bindings/python/module.cpp'] + find_boost_sources(),
        CPPPATH   = [sysconfig.get_python_inc()] + get_cpp_path(),
        LIBPATH   = [sysconfig.get_config_var('LIBDIR')],
        LIBPREFIX = '',
        LIBS      = ['python' + sysconfig.get_config_var('VERSION')],
    )

    return [harness, examples, tool]

def find_test_sources():
    if GROUP:
        return ['tests/groups/%s.cpp' % GROUP]
    else:
        return Glob('tests/groups/*.cpp')

def find_boost_sources():
    boost_path = find_boost_path()
    return (Glob(boost_path + '/libs/python/src/*.cpp')
          + Glob(boost_path + '/libs/python/src/*/*.cpp'))

def find_cxx_version(cxx):
    try:
        # NOTE: '--version' alone doesn't always work with g++
        return subprocess.check_output([cxx, '--version', '--verbose'], stderr=subprocess.STDOUT)
    except OSError as e:
        sys.exit('Unable to find compiler (%s) version: ' % cxx + e.strerror)

def find_boost_path():
    if BOOST == 'local':
        return 'external/boost'
    elif BOOST == 'system':
        return None
    else:
        sys.exit('Unknown value for option `boost`')

def get_cpp_path():
    boost_path = find_boost_path()
    cpp_path   = ['.']
    cpp_path  += [boost_path] if boost_path else []
    return cpp_path

def get_cpp_flags(cxx):
    # TODO: Handle MSVC.

    cpp_flags = []

    # Common flags:
    cpp_flags += ['-Wall']
    cpp_flags += ['-Woverloaded-virtual']
    cpp_flags += ['-Wsign-promo']
    cpp_flags += ['-Wno-long-long']
    # TODO: cpp_flags += ['-Wsurprising']
    # TODO: cpp_flags += ['-Weffc++']
    cpp_flags += ['-Wextra']
    cpp_flags += ['-Wno-unused-parameter']
    # XXX: Not including -Wold-style-cast due to optionparser.h.

    cxx_version    = find_cxx_version(cxx)
    template_depth = '768'

    # Conditional flags:
    if BOOST != 'system':
        cpp_flags += ['-DBOOST_ALL_NO_LIB']

    if 'clang' in cxx_version:
        cpp_flags += ['-std=c++11']
        cpp_flags += ['-pedantic']
        cpp_flags += ['-Wuninitialized']
        cpp_flags += ['-Wc++11-narrowing']
        cpp_flags += ['-ferror-limit=1']
        # cpp_flags += ['-ftemplate-backtrace-limit=1']

        # TODO: Only version 3.3+:
        cpp_flags += ['-ftemplate-depth=' + template_depth]
        cpp_flags += ['-DAJG_SYNTH_CONFIG_MAX_TEMPLATE_DEPTH=' + template_depth]

        if BOOST != 'system':
            cpp_flags += ['-Wno-newline-eof']
            cpp_flags += ['-Wno-nested-anon-types']
            cpp_flags += ['-Wno-language-extension-token']

    elif 'g++' in cxx_version or 'gcc' in cxx_version:
        if not DEBUG:
            cpp_flags += ['-Wuninitialized'] # g++ doesn't support this without -O

        # FIXME: Triggers "warning: non-standard suffix on floating constant [-pedantic]":
        # cpp_flags += ['-pedantic']
        cpp_flags += ['-Wfatal-errors']
        cpp_flags += ['-Wstrict-null-sentinel']
        cpp_flags += ['-Wno-sign-promo']

        triple = re.search(r'\s(\d+)[.](\d+)[.](\d+)\s', cxx_version)
        if triple:
            major = int(triple.group(1))
            minor = int(triple.group(2))
            patch = int(triple.group(3))
            gcc_version = (major, minor, patch)

            if gcc_version < (4, 5):
                cpp_flags += ['-ftemplate-depth-' + template_depth] # Note the dash
                cpp_flags += ['-DAJG_SYNTH_CONFIG_MAX_TEMPLATE_DEPTH=' + template_depth]
            else:
                cpp_flags += ['-ftemplate-depth=' + template_depth]
                cpp_flags += ['-DAJG_SYNTH_CONFIG_MAX_TEMPLATE_DEPTH=' + template_depth]

            if gcc_version >= (4, 8):
                cpp_flags += ['-Wmaybe-uninitialized']
                cpp_flags += ['-Wnarrowing']
                cpp_flags += ['-fmax-errors=1']
                cpp_flags += ['-ftemplate-backtrace-limit=1']

            if gcc_version >= (4, 7):
                cpp_flags += ['-std=c++11']
            else:
                cpp_flags += ['-std=c++0x']

    if DEBUG:
        cpp_flags += ['-g'] # '-fstack-protector-all'
    else:
        cpp_flags += ['-O3', '-DNDEBUG']

    return cpp_flags

run()
