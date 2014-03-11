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

cat tpl.txt | synth -e django -c ctx.json
```

### Python

```python
import synth

def simple_tmpl_example():
    tpl = synth.Template('Howdy, <TMPL_VAR user>!', 'tmpl')
    ctx = {'user': 'Dolph Lundgren'}

    tpl.render_to_file("greeting.txt", ctx)
    # or, e.g.:
    return tpl.render_to_string(ctx)
```

### C++

```c++
#include <map>
#include <string>
#include <iostream>

#include <ajg/synth.hpp>

std::string simple_ssi_example() {
    using namespace ajg::synth;

    typedef string_template<char, ssi::engine<> > template_type;
    template_type const tpl("Howdy, <!--#echo var=\"user\" -->!");
    template_type::context_type ctx;
    ctx["user"] = "Dolph Lundgren";

    tpl.render(std::cout);
    // or, e.g.:
    tpl.render_to_file("greeting.txt", ctx);
    // or, e.g.:
    return tpl.render_to_string(ctx);
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

 - `g++` versions `4.2.1` & `4.6.3`
 - `clang++` version `3.3`, including Apple's `LLVM version 5.0`
 - `boost` versions `1.42`, `1.46` & `1.55`

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

        git clone https://github.com/ajg/synth.git && cd synth

 3. [Optional] Build the command-line program:

        scons synth # Add debug=1 to generate debugging symbols & disable optimizations.

 4. [Optional] Build (and install) the Python module:

        python setup.py install

(Pre-built binaries are [in the works](#future-work).)

### Using Pip:

Install [Boost](#installing-boost), then:

    pip install synth

Components
----------

### Engines

 - [`django`](#django-engine): An implementation of [Django Templates](https://docs.djangoproject.com/en/dev/topics/templates/).
 - [`ssi`](#ssi-engine): An implementation of [Server Side Includes](http://httpd.apache.org/docs/current/howto/ssi.html).
 - [`tmpl`](#tmpl-engine): An implementation of [Perl's HTML::Template](http://html-template.sourceforge.net/html_template.html).

### Bindings

 - `command_line`
 - `python`

### Templates

 - `file_template`
 - `multi_template`
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
 - `ptree`
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

### Formats

 - `ini`
 - `json`
 - `xml`

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
 - `django::load_tag`
 - `django::load_from_tag`
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
 - `django::library_tag`

### Filters

 - `django::add_filter`
 - `django::addslashes_filter`
 - `django::capfirst_filter`
 - `django::center_filter`
 - `django::cut_filter`
 - `django::date_filter`
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
 - `django::pprint_filter`
 - `django::random_filter`
 - `django::removetags_filter`
 - `django::rjust_filter`
 - `django::safe_filter`
 - `django::safeseq_filter`
 - `django::slice_filter`
 - `django::slugify_filter`
 - `django::stringformat_filter`
 - `django::striptags_filter`
 - `django::time_filter`
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

 - `tmpl::comment_tag` (Technically, part of [`ctpp`](http://ctpp.havoc.ru/en/))
 - `tmpl::if_tag`
 - `tmpl::include_tag`
 - `tmpl::loop_tag`
 - `tmpl::unless_tag`
 - `tmpl::variable_tag`

Future Work
-----------

 - Core:
   * Expand documentation
 - Distribution:
   * [Homebrew](http://brew.sh/) formula
   * Pre-built OS X binaries
   * Pre-built Windows binaries
   * Compare benefit/cost of `-O`, `-O2`, `-O3` and `-Ofast`
   * Django template loaders
   * Create Visual Studio solution & project using Scons
 - Bindings:
   * `ruby` binding based on [Rice](http://rice.rubyforge.org/)
   * `go` binding based on [cgo](http://golang.org/cmd/cgo/)
   * `lua` binding
   * `node` binding
   * `php` binding
   * `c` binding
   * `haskell` binding
 - Engines:
   * `cheetah` engine
   * `ctpp` engine
   * `multi_engine`
 - Adapters:
   * `tribool` adapter
   * `c++11` type adapters
   * `c++14` type adapters
 - Templates:
   * `descriptor_template`
   * Clean up and make `multi_template` public
 - Django:
   * Named arguments (e.g. for tags and filters)
   * Option to pre-load libraries, tags and filters
 - Command-line:
   * Allow specifying formats option
   * Allow specifying debug option
   * Allow using arbitrary programs as tags
   * Allow using arbitrary programs as filters
 - Refactoring:
   * Move `*_template`s to own namespace
   * Move `engine`s to own namespace
   * Create `c++11`/`c++14` branch
     + Translate macros to variadic templates
     + Replace `BOOST_FOREACH` with new `for` loop
     + Remove needlessly configurable (especially defaulted) template parameters
   * Sort `#include`s alphabetically
   * Rename `Array`/`array_type` to `Sequence`/`sequence_type`
   * Introduce `Mapping`/`mapping_type` to replace hard-coded `std::map`s
     + Consider making `context`s top-level `value`s instead of `map`s
     + Consider switching to unordered_map/unordered_set where possible
   * Test or drop support for non-`char` types (e.g. `wchar_t`)
   * Hygienicize and prefix all macros (and #undef private ones after use)
   * Rename `this_type` to `self_type` unless it's actually a pointer
   * Reformat all operator _()'s to operator_()

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
