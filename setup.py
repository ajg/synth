##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

import re
# from setuptools import setup
from distutils.core import setup, Extension
from glob import glob

long_description = '''
Synth is a framework that provides C++ implementations of various template
engines, including Django, SSI and HTML::Template (TMPL). This library provides
a simple binding to Synth in the form of a Template class.
'''

# TODO: Find a more elegant way to do this:
cpp_files = (
    glob('ajg/synth/*.?pp') +
    glob('ajg/synth/*/*.?pp') +
    glob('ajg/synth/*/*/*.?pp') +
    glob('ajg/synth/*/*/*/*.?pp')
)

extension = Extension(
    'synth',
    sources = ['ajg/synth/bindings/python/module.cpp'],
    libraries = ['boost_python'],
    include_dirs = ['.'],
    language = 'c++',
    extra_compile_args = ['-Wno-unsequenced', '-Wno-unused-value'],
    define_macros = [('NDEBUG', '1')],
)

classifiers = [
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

def get_synth_version():
    config = open('ajg/synth/config.hpp').read()
    major  = int(re.search('AJG_SYNTH_VERSION_MAJOR\\s+(\\S+)', config).group(1))
    minor  = int(re.search('AJG_SYNTH_VERSION_MINOR\\s+(\\S+)', config).group(1))
    patch  = int(re.search('AJG_SYNTH_VERSION_PATCH\\s+(\\S+)', config).group(1))
    return (major, minor, patch)

setup(
    name = 'synth',
    version = '.'.join(map(str, get_synth_version())),
    description = 'A Python binding to the Synth C++ Template Framework',
    long_description = long_description,
    keywords = 'django, tmpl, ssi, template, framework',
    author = 'Alvaro J. Genial',
    author_email = 'genial@alva.ro',
    license = 'Boost Software License V1',
    url = 'https://github.com/ajg/synth',
    ext_modules = [extension],
    data_files = [('', cpp_files)],
    classifiers = classifiers,
    # TODO: test_suite = 'synth.tests',
)
