##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

import sys
from functools import partial

def get():
    return (context, golden, source, 'django', ('', {}, False, [], {}, [library_loader], []))

class Library(object):
    def __init__(self, tags={}, filters={}):
        self.tags = tags
        self.filters = filters

def dump(n, x): print n, '=', x; return x

def render_block(segments, match, index=0):
    # print '### render_block(', segments, match, ')'
    return segments[index][1](match)

def bind_segments(segments, match):
    # print '### bind_segments(', segments, match, ')'
    return [(pieces, partial(renderer, match)) for pieces, renderer in segments]

def simple_tag(fn):
    return (lambda segments: (lambda match, *args, **kwargs: str(fn(*args, **kwargs))), ())

def block_tag(name, fn):
    return (lambda segments: (lambda match, *args, **kwargs: str(fn(render_block(segments, match), *args, **kwargs))), ('end' + name,))

def variadic_tag(name, fn, expected):
    return (lambda segments: (lambda match, *args, **kwargs: str(fn(bind_segments(segments, match), *args, **kwargs))), expected + ('end' + name,))

dummy_tag = (None, ())
dummy_filter = None

def library_loader(name):
    if name == 'empty_library':
        return Library()

    elif name == 'dummy.tags.and.filters':
        return Library(
            tags={'a': dummy_tag, 'b': dummy_tag, 'c': dummy_tag},
            filters={'x': dummy_filter, 'y': dummy_filter, 'z': dummy_filter},
        )

    elif name == 'test_tags':
        return Library(tags={
            # Monadic ('simple') tags:
            'answer_to_life': simple_tag(answer_to_life), # nullary
            'identity':       simple_tag(identity),       # unary
            'ackermann':      simple_tag(ackermann),      # binary
            'add':            simple_tag(add),            # n-ary
            # Polyadic tags:
            'encode':        block_tag('encode', encode),                    # dyadic
            'decode':        block_tag('decode', decode),                    # dyadic
            'unless':        variadic_tag('unless', unless, ('otherwise',)), # variadic (dyadic or triadic)
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

def encode(s, arg):
    # print '### encode(', repr(s), repr(arg), ')'
    return s.encode(arg)

def decode(s, arg):
    # print '### decode(', repr(s), repr(arg), ')'
    return s.decode(arg)

def unless(segments, arg):
    # print '### unless(', repr(segments), repr(arg), ')'
    pieces, renderer = segments[1 if arg else 0]
    return renderer()


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
{% load encode decode unless from test_tags %}
({% answer_to_life %})
({% identity 'wow' %})
({% ackermann 3 4 %})
({% add %})
({% add 1.1 %})
({% add 1.1 2.2 %})
({% add 1.1 2.2 3.3 %})
({% encode 'rot13' %}Hello Kitty{% endencode %})
({% encode 'rot13' %}{{ 'foo'|upper }}{% endencode %})
({% encode 'rot13' %}{% if True %}Hello Kitty{% endif %}{% endencode %})
({% decode 'rot13' %}|{% encode 'rot13' %}Hello Kitty{% endencode %}|{% enddecode %})
({% encode 'rot13' %}|{% encode 'rot13' %}Hello Kitty{% endencode %}|{% endencode %})
({% unless True%}A{% otherwise %}B{% endunless %})
({% unless False%}A{% otherwise %}B{% endunless %})
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
(Uryyb Xvggl)
(SBB)
(Uryyb Xvggl)
(|Hello Kitty|)
(|Hello Kitty|)
(B)
(A)
"""


