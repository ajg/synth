##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

import fnmatch
import os
import platform
import re
import sys
from distutils import sysconfig
from distutils.core import setup, Extension

# TODO: Allow some of these to be overridden via environment variable and/or command option.
DEBUG = False
CHAR  = 'char' # Other possibilities are 'wchar_t' or 'Py_UNICODE', which differ until Python 3.something.
BOOST = 'auto' # TODO: Implement `local` and `system`.

if BOOST not in ('auto', 'local', 'system'):
    sys.exit('Option `boost` must be `auto`, `local` or `system`')

def get_boost_path():
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
    )

def find_synth_version():
    config = open('ajg/synth/version.hpp').read()
    major  = int(re.search(r'AJG_SYNTH_VERSION_MAJOR\s+(\S+)', config).group(1))
    minor  = int(re.search(r'AJG_SYNTH_VERSION_MINOR\s+(\S+)', config).group(1))
    patch  = int(re.search(r'AJG_SYNTH_VERSION_PATCH\s+(\S+)', config).group(1))
    return (major, minor, patch)

# TODO: For some reason distutils.Extension adds -Wstrict-prototypes to all extensions, even C++
#       ones, to which it doesn't apply, which causes GCC to print out a warning while building.
def initialize_compiler(platform):
    if not DEBUG:
        # Don't produce debug symbols when debug is off.
        if platform != 'windows':
            cflags = sysconfig.get_config_var('CFLAGS')
            opt = sysconfig.get_config_var('OPT')
            sysconfig._config_vars['CFLAGS'] = cflags.replace(' -g ', ' ')
            sysconfig._config_vars['OPT'] = opt.replace(' -g ', ' ')

        if platform == 'linux':
            ldshared = sysconfig.get_config_var('LDSHARED')
            sysconfig._config_vars['LDSHARED'] = ldshared.replace(' -g ', ' ')

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
        # TODO: Some of this is repeated in *.vcxproj.
        return [
            '/bigobj', # Prevent reaching object limit.
            '/EHsc',   # Override structured exception handling (SEH).
            '/FD',     # Allow minimal rebuild.
            '/wd4273', # "inconsistent dll linkage" in pymath.h.
            '/wd4180', # "qualifier applied to function type has no meaning" in list_of.hpp.
        ]
    else:
        # TODO: Some of this is repeated in SConstruct.
        return [
            # '-Wno-unsequenced',
            '-Wno-unused-value',
        ]

def get_include_dirs():
    return ['.', get_boost_path()]

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
        defines += ['NDEBUG']

    return undefines

def get_language():
    return 'c++'

def get_sources():
    sources = []

    sources += ['ajg/synth/bindings/python/module.cpp']

    if BOOST != 'system':
        boost_path = get_boost_path() + '/'
        sources += [boost_path + source for source in boost_python_sources]

    return sources

# NOTE: distutils does not recognize the .hpp extension for headers so they have to be included as
#       data files, otherwise they won't show up in the MANIFEST and build_ext will fail (one known
#       "workaround", which is to deal with MANIFEST.in manually, sounds like a first-class ticket
#       to an asylum.) Also, sadly, the `depends` argument to `Extension` is, essentially, useless.
def get_data_files():
    data_files = []

    for base, _, files in os.walk('ajg/synth'):
        headers = []
        for file in fnmatch.filter(files, '*.hpp'):
            header = base + '/' + file
            headers.append(header)
        target = 'include/' + base
        data_files.append((target, headers))

    return data_files

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
