##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

import sys

def get():
    return (context, golden, source, 'django', (True, '', {}, False, [], {}, [library_loader], []))

class Library(object):
    def __init__(self, tags={}, filters={}):
        self.tags = tags
        self.filters = filters

def library_loader(name):
    if name == 'tags.examples':
        return Library(tags={'x': None, 'y': None, 'z': None})
    elif name == 'example_filters':
        return Library(filters={'flip': flip})
    elif name in ('foo', 'bar', 'qux'):
        return Library()
    else:
        raise Exception('Library not found')

def flip(string):
    return ''.join(map(lambda c: c.upper() if c.islower() else c.lower(), string))


context = {'motto': 'May the Force be with you.'}
source = """

{% load x y z from tags.examples %}
{% load flip from example_filters %}
{% load foo bar qux %}

{{ motto }}
{{ motto|flip }}

"""
golden = """





May the Force be with you.
mAY THE fORCE BE WITH YOU.

"""
