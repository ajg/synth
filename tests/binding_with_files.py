##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

import synth

print('Loaded synth; version: ' + synth.version())

directory = 'samples/django/templates/'
source = open(directory + 'layout.html').read().encode('utf-8')


class User:
    def __init__(self):
        self.is_staff = False
        self.is_authenticated = True

context = {
    'request': {
        'user': User(),
    },
    'STATIC_URL': '/static/',
    'title': 'Default Title',
    'messages': ["Foo", "Bar", "Qux"],
}

template = synth.Template(source, 'django', True, 'INVALID_VALUE', [directory])

print(template.render_to_string(context))

