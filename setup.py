

# from setuptools import setup
from distutils.core import setup, Extension
from glob import glob

long_description='''
Synthesis is a framework that provides C++ implementations of various template
engines, including Django, SSI and HTML::Template (TMPL). This library provides
a simple binding to Synthesis in the form of a Template class.
'''

headers=(
    glob('ajg/synthesis/*.hpp') +
    glob('ajg/synthesis/*/*.hpp') +
    glob('ajg/synthesis/*/*/*.hpp') +
    glob('ajg/synthesis/*/*/*/*.hpp')
)

setup(
    name='synthesis',
    version = '0.1.0',
    description='A Python binding to the Synthesis C++ Template Framework',
    long_description=long_description,
    author='Alvaro J. Genial',
    author_email='genial@alva.ro',
    license = "Boost Software License V1",
    url="https://github.com/ajg/synthesis",
    ext_modules=[
        Extension(
            'synthesis',
            sources=['ajg/synthesis/bindings/python/module.cpp'],
            libraries=['boost_python'],
            include_dirs=['.'],
            language='c++',
            extra_compile_args=['-Wno-unsequenced', '-Wno-unused-value'],
        ),
    ],
    data_files=[('', headers)],
    # headers=headers,
    # test_suite = 'your.module.tests',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Environment :: Console',
        'Intended Audience :: Developers',
        'Intended Audience :: Other Audience',
        'Intended Audience :: Science/Research',
        'Natural Language :: English',
        'Programming Language :: C++',
        'Programming Language :: Python',
        'Topic :: Software Development :: Libraries',
        'Topic :: Scientific/Engineering',
    ],
)
