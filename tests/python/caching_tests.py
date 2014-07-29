##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

directory = 'tests/templates/django/'

import synth

def get():
    return (data, golden, source, 'django', {
        'directories': [directory],
        'caching': (synth.CACHE_ALL | synth.CACHE_PER_PROCESS),
        'directories': [directory],
    })

data = {'foo': 1, 'bar': 2, 'qux': 3}
source = """Foo Bar Qux; {% include "variables.tpl" %}; {% include "variables.tpl" %}"""
golden = """Foo Bar Qux; \
foo: 1\nbar: 2\nqux: 3\n; \
foo: 1\nbar: 2\nqux: 3\n"""
