##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)

from __future__ import print_function

import django.template as django
import synth
import sys

from django.template.loader import * # Fixes missing block & extends tags.
from django.conf import settings
from timeit import timeit

print('Loaded synth; version: %s' % synth.version(), file=sys.stderr)

if not settings.configured:
    settings.configure()

def main():
    print('=== Prepping ===')
    print('Directories:', directories)
    print('Data:', data)
    print('Files:', files)
    print('Debug:', debug)

    # Set the right directories:
    settings.TEMPLATE_DEBUG = debug
    settings.TEMPLATE_DIRS += directories
    synth.Template.set_default_options({
        'debug':       debug,
        'directories': directories,
    })

    print('=== Starting ===')
    print('Iterations:', iterations)
    for name in files: bench(name)
    print('Average: %2.1fx' % (sum(changes) / len(changes)))
    print('=== Finished ===')

debug = False
iterations = 1000
directories = (
    'tests/templates/django/blog',
    'tests/templates/django',
    '.',
)
files = [
    'base.html',
    'home.html',
    'index.html',
    'A.tpl',
    'B.tpl',
    'base.tpl',
    'C.tpl',
    'D.tpl',
    'derived.tpl',
    'empty.tpl',
    'layout.html',
    'messages.html',
    'variables.tpl',
    'X.tpl',
]
data = {
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

source = None
changes = []

def print_diff(a, b):
    import sys, subprocess, tempfile

    f = tempfile.NamedTemporaryFile(mode='w+')
    g = tempfile.NamedTemporaryFile(mode='w+')
    f.write(a)
    f.flush()
    g.write(b)
    g.flush()
    command = ['git', 'diff', '--no-index', '--color-words', f.name, g.name]
    try:
        subprocess.call(command, stdout=sys.stdout, stderr=sys.stderr)
    except subprocess.CalledProcessError as e:
        print('[Diff unavailable; please install git]')
        return

def bench(name):
    global source, changes
    source = '{% include "' + name + '" %}'

    # Ensure the output is the same:
    a = django.Template(source).render(django.Context(data))
    b = synth.Template(source, 'django').render_to_string(data)

    if a != b:
        print('WARNING: Outputs were not equal for file %s', repr(name))
        print('    - Django Output: %s' % repr(a))
        print('    - Synth Output:  %s' % repr(b))
        print_diff(a, b)

    x = timeit("bench_django()", number=iterations, setup="from __main__ import bench_django")
    y = timeit("bench_synth()",  number=iterations, setup="from __main__ import bench_synth")
    change = x / y
    changes.append(change)
    print('Django: %.3fs; Synth: %.3fs; +/-: %2.1fx; File: %s' % (x, y, change, repr(name)))

def bench_django():
    django_template = django.Template(source)
    django_template.render(django.Context(data))

def bench_synth():
    synth_template = synth.Template(source, 'django')
    synth_template.render_to_string(data)

main()
