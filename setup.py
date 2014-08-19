##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

import fnmatch
import os
import platform
import re
import sys
from distutils import sysconfig
# from distutils.core import setup, Extension
from setuptools import setup, Extension

# TODO: Allow CHAR and BOOST to be passed via command-line.
DEBUG = ('-g' in sys.argv or '--debug' in sys.argv)
CHAR  = 'char' # Other possibilities are 'wchar_t' or 'Py_UNICODE', which differ until Python 3.something.
BOOST = 'auto' # TODO: Implement `local` and `system`.

if BOOST not in ('auto', 'local', 'system'):
    sys.exit('Option `boost` must be `auto`, `local` or `system`')

def find_boost_path():
    if BOOST in ('auto', 'local'):
        return 'external/boost'
    elif BOOST == 'system':
        sys.exit('Not implemented: `system` boost')
    else:
        sys.exit('Unknown value for option `boost`')

def run():
    setup(
        name             = 'synth',
        version          = '.'.join(map(str, find_synth_version())),
        description      = 'A Python binding to the Synth C++ Template Framework',
        long_description = get_long_description(),
        keywords         = 'django, tmpl, ssi, template, framework',
        author           = 'Alvaro J. Genial',
        author_email     = 'genial@alva.ro',
        license          = 'Boost Software License V1',
        url              = 'https://github.com/ajg/synth',
        ext_modules      = [get_extension()],
        data_files       = get_data_files(),
        classifiers      = get_classifiers(),
        zip_safe         = True,
        # TODO: test_suite
    )

def get_long_description():
    return '''
Synth is a framework that provides C++ implementations of various template
engines, including Django, SSI and HTML::Template (TMPL). This library provides
a simple binding to Synth in the form of a Template class.
'''

def get_classifiers():
    return [
        'Development Status :: 5 - Production/Stable',
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
    compiler = initialize_compiler(platform.system().lower())
    return Extension(
        'synth',
        language             = get_language(),
        sources              = get_sources(),
        libraries            = get_libraries(),
        include_dirs         = get_include_dirs(),
        library_dirs         = get_library_dirs(),
        runtime_library_dirs = get_runtime_library_dirs(),
        extra_compile_args   = get_extra_compile_args(compiler),
        define_macros        = get_define_macros(),
        undef_macros         = get_undef_macros(),
        depends              = get_depends(),
    )

def find_synth_version():
    config = open('ajg/synth/version.hpp').read()
    major  = int(re.search(r'AJG_SYNTH_VERSION_MAJOR\s+(\S+)', config).group(1))
    minor  = int(re.search(r'AJG_SYNTH_VERSION_MINOR\s+(\S+)', config).group(1))
    patch  = int(re.search(r'AJG_SYNTH_VERSION_PATCH\s+(\S+)', config).group(1))
    return (major, minor, patch)

# TODO: Use regexen here with word boundaries (\b) rather than spaces.
def initialize_compiler(platform):
    def purge(name):
        old = sysconfig.get_config_var(name)

        if not old:
            return

        if DEBUG:
            # Don't optimize when debug is on.
            new = (old.
                replace(' -O ', ' ').
                replace(' -O2 ', ' ').
                replace(' -O3 ', ' ').
                replace('-Wstrict-prototypes', ' '))
        else:
            # Don't produce debug symbols when debug is off.
            new = (old.
                replace('-g ', ' '). # No space
                replace('-Wstrict-prototypes', ' '))

        sysconfig._config_vars[name] = new

    if platform == 'windows':
        msvc_info = find_msvc_info()
        if msvc_info is None:
            return 'unknown'
        version, path = msvc_info

        # Reliance on these is hard-coded somewhere in the guts of setuptools/distutils:
        os.environ['VS90COMNTOOLS']  = path
        os.environ['VS100COMNTOOLS'] = path
        os.environ['VS110COMNTOOLS'] = path
        os.environ['VS120COMNTOOLS'] = path

        return 'msvc'
    else:
        purge('CFLAGS')
        purge('CONFIGURE_CFLAGS') # Python 3 specific, it seems.
        purge('LDSHARED')
        purge('OPT')

        if not DEBUG and 'CFLAGS' in os.environ: # RPM-specific
            os.environ['CFLAGS'] = os.environ['CFLAGS'].replace('-g ', ' ')

        return 'default' # TODO: clang vs. gcc

def find_msvc_info():
    latest, path = None, None

    for name, value in os.environ.items():
        match = re.search(r'VS(\d\d?)(\d)COMNTOOLS', name)
        if match:
            version = (int(match.group(1)), int(match.group(2)))
            if not latest or version > latest:
                latest = version
                path = value

    if not latest:
        return None

    return (latest, path)

def get_extra_compile_args(compiler):
    if compiler == 'msvc':
        # TODO: Use Synth.props instead.
        return [
            '/bigobj', # Prevent reaching object limit.
            '/EHsc',   # Override structured exception handling (SEH).
            '/FD',     # Allow minimal rebuild.
            '/wd4180', # "qualifier applied to function type has no meaning" in list_of.hpp.
            '/wd4244', # "conversion from 'Py_ssize_t' to 'unsigned int'" in caller.hpp.
            '/wd4267', # "conversion from 'size_t' to 'const int'" in various places.
            '/wd4273', # "inconsistent dll linkage" in pymath.h.
            '/wd4503', # "decorated name length exceeded, name was truncated" in xpressive.
        ]
    else:
        # TODO: Some of this is repeated in SConstruct.
        return [
            # '-ferror-limit=1',
            # '-ftemplate-backtrace-limit=0',
            # '-Wno-unsequenced',
            '-Wno-unused',
        ]

def get_include_dirs():
    return ['.', find_boost_path()]

def get_library_dirs():
    return []

def get_libraries():
    return []

def get_runtime_library_dirs():
    return []

def get_define_macros():
    defines = []

    # Common defines:
    defines += [('AJG_SYNTH_CONFIG_DEFAULT_CHAR_TYPE', CHAR)]

    # Conditional defines:
    if BOOST != 'system':
        defines += [
            ('BOOST_ALL_NO_LIB',            None),
            ('BOOST_PYTHON_NO_LIB',         None),
            ('BOOST_PYTHON_STATIC_LIB',     None),
            ('BOOST_PYTHON_SOURCE',         None),
        ]

    if not DEBUG:
        defines += [('NDEBUG', None)]

    return defines

def get_undef_macros():
    undefines = []

    if DEBUG:
        undefines += ['NDEBUG']

    return undefines

def get_depends():
    return get_sources() + get_headers()

def get_language():
    return 'c++'

def get_sources():
    sources = []

    sources += ['ajg/synth/bindings/python/module.cpp']

    if BOOST != 'system':
        boost_path = find_boost_path() + '/'
        sources += [boost_path + source for source in boost_python_sources]

    return sources

def get_headers():
    headers = []

    for base, _, files in os.walk('.'):
        for file in fnmatch.filter(files, '*.[hi]pp') + fnmatch.filter(files, '*.h'):
            header = base + '/' + file
            headers.append(header)

    return headers

def get_data_files():
    return []

boost_python_sources = [
    'libs/python/src/numeric.cpp',
    'libs/python/src/list.cpp',
    'libs/python/src/long.cpp',
    'libs/python/src/dict.cpp',
    'libs/python/src/tuple.cpp',
    'libs/python/src/str.cpp',
    'libs/python/src/slice.cpp',
    'libs/python/src/converter/from_python.cpp',
    'libs/python/src/converter/registry.cpp',
    'libs/python/src/converter/type_id.cpp',
    'libs/python/src/object/enum.cpp',
    'libs/python/src/object/class.cpp',
    'libs/python/src/object/function.cpp',
    'libs/python/src/object/inheritance.cpp',
    'libs/python/src/object/life_support.cpp',
    'libs/python/src/object/pickle_support.cpp',
    'libs/python/src/errors.cpp',
    'libs/python/src/module.cpp',
    'libs/python/src/converter/builtin_converters.cpp',
    'libs/python/src/converter/arg_to_python_base.cpp',
    'libs/python/src/object/iterator.cpp',
    'libs/python/src/object/stl_iterator.cpp',
    'libs/python/src/object_protocol.cpp',
    'libs/python/src/object_operators.cpp',
    'libs/python/src/wrapper.cpp',
    'libs/python/src/import.cpp',
    'libs/python/src/exec.cpp',
    'libs/python/src/object/function_doc_signature.cpp',
]

run()
