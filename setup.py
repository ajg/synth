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
headers = (
    glob('ajg/synth/*.hpp') +
    glob('ajg/synth/*/*.hpp') +
    glob('ajg/synth/*/*/*.hpp') +
    glob('ajg/synth/*/*/*/*.hpp')
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
    return re.search('AJG_SYNTH_VERSION\\s+(\\S+)', config).group(1)

setup(
    name = 'synth',
    version = get_synth_version(),
    description = 'A Python binding to the Synth C++ Template Framework',
    long_description = long_description,
    keywords = 'django, tmpl, ssi, template, framework',
    author = 'Alvaro J. Genial',
    author_email = 'genial@alva.ro',
    license = 'Boost Software License V1',
    url = 'https://github.com/ajg/synth',
    ext_modules = [extension],
    data_files = [('', headers)],
    # headers = headers,
    # test_suite = 'your.module.tests',
    classifiers = classifiers,
)
