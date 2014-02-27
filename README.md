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

Synthesis blurs the line between compile-time and runtime, and it does so by blending three worlds: (a) the static C++ type system; (b) the dynamic values that need to be manipulated and formatted; and (c) the templates to do so. The name is an allusion to this process, which generates new entities (strings, streams, files, values, etc.)

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

 - [`django`](#django-engine): An implementation of [Django Templates](https://docs.djangoproject.com/en/dev/topics/templates/).
 - [`ssi`](#ssi-engine): An implementation of [Server Side Includes](http://httpd.apache.org/docs/current/howto/ssi.html).
 - [`tmpl`](#tmpl-engine): An implementation of [Perl's HTML::Template](http://html-template.sourceforge.net/html_template.html).

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

Django Engine
-------------

### Tags

 - `django::autoescape_tag`
 - `django::block_tag`
 - `django::comment_tag`
 - `django::csrf_token_tag`
 - `django::cycle_tag`
 - `django::debug_tag`
 - `django::extends_tag`
 - `django::filter_tag`
 - `django::firstof_tag`
 - `django::for_tag`
 - `django::for_empty_tag`
 - `django::if_tag`
 - `django::ifchanged_tag`
 - `django::ifequal_tag`
 - `django::ifnotequal_tag`
 - `django::include_tag`
 - `django::load_tag` (TODO)
 - `django::now_tag`
 - `django::regroup_tag` (TODO)
 - `django::spaceless_tag`
 - `django::ssi_tag`
 - `django::templatetag_tag`
 - `django::url_tag` (TODO)
 - `django::variable_tag`
 - `django::verbatim_tag`
 - `django::widthratio_tag`
 - `django::with_tag`

### Filters

 - `django::add_filter`
 - `django::addslashes_filter`
 - `django::capfirst_filter`
 - `django::center_filter`
 - `django::cut_filter`
 - `django::date_filter` (TODO)
 - `django::default_filter`
 - `django::default_if_none_filter`
 - `django::dictsort_filter` (TODO)
 - `django::dictsortreversed_filter` (TODO)
 - `django::divisibleby_filter`
 - `django::escape_filter`
 - `django::escapejs_filter`
 - `django::filesizeformat_filter`
 - `django::first_filter`
 - `django::fix_ampersands_filter`
 - `django::floatformat_filter`
 - `django::force_escape_filter`
 - `django::get_digit_filter`
 - `django::iriencode_filter`
 - `django::join_filter`
 - `django::last_filter`
 - `django::length_filter`
 - `django::length_is_filter`
 - `django::linebreaks_filter`
 - `django::linebreaksbr_filter`
 - `django::linenumbers_filter`
 - `django::ljust_filter`
 - `django::lower_filter`
 - `django::make_list_filter`
 - `django::phone2numeric_filter`
 - `django::pluralize_filter`
 - `django::pprint_filter` (TODO)
 - `django::random_filter`
 - `django::removetags_filter`
 - `django::rjust_filter`
 - `django::safe_filter`
 - `django::safeseq_filter`
 - `django::slice_filter`
 - `django::slugify_filter`
 - `django::stringformat_filter`
 - `django::striptags_filter`
 - `django::time_filter` (TODO)
 - `django::timesince_filter` (TODO)
 - `django::timeuntil_filter` (TODO)
 - `django::title_filter`
 - `django::truncatechars_filter`
 - `django::truncatechars_html_filter` (TODO)
 - `django::truncatewords_filter`
 - `django::truncatewords_html_filter` (TODO)
 - `django::unordered_list_filter`
 - `django::upper_filter`
 - `django::urlencode_filter`
 - `django::urlize_filter`
 - `django::urlizetrunc_filter`
 - `django::wordcount_filter`
 - `django::wordwrap_filter`
 - `django::yesno_filter`

### Options

 - `django::options::autoescape`
 - `django::options::default_value` (for `TEMPLATE_STRING_IF_INVALID`)
 - `django::options::directories` (for `TEMPLATE_DIRS`)

SSI Engine
----------

### Directives

 - `ssi::config_directive`
 - `ssi::echo_directive`
 - `ssi::exec_directive`
 - `ssi::fsize_directive`
 - `ssi::flastmod_directive`
 - `ssi::if_directive`
 - `ssi::include_directive`
 - `ssi::printenv_directive`
 - `ssi::set_directive`

TMPL Engine
-----------

### Tags

 - `tmpl::comment_tag` (Part of [`ctpp`](http://ctpp.havoc.ru/en/), not `tmpl`.)
 - `tmpl::if_tag`
 - `tmpl::include_tag`
 - `tmpl::loop_tag`
 - `tmpl::unless_tag`
 - `tmpl::variable_tag`

Future Work
-----------

 - Update VS project files and test on Windows.
 - Create `c++11`/`c++14` branch.
   * `c++11` type adapters
   * `c++14` type adapters
   * Translate macros to variadic templates
 - Django template loaders.
 - `ruby` binding based on [Rice](http://rice.rubyforge.org/)
 - `go` binding based on [cgo](http://golang.org/cmd/cgo/)
 - `cheetah` engine
 - `ctpp` engine
 - `stream_template`
 - `multi_engine`
 - `tribool` adapter
 - Move `*_template`s to own namespace.
 - Move `engine`s to own namespace.
 - Compare benefit/cost of `-O`, `-O2`, `-O3` and `-Ofast`.

License
-------

This library is distributed under the Boost [LICENSE](./LICENSE_1_0.txt).


[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/ajg/synthesis/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

