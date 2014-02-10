synthesis
=========

An Efficient, Versatile C++ Templating Framework, written by [Alvaro J. Genial](http://alva.ro).

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

Components
----------

### Engines

 - `django`
 - `ssi`
 - `tmpl`

### Templates

 - `file_template`
 - `stream_template`
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
 - `string`
 - `utility`
 - `variant`
 - `vector``

### Extesibility

 - `base_template`
 - `abstract_adapter`

License
-------

This library is distributed under the Boost [LICENSE](./LICENSE_1_0.txt).


[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/ajg/synthesis/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

