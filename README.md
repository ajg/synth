synthesis
=========

A Powerful C++ Templating Framework, written by [Alvaro J. Genial](http://alva.ro).

[![Build Status](https://travis-ci.org/ajg/synthesis.png?branch=master)](https://travis-ci.org/ajg/synthesis)

Synopsis
--------

Synthesis is a template framework—a set of components that can be mixed and matched to build the right functionality; furthermore, components are loosely-coupled, designed to be both extensible and replaceable.

Status
------

The `synthesis` codebase is mature and well-tested; the bindings are still a work in progress.

Motivation
----------

Synthesis blurs the line between compile-time and runtime, and it does so by blending three worlds: (a) the static C++ type system; (b) the dynamic values that need to be manipulated and formatted; and (c) the templates to do so. The name is an allusion to this process.

Dependencies
------------

Building `synthesis` requires:

 - A modern C++ compiler
 - [Boost](http://boost.org)
 - [SCons](http://scons.org)

It is known to compile with:

 - `g++` versions `4.2.1` & `4.6.3`
 - `clang++` version `3.3`, including Apple's `LLVM version 5.0`
 - `boost` versions `1.42`, `1.46` & `1.55`

Note: to use the `python` binding, Boost.Python is required; if using `brew` on OS X, use the command:

    brew install boost --build-from-source

Components
----------

### Engines

 - `django`
 - `ssi`
 - `tmpl`

### Bindings

 - `python`

### Templates

 - `file_template`
 - `string_template`

### Adapters

 - `array`
 - `bool`
 - `complex`
 - `container`
 - `deque`
 - `list`
 - `map`
 - `memory`
 - `numeric`
 - `optional`
 - `pair`
 - `pointer`
 - `ref`
 - `scoped_array`
 - `scoped_ptr`
 - `set`
 - `shared_array`
 - `shared_ptr`
 - `smart_ptr`
 - `stack`
 - `string`
 - `utility`
 - `variant`
 - `vector`

### Extesibility

 - `base_template`
 - `abstract_adapter`

Future Work
-----------

 - Update VS project files and test on Windows.
 - Facility to handle `TEMPLATE_STRING_IF_INVALID`.
 - Create `c++11` branch.
 - Complex expressions in the `django` engine.
 - `python` binding based on [Boost.Python](http://www.boost.org/doc/libs/release/libs/python/)
   * Plus template loaders for Django
 - `ruby` binding based on [Rice](http://rice.rubyforge.org/)
 - `go` binding based on [cgo](http://golang.org/cmd/cgo/)
 - `cheetah` engine
 - `stream_template`
 - `multi_engine`
 - `c++11` type adapters
 - `c++14` type adapters
 - `tribool` adapter
 - Move `*_template`s to own namespace.
 - Move `engine`s to own namespace.
 - Split out engine options to separate header files.

License
-------

This library is distributed under the Boost [LICENSE](./LICENSE_1_0.txt).


[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/ajg/synthesis/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

