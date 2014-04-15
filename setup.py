##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

import fnmatch
import os
import re
import sys
from distutils.core import setup, Extension
from glob import glob
from os.path import join

def run():
    setup(
        name = 'synth',
        version = '.'.join(map(str, get_synth_version())),
        description = 'A Python binding to the Synth C++ Template Framework',
        long_description = get_long_description(),
        keywords = 'django, tmpl, ssi, template, framework',
        author = 'Alvaro J. Genial',
        author_email = 'genial@alva.ro',
        license = 'Boost Software License V1',
        url = 'https://github.com/ajg/synth',
        ext_modules = [get_extension()],
        data_files = get_data_files(),
        classifiers = get_classifiers(),
        # TODO: test_suite = 'synth.tests',
    )

def get_long_description():
    return '''
Synth is a framework that provides C++ implementations of various template
engines, including Django, SSI and HTML::Template (TMPL). This library provides
a simple binding to Synth in the form of a Template class.
'''

def get_classifiers():
    return [
        'Development Status :: 4 - Beta',
        'Environment :: Console',
        'Intended Audience :: Developers',
        'Intended Audience :: Other Audience',
        'Intended Audience :: Science/Research',
        'Natural Language :: English',
        'Operating System :: OS Independent',
        'Programming Language :: C++',
        'Programming Language :: Python',
        'Topic :: Software Development :: Libraries',
        'Topic :: Scientific/Engineering',
    ]

def get_extension():
    return Extension(
        'synth',
        language             = get_language(),
        sources              = get_sources(),
        libraries            = get_libraries(),
        include_dirs         = get_include_dirs(),
        library_dirs         = get_library_dirs(),
        runtime_library_dirs = get_runtime_library_dirs(),
        extra_compile_args   = get_extra_compile_args(),
        define_macros        = get_define_macros(),
        undef_macros         = get_undef_macros(),
    )

# TODO: Allow some of these to be overridden via environment variable:
char_type   = 'char' # Other possibilities are 'wchar_t' or 'Py_UNICODE', which differ in Python < 3.
is_debug    = False
is_static   = True
is_threaded = True
synth_base  = join('ajg', 'synth')
is_windows  = sys.platform == 'win32'

def get_and_set_windows_msvc_version():
    latest, path = None, None

    for name, value in os.environ.items():
        match = re.search(r'VS(\d\d?)(\d)COMNTOOLS', name)
        if match:
            version = (int(match.group(1)), int(match.group(2)))
            if not latest or version > latest:
                latest = version
                path = value
    
    if not latest:
        raise Exception('No version of MSVC found')

    # These are hard-coded somewhere in the guts of setuptools/distutils:
    os.environ['VS90COMNTOOLS']  = path
    os.environ['VS100COMNTOOLS'] = path
    return latest

if is_windows:
    is_msvc       = True       # FIXME: Exclude non-msvc compilers.
    architecture  = 32         # TODO: Detect.
    msvc_version  = get_and_set_windows_msvc_version()
    boost_version = (1, 55, 0) # TODO: Try to detect from standard install locations (c:/boost*, c:/local/boost*, etc.)
    boost_path    = 'c:/local/boost_%d_%d_%d/' % boost_version
else:
    is_msvc = False

def get_windows_boost_include_dir():
    return boost_path

def get_windows_boost_library_dir():
    prefix = 'lib%d' % architecture
    suffix = '%d.%d' % msvc_version
    dir    = prefix + '-msvc-' + suffix
    return boost_path + dir

def get_windows_boost_runtime_library_dir():
    return get_windows_boost_library_dir()

def get_extra_compile_args():
    if is_msvc:
        # TODO: SET VS90COMNTOOLS=%VS120COMNTOOLS% (modulo version)
        # TODO: Some of this is repeated in *.vcxproj.

        linkage = 'STATIC' if is_static else 'DYN'
        define  = 'BOOST_PYTHON_%s_LIB=1' % linkage
        return [
            '/D' + define,
            '/bigobj', # Prevent reaching object limit.
            '/EHsc',   # Override structured exception handling (SEH).
            '/FD',     # Allow minimal rebuild.
            '/wd4273', # "inconsistent dll linkage" in pymath.h.
            '/wd4180', # "qualifier applied to function type has no meaning" in list_of.hpp.
            # TODO: Silence or fix the spurious conversion warnings.
        ]
    else:
        # TODO: Some of this is repeated in SConstruct.
        return [
            '-Wno-unsequenced',
            '-Wno-unused-value',
            # TODO: if clang, '-ferror-limit=1',
            # TODO: if clang, '-ftemplate-backtrace-limit=1',
        ]

def get_include_dirs():
    if is_windows:
        return ['.', get_windows_boost_include_dir()]
    else:
        return ['.']

def get_library_dirs():
    if is_windows:
        return [get_windows_boost_library_dir()]
    else:
        return []

def get_libraries():
    name = 'boost_python'
    if is_windows:
        prefix    = 'lib' if is_static else ''
        threading = '-mt' if is_threaded else ''
        compiler  = '-vc%d%d' % msvc_version
        version   = '-%d_%d' % boost_version[:-1]
        return [prefix + name + compiler + threading + version]
    else:
        return [name]

def get_runtime_library_dirs():
    if is_windows:
        return [] if is_static else [get_windows_boost_runtime_library_dir()]
    else:
        return []

def get_synth_version():
    config = open(join(synth_base, 'version.hpp')).read()
    major  = int(re.search(r'AJG_SYNTH_VERSION_MAJOR\s+(\S+)', config).group(1))
    minor  = int(re.search(r'AJG_SYNTH_VERSION_MINOR\s+(\S+)', config).group(1))
    patch  = int(re.search(r'AJG_SYNTH_VERSION_PATCH\s+(\S+)', config).group(1))
    return (major, minor, patch)

def get_define_macros():
    defines = [('AJG_SYNTH_DEFAULT_CHAR_TYPE', char_type)]
    return defines if is_debug else defines + [('NDEBUG', None)]

def get_undef_macros():
    return ['NDEBUG'] if is_debug else []

def get_language():
    return 'c++'

def get_sources():
    return [
        join(synth_base, 'bindings', 'python', 'module.cpp'),
    ]

def get_data_files():
    data_files = []

    for base, _, files in os.walk(synth_base):
        headers = []
        for file in fnmatch.filter(files, '*.hpp'):
            header = join(base, file)
            headers.append(header)
        target = join('include', base)
        data_files.append((target, headers))

    return data_files

run()
