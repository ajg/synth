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
    if name in ('empty_library'):
        return Library()

    elif name == 'dummy.tags.and.filters':
        return Library(
            tags={'a': None, 'b': None, 'c': None},
            filters={'x': None, 'y': None, 'z': None},
        )

    elif name == 'test_tags':
        return Library(tags={
            # niladic ('simple') tags:
            'ackermann':      ackermann,      # nullary
            'identity':       identity,       # unary
            'answer_to_life': answer_to_life, # binary
            'add':            add,            # n-ary
            # polyadic tags:
            # 'rot13':        rot13,          # monadic
            # '?':            ?,              # dyadic
            # 'sizable':      sizable,        # triadic
            # 'unless':       unless,         # variadic (monadic or dyadic)
        })

    elif name == 'test_filters':
        return Library(filters={'flip': flip})

    else:
        raise Exception('Library not found')

def flip(s):
    return ''.join(map(lambda c: c.upper() if c.islower() else c.lower(), s))

def answer_to_life():
    return 42

def identity(x):
    return x

def ackermann(m, n):
    if m == 0:
        return n + 1
    elif n == 0:
        return ackermann(m - 1, 1)
    else:
        return ackermann(m - 1, ackermann(m, n - 1))

def add(*args):
    return sum(args)



context = {'motto': 'May the Force be with you.'}
source = """\
{% load empty_library %}
{% load a x b y c z from dummy.tags.and.filters %}
{% load flip from test_filters %}
{{ motto }}
{{ motto|flip }}
{% load ackermann from test_tags %}
{% load identity from test_tags %}
{% load answer_to_life from test_tags %}
{% load add from test_tags %}
({% answer_to_life %})
({% identity 'wow' %})
({% ackermann 3 4 %})
({% add %})
({% add 1.1 %})
({% add 1.1 2.2 %})
({% add 1.1 2.2 3.3 %})
"""
golden = """\



May the Force be with you.
mAY THE fORCE BE WITH YOU.




(42)
(wow)
(125)
(0)
(1.1)
(3.3)
(6.6)
"""


