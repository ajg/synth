##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

import synth
from difflib import unified_diff

print('Loaded synth; version: ' + synth.version())

class User:
    def __init__(self):
        self.is_staff = False
        self.is_authenticated = True

directory = 'tests/templates/django/'

context = {
    'request': {
        'user': User(),
    },
    'STATIC_URL': '/static/',
    'title': 'Default Title',
    'messages': ["Foo", "Bar", "Qux"],
}
source = open(directory + 'layout.html').read().encode('utf-8')

template = synth.Template(source, 'django', True, 'INVALID_VALUE', {}, False, [directory])
print('Parsing succeeded!')

string = template.render_to_string(context)
print('Rendering succeeded!')

golden = """
"""

if string != golden:
    diff = ''.join(unified_diff(golden.splitlines(True), string.splitlines(True)))
    raise Exception("MISMATCH:\n" + diff)

print('Matching succeeded!')
