synth
=====

A Powerful C++ Templating Framework with [command-line](#command-line) and [Python](#python) bindings, written by [Alvaro J. Genial](http://alva.ro).

[![Build Status](https://travis-ci.org/ajg/synth.png?branch=master)](https://travis-ci.org/ajg/synth)

Synopsis
--------

Synth is a template framework—a set of components that can be mixed and matched to build the right functionality; furthermore, components are loosely-coupled, designed to be both extensible and replaceable.

Status
------

Beta, approaching a first full release.

Motivation
----------

Synth blurs the line between compile-time and runtime, and it does so by blending three worlds: (a) the static C++ type system; (b) the dynamic values that need to be manipulated and formatted, including those from other languages; and (c) the templates to do so. The name is an allusion to this synthesis process, which combines values to generate new ones (streams, files, strings, numbers, etc.)

Examples
--------

### Command-line

```shell
echo '{"user": "Dolph Lundgren"}' > 'ctx.json'
echo 'Howdy, {{ user }}!' > 'tpl.txt'

cat tpl.txt | ./synth -e django -c ctx.json
```

### Python

```python
import synth, sys

def simple_tmpl_example():
    tpl = synth.Template('Howdy, <TMPL_VAR user>!', 'tmpl')
    ctx = {'user': 'Dolph Lundgren'}

    tpl.render_to_path("greeting.txt", ctx) # or, e.g.:
    tpl.render_to_file(sys.stdout, ctx) # or, e.g.:
    print tpl.render_to_string(ctx)
```

### [C++](./examples/simple_ssi.cpp)

```c++
#include <iostream>
#include <ajg/synth.hpp>

namespace synth = ajg::synth;

typedef synth::default_traits<char>                         traits_type;
typedef synth::ssi::engine<traits_type>                     engine_type;
typedef synth::templates::string_template<engine_type>      template_type;

int main() {
    template_type const tpl("Howdy, <!--#echo var=\"user\" -->!");
    template_type::context_type ctx;
    ctx["user"] = "Dolph Lundgren";

    // Render to different destinations:
    tpl.render_to_stream(std::cout);
    tpl.render_to_path("greeting.txt", ctx);
    std::cout << tpl.render_to_string(ctx);
    return 0;
}
```

Reference
---------

### Command-line

    synth [FLAGS...]
      -h [ --help ]             print help message
      -v [ --version ]          print library version
      -c [ --context ] file     the data: *.{ini,json,xml}
      -e [ --engine ] name      template engine: {django,ssi,tmpl}
      -a [ --autoescape ] bool  automatically escape values (default: 'true')
      -d [ --directories ] path template lookup directories (default: '.')
      -r [ --replacement ] text replaces missing values (default: '')

Dependencies
------------

Building `synth` from source requires:

 - [Boost](http://boost.org)
 - [SCons](http://scons.org)
 - A modern C++ compiler (`c++11` support is not required.)

Synth is known to compile with:

 - `g++` versions `4.2.1` and `4.6.3`
 - `clang++` version `3.3`, including Apple's `LLVM version 5.0`
 - `boost` versions `1.46` and `1.55`

### Installing Boost

 - On OS X, using Homebrew:

        brew install boost --build-from-source

 - Using Apt, typically on Debian or Ubuntu:

        sudo apt-get install libboost-all-dev

 - Using Yum, typically on Fedora or RHEL (untested):

        sudo yum install boost-devel

 - On Windows, download a suitable version from here:

        http://sourceforge.net/projects/boost/files/boost-binaries/

Installation
------------

### From source:

 1. Install the [dependencies](#dependencies).

 2. Get the source:

        git clone --recursive https://github.com/ajg/synth.git && cd synth

 3. [Optional] Build the command-line program:

        scons synth # Add debug=1 to generate debugging symbols & disable optimizations.

 4. [Optional] Build (and install) the Python module:

        python setup.py install # Add `sudo` as needed.

(Pre-built binaries are [in the works](#future-work).)

### Using Pip:

Install [Boost](#installing-boost), then:

    pip install synth # Add `sudo` as needed.

### Using Easy Install:

Install [Boost](#installing-boost), then:

    easy_install synth # Add `sudo` as needed.

Components
----------

### Engines

 - [`django`](#django-engine): An implementation of [Django Templates](https://docs.djangoproject.com/en/dev/topics/templates/).
 - [`ssi`](#ssi-engine): An implementation of [Server Side Includes](http://httpd.apache.org/docs/current/howto/ssi.html).
 - [`tmpl`](#tmpl-engine): An implementation of [Perl's HTML::Template](http://html-template.sourceforge.net/html_template.html).
 - `null`: A minimal implementation that does nothing (i.e. a no-op.)

### Bindings

 - `command_line`
 - `python`

### Templates

 - `char_template`
 - `path_template`
 - `stream_template`
 - `string_template`

### Adapters

 - `array`
   * `[N]` (Native static array.)
   * `[]` (Native dynamic array.)
   * `boost::array`
 - `bool`
   * `bool`
 - `complex`
   * `std::complex`
 - `container`
   * `std::deque`
   * `std::list`
   * `map`
     + `std::map`
     + `std::multimap`
   * `set`
     + `std::set`
     + `std::multiset`
   * `std::stack`
   * `std::vector`
 - `memory`
   * `std::auto_ptr`
 - `none`
   * `boost::none_t`
 - `numeric`
   * `char`
   * `char signed`
   * `char unsigned`
   * `short`
   * `short unsigned`
   * `int`
   * `int unsigned`
   * `long`
   * `long unsigned`
   * `wchar_t` (When available.)
   * `long long` (When available.)
   * `long long unsigned` (When available.)
   * `__int64` (MSVC-only.)
   * `__int64 unsigned` (MSVC-only.)
   * `float`
   * `double`
   * `long double`
 - `optional`
   * `boost::optional`
 - `ptime`
   * `boost::posix_time::ptime`
 - `ptree`
   * `boost::property_tree::ptree`
 - `pointer`
   * `*` (Native pointer.)
 - `ref`
   * `boost::reference_wrapper`
 - `smart_ptr`
   * `boost::scoped_array`
   * `boost::scoped_ptr`
   * `boost::shared_array`
   * `boost::shared_ptr`
 - `string`
   * `std::basic_string`
   * `char*`
   * `char[N]`
   * `wchar_t*` (When available.)
   * `wchar_t[N]` (When available.)
 - `utility`
   * `std::pair`
 - `variant`
   * `boost::variant`

### Input Formats

 - `ini`
 - `json`
 - `xml`

### Base Components

 - `base_adapter`
 - `base_binding`
 - `base_engine`
 - `base_template`

Django Engine
-------------

### Tags

 - `django::builtin_tags::autoescape_tag`
 - `django::builtin_tags::block_tag`
 - `django::builtin_tags::comment_tag`
 - `django::builtin_tags::csrf_token_tag`
 - `django::builtin_tags::cycle_tag`
 - `django::builtin_tags::cycle_as_tag`
 - `django::builtin_tags::cycle_as_silent_tag`
 - `django::builtin_tags::debug_tag`
 - `django::builtin_tags::extends_tag`
 - `django::builtin_tags::filter_tag`
 - `django::builtin_tags::firstof_tag`
 - `django::builtin_tags::for_tag`
 - `django::builtin_tags::for_empty_tag`
 - `django::builtin_tags::if_tag`
 - `django::builtin_tags::ifchanged_tag`
 - `django::builtin_tags::ifequal_tag`
 - `django::builtin_tags::ifnotequal_tag`
 - `django::builtin_tags::include_tag`
 - `django::builtin_tags::include_with_tag`
 - `django::builtin_tags::include_with_only_tag`
 - `django::builtin_tags::load_tag`
 - `django::builtin_tags::load_from_tag`
 - `django::builtin_tags::now_tag`
 - `django::builtin_tags::regroup_tag`
 - `django::builtin_tags::spaceless_tag`
 - `django::builtin_tags::ssi_tag`
 - `django::builtin_tags::templatetag_tag`
 - `django::builtin_tags::url_tag`
 - `django::builtin_tags::url_as_tag`
 - `django::builtin_tags::variable_tag`
 - `django::builtin_tags::verbatim_tag`
 - `django::builtin_tags::widthratio_tag`
 - `django::builtin_tags::with_tag`
 - `django::builtin_tags::library_tag`

### Filters

 - `django::builtin_filters::add_filter`
 - `django::builtin_filters::addslashes_filter`
 - `django::builtin_filters::capfirst_filter`
 - `django::builtin_filters::center_filter`
 - `django::builtin_filters::cut_filter`
 - `django::builtin_filters::date_filter`
 - `django::builtin_filters::default_filter`
 - `django::builtin_filters::default_if_none_filter`
 - `django::builtin_filters::dictsort_filter`
 - `django::builtin_filters::dictsortreversed_filter`
 - `django::builtin_filters::divisibleby_filter`
 - `django::builtin_filters::escape_filter`
 - `django::builtin_filters::escapejs_filter`
 - `django::builtin_filters::filesizeformat_filter`
 - `django::builtin_filters::first_filter`
 - `django::builtin_filters::fix_ampersands_filter`
 - `django::builtin_filters::floatformat_filter`
 - `django::builtin_filters::force_escape_filter`
 - `django::builtin_filters::get_digit_filter`
 - `django::builtin_filters::iriencode_filter`
 - `django::builtin_filters::join_filter`
 - `django::builtin_filters::last_filter`
 - `django::builtin_filters::length_filter`
 - `django::builtin_filters::length_is_filter`
 - `django::builtin_filters::linebreaks_filter`
 - `django::builtin_filters::linebreaksbr_filter`
 - `django::builtin_filters::linenumbers_filter`
 - `django::builtin_filters::ljust_filter`
 - `django::builtin_filters::lower_filter`
 - `django::builtin_filters::make_list_filter`
 - `django::builtin_filters::phone2numeric_filter`
 - `django::builtin_filters::pluralize_filter`
 - `django::builtin_filters::pprint_filter`
 - `django::builtin_filters::random_filter`
 - `django::builtin_filters::removetags_filter`
 - `django::builtin_filters::rjust_filter`
 - `django::builtin_filters::safe_filter`
 - `django::builtin_filters::safeseq_filter`
 - `django::builtin_filters::slice_filter`
 - `django::builtin_filters::slugify_filter`
 - `django::builtin_filters::stringformat_filter`
 - `django::builtin_filters::striptags_filter`
 - `django::builtin_filters::time_filter`
 - `django::builtin_filters::timesince_filter`
 - `django::builtin_filters::timeuntil_filter`
 - `django::builtin_filters::title_filter`
 - `django::builtin_filters::truncatechars_filter`
 - `django::builtin_filters::truncatechars_html_filter`
 - `django::builtin_filters::truncatewords_filter`
 - `django::builtin_filters::truncatewords_html_filter`
 - `django::builtin_filters::unordered_list_filter`
 - `django::builtin_filters::upper_filter`
 - `django::builtin_filters::urlencode_filter`
 - `django::builtin_filters::urlize_filter`
 - `django::builtin_filters::urlizetrunc_filter`
 - `django::builtin_filters::wordcount_filter`
 - `django::builtin_filters::wordwrap_filter`
 - `django::builtin_filters::yesno_filter`

### Options

 - `django::options::autoescape`
 - `django::options::nonbreaking_space`
 - `django::options::default_value` (for `TEMPLATE_STRING_IF_INVALID`)
 - `django::options::formats` (for `TIME_FORMAT`, `DATE_FORMAT`, etc.)
 - `django::options::debug` (for `TEMPLATE_DEBUG`)
 - `django::options::directories` (for `TEMPLATE_DIRS`)
 - `django::options::libraries` (for external tags & filters)
 - `django::options::loaders` (for dynamically loading libraries)

SSI Engine
----------

### Tags

 - `ssi::builtin_tags::config_tag`
 - `ssi::builtin_tags::echo_tag`
 - `ssi::builtin_tags::exec_tag`
 - `ssi::builtin_tags::fsize_tag`
 - `ssi::builtin_tags::flastmod_tag`
 - `ssi::builtin_tags::if_tag`
 - `ssi::builtin_tags::include_tag`
 - `ssi::builtin_tags::printenv_tag`
 - `ssi::builtin_tags::set_tag`

### Options

 - `ssi::options::echo_message`
 - `ssi::options::directories`
 - `ssi::options::size_format`
 - `ssi::options::time_format`
 - `ssi::options::error_message`

TMPL Engine
-----------

### Tags

 - `tmpl::builtin_tags::comment_tag` (Technically, part of [`ctpp`](http://ctpp.havoc.ru/en/))
 - `tmpl::builtin_tags::if_tag`
 - `tmpl::builtin_tags::include_tag`
 - `tmpl::builtin_tags::loop_tag`
 - `tmpl::builtin_tags::unless_tag`
 - `tmpl::builtin_tags::variable_tag`

Future Work
-----------

 - Build:
   * Pre-commit script/hook
   * Visual Studio 2013:
     + Add project for command-line tool
     + Pass /W4 cleanly
     + Pass /Wall cleanly
   * Create Visual Studio 2012 solution & projects

 - Distribution:
   * Pre-built OS X binaries
   * Pre-built Windows binaries
   * [Homebrew](http://brew.sh/) formula

 - Documentation:
   * Produce Boost-compatible documentation
   * Create `conf.py` (et al.) to enable ReadTheDocs

 - Testing:
   * Rewrite the majority of unit tests as a set of .in/.out files
   * Add exhaustive date/time formatting tests
   * Add way to specify expected failures; re-enable commented out tests

 - Optimization:
   * Compare benefit/cost of `-O`, `-O2`, `-O3` and `-Ofast`
   * Investigate using thread locals instead of statics for medium-expense objects
   * Investigate `-fvisibility-inlines-hidden`
   * Replace `ostream << string(a, b)` constructs with `std::ostream_iterator` + `std::copy`

 - Bindings:
   * Command-line:
     + Allow specifying formats option
     + Allow specifying debug option
     + Allow named input files
     + Allow named output files
     + [v1+] Allow using arbitrary programs as tags
     + [v1+] Allow using arbitrary programs as filters
   * Python:
     + Turn optional arguments to synth.Template into kwargs
     + Support is_safe, needs_autoescape, and expects_localtime in custom filters
     + Support for non-simple custom tags via token and parser objects
         1. Custom inclusion tags
         2. Custom assignment tags
         3. Custom tags with arbitrary blocks
     + Set docstrings where appropriate
     + Support for Python 3
   * Other:
     + Create `ruby` binding based on [Rice](http://rice.rubyforge.org/)
     + Create `go` binding based on [cgo](http://golang.org/cmd/cgo/)
     + Create `lua` binding
     + Create `node` binding
     + Create `php` binding
     + Create `c` binding
     + [v1+] Create `haskell` binding

 - Engines:
   * Django:
     + Implement multi-level inheritance
     + Support non-UTC timezones
     + Option to pre-load libraries, tags and filters
     + Implement missing date/time format specifiers
     + Make markers dynamically configurable
     + Consider using hex character entities (vs. named) to match Django's engine
   * SSI:
     + Implement additional tags from [Jigsaw](http://www.w3.org/Jigsaw/Doc/User/SSI.html)
   * Other:
     + Create `ctpp` engine
     + [v1+] Create `jinja2` engine
     + [v1+] Create `cheetah` engine
     + [v1+] Create `xslt` engine

 - Adapters:
   * Create `boost::tribool` adapter
   * Create `boost::fusion` sequence adapters
   * [v2] Create `c++11` type adapters
   * [v2] Create `c++14` type adapters

 - Templates:
   * Create new `file_template` (using `FILE* file`)
     + Add `render_to_file` to base_template
     + Add `render_file` to engines
   * Create `descriptor_template` (using `int descriptor`)
     + Add `render_to_descriptor` to base_template
     + Add `render_descriptor` to engines

 - Refactoring:
   * Replace all ``` `foo' ``` messages with ``` `foo` ```
   * Move `engine`s to own namespace
   * Make documentation comments uniformly `///`
   * Make `frame_type` and opaque wrapper for `match_type`
   * Experiment with embedding a slim, modularized version of Boost and using it by default
   * Move `render_tag` and `builtin_tags_` to `base_engine::kernel`
   * Remove all no-op tags (e.g. `cycle_as_silent_tag`)
   * Fold all variant tags into their main version (e.g. `load_from_tag`)
   * [v2] Create `c++11`/`c++14` branch
     + Translate macros to variadic templates
     + Replace `BOOST_FOREACH` with new `for` loop
     + Replace `BOOST_STATIC_CONSTANT` with `static const`
     + Replace `boost::assign` use with aggregate initializers
     + Remove complex redundant `typedef`s in favor of `auto`
     + Replace `<boost/cstdint.hpp>` with `<cstdint>`
     + Consider switching to unordered_map/unordered_set where possible
   * Experiment with turning `detail` namespaces into `struct`s with `private` members and `friend`s
   * [v1+] Add `AJG_PRAGMA` macro that invokes `_Pragma` or `__pragma` (MSVC) as needed
     + Add `AJG_PRAGMA(once)` to all header files and see if it speeds up compilations
   * [v1+] Sort `#include`s alphabetically
   * [v1+] Run entire C++ codebase through clang-format
   * Change `class` in template signatures to `typename`
   * [v1] Hygienicize and prefix all macros (and #undef private ones after use)
   * [v1] Reformat all `operator _()`'s to `operator_()`
   * [v1+] Remove header guard closing comments
   * [v1+] Remove namespace closing comments
   * [v2+] Factor out values & adapters into separate library for generic language interop

Frequently Asked Questions (FAQs)
---------------------------------

 - Q: Why does installation fail with the following error?

        #include <boost/python.hpp>
                ^
        1 error generated.
        error: command 'cc' failed with exit status 1

   A: You need to [install Boost.Python](#installing-boost) first.

License
-------

This library is distributed under the Boost [LICENSE](./LICENSE_1_0.txt).


[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/ajg/synth/trend.png)](https://bitdeli.com/free "Bitdeli Badge")
