##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)

from __future__ import print_function

import django.template as django
import synth

from django.template.loader import * # Fixes missing block & extends tags.
from django.conf import settings
from timeit import timeit

if not settings.configured:
    settings.configure()

print('Loaded synth; version: %s' % synth.version())

iterations  = 1000
directories = ('tests/templates/django/blog',)
source      = '{% include "home.html" %}'
data        = {
    'foo': 1,
    'bar': 2,
    'qux': 3,
    'user': 'Dolph Lundgren',
    'subtitle': 'A blog about movies and TV shows',
    'categories': [
        {'title': 'Movies', 'url': 'http://example.org/categories/movies'},
        {'title': 'Shows', 'url': 'http://example.org/categories/shows'},
    ],
    'posts': [
        {'title': 'Rocky', 'url': 'http://example.org/posts/1', 'category': 'movies'},
        {'title': 'Blackjack', 'url': 'http://example.org/posts/2', 'category': 'shows'},
    ],
}

def run():
    # Display the parameters:
    print('=== Prepping ===')
    print('Iterations: %d' % iterations)
    print('Directories: ' + repr(directories))
    print('Source: ' + repr(source))
    print('Data: ' + repr(data))

    # Set the right directories:
    settings.TEMPLATE_DIRS += directories
    synth.Template.set_default_options({'directories': directories})

    # Ensure the output is the same:
    a = django.Template(source).render(django.Context(data))
    b = synth.Template(source, 'django').render_to_string(data)

    if a != b:
        print('Django Output: ' + repr(a))
        print('Synth Output:  ' + repr(b))
        raise Exception('Outputs were not equal')

    print('=== Starting ===')
    x = timeit("bench_django()", number=iterations, setup="from __main__ import bench_django")
    y = timeit("bench_synth()",  number=iterations, setup="from __main__ import bench_synth")
    print('Django: %.3fs' % x)
    print('Synth:  %.3fs' % y)
    print('+/-: %.1fx' % (x / y))
    print('=== Finished ===')

def bench_django():
    django_template = django.Template(source)
    django_template.render(django.Context(data))

def bench_synth():
    synth_template = synth.Template(source, 'django')
    synth_template.render_to_string(data)

run()
