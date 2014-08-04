[![Synth]](https://github.com/ajg/synth)
========================================

A Powerful C++ Templating Framework with a [command-line tool](#command-line) and [Python module](#python), written by [Alvaro J. Genial](http://alva.ro).

[![Build Status]](https://travis-ci.org/ajg/synth)

Synopsis
--------

Synth is a template framework—a set of components that can be mixed and matched to build the right functionality; furthermore, components are loosely-coupled, designed to be both extensible and replaceable.

Status
------

Version 1 is released and stable.

Motivation
----------

Synth blurs the line between compile-time and runtime, and it does so by blending three worlds: (a) the static C++ type system; (b) the dynamic values that need to be manipulated and formatted, including those from other languages; and (c) the templates to do so. The name is an allusion to this synthesis process, which combines values to generate new ones (streams, files, strings, numbers, etc.)

Examples
--------

### Command-line

```shell
echo '{"user": "Dolph Lundgren"}' > context.json
echo 'Howdy, {{ user }}!' > template.txt
cat template.txt | synth -e django -c context.json
```

_Or on Windows:_

```
echo {"user":"Dolph Lundgren"} > context.json
echo Howdy, {{ user }}! > template.txt
type template.txt | synth -e django -c context.json
```

### Python

```python
import synth, sys

def simple_tmpl_example():
    t = synth.Template('Howdy, <TMPL_VAR user>!', 'tmpl')
    c = {'user': 'Dolph Lundgren'}

    # Render to different destinations:
    t.render_to_path("greeting.txt", c)
    t.render_to_file(sys.stdout, c)
    print(t.render_to_string(c))
```

### [C++](./examples/simple_ssi.cpp)

```c++
#include <map>
#include <string>
#include <iostream>
#include <ajg/synth.hpp>

typedef ajg::synth::default_traits<char>                    traits_type;
typedef ajg::synth::engines::ssi::engine<traits_type>       engine_type;
typedef ajg::synth::templates::string_template<engine_type> template_type;

int main() {
    // Parse the template.
    template_type const t(
        "Howdy, <!--#echo var='user' -->! "
        "Your balance is <!--#echo var='points' -->.");

    // Create some data.
    std::map<std::string, engine_type::value_type> m;
    m["user"] = "Dolph Lundgren";
    m["points"] = 42;

    // Render to different destinations:
    t.render_to_stream(std::cout, m);
    t.render_to_path("greeting.txt", m);
    std::cout << t.render_to_string(m);
    return 0;
}
```

Reference
---------

### Command-line

    USAGE: synth [OPTIONS...]
    Options:
      -h,      --help              print help message
      -v,      --version           print library version
      -c file, --context=file      contextual data             *.{ini,json,xml}
      -e name, --engine=name       template engine             {django,ssi,tmpl}
      -d path, --directory=path    template location(s)        (default: '.')

Installation
------------

### Via System Package Manager

#### Using [Homebrew](http://brew.sh/):

    brew install https://raw.github.com/ajg/synth/master/synth.rb
    # Note that you must append `--with-python` to install the Python module.

#### Using [Chocolatey](https://chocolatey.org/):

    choco install synth

(Support for other system package managers like Apt or Yum is [welcome](#future-work).)

### Via Python Package Manager

#### Using [Pip](http://www.pip-installer.org/):

    pip install synth # Prefix with `sudo` if needed.

#### Using [Easy Install](https://wiki.python.org/moin/EasyInstall):

    easy_install synth # Prefix with `sudo` if needed.

### Manually

#### From Binaries:

(Pre-built binary releases for common platforms are [welcome](#future-work).)

#### From Source:

 1. Ensure you have the following:
   - A tool to get the source (`git`, `curl`, a browser, etc.)
   - A system to build it with ([SCons](http://scons.org) or Visual Studio)
   - A C++ compiler (`clang`, `gcc`, `msvc`, etc.) In particular, Synth is known to compile with:
     * `gcc` versions `4.2.1`, `4.6.3` and `4.7.2`
     * `clang` versions `3.0` and `3.3`
     * `msvc` version `12.0` (VS2013)
 2. Get the source (e.g. the latest, as shown here):

        git clone --depth 1 --recursive https://github.com/ajg/synth.git && cd synth

 3. *Optionally*, build the [command-line tool](#command-line):

        scons synth # Add debug=1 to generate debugging symbols & disable optimizations.

    (Note that if you are hell bent on it, you can use a different version of Boost; see [Infrequently Asked Questions](#infrequently-asked-questions).)

 4. *Optionally*, build (and install) the [Python module](#python):

        python setup.py install # Prefix with `sudo` if needed.


<!-- TODO: Figure out how to include submodules in tarballs & zipballs.
 2. Get the source (pick your poison):
      - With Git:

            git clone --depth 1 --recursive https://github.com/ajg/synth.git && cd synth
      - With Curl:

            curl -L https://github.com/ajg/synth/archive/master.tar.gz | tar xz && cd synth-master
      - Otherwise, try using https://github.com/ajg/synth/archive/master.zip
-->

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

 - `buffer_template`
 - `path_template`
 - `stream_template`
 - `string_template`

### Adapters

 - `array`
   * `T[N]`
   * `T[]`
   * `boost::array`
 - `bool`
   * `bool`
 - `complex`
   * `std::complex`
 - `container`
   * `deque`
     + `std::deque`
   * `list`
     + `std::list`
   * `map`
     + `std::map`
     + `std::multimap`
   * `set`
     + `std::set`
     + `std::multiset`
   * `stack`
     + `std::stack`
   * `vector`
     + `std::vector`
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
   * `wchar_t`            (When available, unless disabled by `AJG_SYNTH_CONFIG_NO_WCHAR_T`)
   * `long long`          (When available, unless disabled by `AJG_SYNTH_CONFIG_NO_LONG_LONG`)
   * `long long unsigned` (When available, unless disabled by `AJG_SYNTH_CONFIG_NO_LONG_LONG`)
   * `__int64`            (`msvc`-only.)
   * `__int64 unsigned`   (`msvc`-only.)
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
   * `void*`
   * `T*`
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
   * `wchar_t*`   (When available, unless disabled by `AJG_SYNTH_CONFIG_NO_WCHAR_T`)
   * `wchar_t[N]` (When available, unless disabled by `AJG_SYNTH_CONFIG_NO_WCHAR_T`)
 - `utility`
   * `std::pair`
 - `variant`
   * `boost::variant`

### Base Components

 - `adapters::base_adapter`
 - `bindings::base_binding`
 - `engines::base_engine`
 - `templates::base_template`

### Other Components

 - `engines::context`
 - `engines::options`
 - `engines::state`
 - `engines::value`
 - `templates::cache`

### [Version](./ajg/synth/version.hpp)

 - `AJG_SYNTH_VERSION`        (e.g. `1.2.3`)
 - `AJG_SYNTH_VERSION_MAJOR`  (e.g. `1`)
 - `AJG_SYNTH_VERSION_MINOR`  (e.g. `2`)
 - `AJG_SYNTH_VERSION_PATCH`  (e.g. `3`)
 - `AJG_SYNTH_VERSION_SEQ`    (e.g. `(1)(2)(3)`)
 - `AJG_SYNTH_VERSION_TUPLE`  (e.g. `(1, 2, 3)`)
 - `AJG_SYNTH_VERSION_ARRAY`  (e.g. `(3, (1, 2, 3))`)
 - `AJG_SYNTH_VERSION_STRING` (e.g. `"1.2.3"`)

### [Configuration](./ajg/synth/config.hpp)

 - `AJG_SYNTH_CONFIG_NO_WCHAR_T`        (default: automatically determined)
 - `AJG_SYNTH_CONFIG_NO_LONG_LONG`      (default: automatically determined)
 - `AJG_SYNTH_CONFIG_NO_DEBUG`          (default: automatically determined)
 - `AJG_SYNTH_CONFIG_NO_WINDOWS_H`      (default: not defined)
 - `AJG_SYNTH_CONFIG_DEFAULT_CHAR_TYPE` (default: `char`)
 - `AJG_SYNTH_CONFIG_MAX_FRAMES`        (default: `1024`)
 - `AJG_SYNTH_CONFIG_HANDLE_ASSERT`     (default: `BOOST_ASSERT`)
 - `AJG_SYNTH_CONFIG_HANDLE_EXCEPTION`  (default: `boost::throw_exception`)

### Context Data Input Formats

 - `ini`
 - `json`
 - `xml`

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
 - `django::builtin_tags::library_tag`
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

### Formats

 - `context::formats["TEMPLATE_STRING_IF_INVALID"]` (default: `""`)
 - `context::formats["DATE_FORMAT"]`                (default: `"N j, Y"`)
 - `context::formats["DATETIME_FORMAT"]`            (default: `"N j, Y, P"`)
 - `context::formats["MONTH_DAY_FORMAT"]`           (default: `"F j"`)
 - `context::formats["SHORT_DATE_FORMAT"]`          (default: `"m/d/Y"`)
 - `context::formats["SHORT_DATETIME_FORMAT"]`      (default: `"m/d/Y P"`)
 - `context::formats["TIME_FORMAT"]`                (default: `"P"`)
 - `context::formats["YEAR_MONTH_FORMAT"]`          (default: `"F Y"`)

_(Note: Django's `TEMPLATE_DEBUG` and `TEMPLATE_DIRS` are handled through `options::debug` and `options::directories`, respectively.)_

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

### Formats

 - `context::formats["echomsg"]`  (default: `"(none)"`)
 - `context::formats["errormsg"]` (default: `"[an error occurred while processing this directive]"`)
 - `context::formats["sizefmt"]`  (default: `"bytes"`)
 - `context::formats["timefmt"]`  (default: `"%A, %d-%b-%Y %H:%M:%S %Z"`)

TMPL Engine
-----------

### Tags

 - `tmpl::builtin_tags::comment_tag` (Technically, part of [`ctpp`](http://ctpp.havoc.ru/en/))
 - `tmpl::builtin_tags::if_tag`
 - `tmpl::builtin_tags::include_tag`
 - `tmpl::builtin_tags::loop_tag`
 - `tmpl::builtin_tags::unless_tag`
 - `tmpl::builtin_tags::variable_tag`

Options
-------

 - `options::debug`       (default: `false`)
 - `options::directories` (default: `"."`)
 - `options::libraries`   (for dynamic tags & filters)
 - `options::loaders`     (for dynamic library loading)
 - `options::resolvers`   (for dynamic URL resolution and reversal)
 - `options::caching`     (a bit mask of caching behavior)
   * `caching_none`
   * `caching_all`
   * `caching_paths`
   * `caching_buffers`
   * `caching_strings`
   * `caching_per_thread`
   * `caching_per_process`

Future Work
-----------

 - Build:
   * Pre-commit script/hook
   * GCC:
     + [4.9+] Add `-fsanitize=undefined` when available
   * Visual Studio:
     + [2013] Pass /W4 cleanly
     + [2013] Pass /Wall cleanly
     + [2012] Create solution & projects

 - Distribution:
   * RPM package
   * Apt package
   * Yum package
   * MacPort support
   * Standalone pre-built Darwin binaries
   * Standalone pre-built Windows binaries
   * MSI installer

 - Documentation:
   * Produce Boost-compatible documentation
   * Create `conf.py` (et al.) to enable ReadTheDocs

 - Testing:
   * Rewrite the majority of unit tests as a set of `.in`/`.out` files
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
     + [v1] Set docstrings where appropriate
     + Support is_safe, needs_autoescape, and expects_localtime in custom filters
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
     + Make markers dynamically configurable
     + Native `i18n` library
     + Native `l10n` library
     + Native `tz` library
     + Native `static` library
   * SSI:
     + Implement additional tags from [Jigsaw](http://www.w3.org/Jigsaw/Doc/User/SSI.html)
   * TMPL:
     + Consider renaming `html`
   * Other:
     + Create `ctpp` engine
     + [v1+] Create `jinja2` engine
     + [v1+] Create `cheetah` engine
     + [v1+] Create `xslt` engine

 - Adapters:
   * Adapter for `boost::tuple`
   * Adapter for `boost::any`
   * Adapter for `boost::function`
   * Adapter for `boost::tribool`
   * Adapters for `boost::fusion` sequences
   * Adapters for `boost::local_time` types
   * Adapter for `boost::intrusive_ptr`
   * Support for `boost::enable_shared_from_this`
   * [v2] Adapters for `c++11` types
   * [v2] Adapters for `c++14` types

 - Templates:
   * Create new `file_template` (using `FILE* file`)
     + Add `render_to_file` to base_template
     + Add `render_file` to engines
   * Create `descriptor_template` (using `int descriptor`)
     + Add `render_to_descriptor` to base_template
     + Add `render_descriptor` to engines

 - Refactoring:
   * Make documentation comments uniformly `///`
   * Replace `BOOST_ASSERT` with `AJG_SYNTH_ASSERT`
   * Replace local version of Boost with minimal, shallow submodules once boostorg/boost is ready
   * Move `render_tag` and `builtin_tags_` to `base_engine::kernel`
   * Move `render_block` to `base_engine::kernel`
   * Remove all no-op tags (e.g. `cycle_as_silent_tag`)
   * Fold all variant tags into their main version (e.g. `load_from_tag`)
   * [v2] Create `c++11`/`c++14` branch
     + Translate macros to variadic templates
     + Replace `BOOST_FOREACH` with new `for` loop
     + Replace `boost::assign` use with aggregate initializers
     + Remove complex redundant `typedef`s in favor of `auto`
     + Replace `<boost/cstdint.hpp>` with `<cstdint>`
     + Replace Boost.Random with `<random>`
     + Replace Boost.Atomic with `<atomic>`
     + Consider switching to unordered_map/unordered_set where possible
   * [v1+] Add `AJG_SYNTH_PRAGMA` macro that invokes `_Pragma` or `__pragma` (MSVC) as needed
     + Add `AJG_SYNTH_PRAGMA(once)` to all header files and see if it speeds up compilations
   * [v1+] Sort `#include`s alphabetically
   * [v1+] Run entire C++ codebase through clang-format
   * Change `class` in template signatures to `typename`
   * [v1] Reformat all `operator _()`'s to `operator_()`
   * [v1+] Remove header guard closing comments
   * [v1+] Remove namespace closing comments
   * [v2+] Factor out values & adapters into separate library for generic language interop

Infrequently Asked Questions
----------------------------

 - Q: Can I use a version of [Boost](http://boost.org) other than the one bundled?
   * A: Yes, you can, though note that unless you're already using Boost in your project, there is usually no good reason to; anyway:
     + On most systems, you can build Synth with the system's Boost by passing `boost=system` to `scons`.
     + On Windows, you'll need to edit the project file(s) in Visual Studio and set the include and library directories to point to the existing Boost installation.
     + The [Python module](#python) does not support this option yet.

 - Q: How can I install a system-wide version of [Boost](http://boost.org)?
   * A: Here are some suggestions:
     + Using Homebrew: `brew install boost` (Append `--with-python` for Boost.Python support.)
     + Using Apt: `sudo apt-get install libboost-all-dev`
     + Using Yum: `sudo yum install boost-devel`
     + On Windows, try `PM> Install-Package boost` in the Package Manager Console

License
-------

This library is distributed under the Boost [LICENSE](./LICENSE_1_0.txt).

[Synth]:        ./synth.gif
[Build Status]: https://travis-ci.org/ajg/synth.png?branch=master
