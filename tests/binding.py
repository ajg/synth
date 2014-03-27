##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

import synth
from difflib import unified_diff

# TODO: Golden-file tests like these are brittle and ugly;
#       refactor and figure out how to reuse the native unit tests.

print('Loaded synth; version: ' + synth.version())

source = """
True:   {{ True }}; {% if True %}Good{% else %}Bad{% endif %}
False:  {{ False }}; {% if False %}Bad{% else %}Good{% endif %}

0:      {{ 0 }}; {% if 0 %}Bad{% else %}Good{% endif %}
42:     {{ 42 }}; {% if 42 %}Good{% else %}Bad{% endif %}

"":     {{ "" }}; {% if "" %}Bad{% else %}Good{% endif %}
"test": {{ "test" }}; {% if "test" %}Good{% else %}Bad{% endif %}

bool_true: {{ bool_true }}; {% if bool_true %}Good{% else %}Bad{% endif %}
bool_false: {{ bool_false }}; {% if bool_false %}Bad{% else %}Good{% endif %}

int_0: {{ int_0 }}; {% if int_0 %}Bad{% else %}Good{% endif %}
int_42: {{ int_42 }}; {% if int_42 %}Good{% else %}Bad{% endif %}

str_empty: {{ str_empty }}; {% if str_empty %}Bad{% else %}Good{% endif %}
str_test: {{ str_test }}; {% if str_test %}Good{% else %}Bad{% endif %}

list_empty: {{ list_empty }}; {% if list_empty %}Bad{% else %}Good{% endif %}
list_12345: {{ list_12345 }}; {% if list_12345 %}Good{% else %}Bad{% endif %}

dict_empty: {{ dict_empty }}; {% if dict_empty %}Bad{% else %}Good{% endif %}
dict_abc: {{ dict_abc }}; {% if dict_abc %}Good{% else %}Bad{% endif %}

{# FIXME: {% for k, v in str_empty %}Bad{% empty %}Good{% endfor %} #}
{% for k, v in list_empty %}Bad{% empty %}Good{% endfor %}
{% for k, v in dict_empty %}Bad{% empty %}Good{% endfor %}

"test": {% for v in "test" %}{{ v }}{% endfor %}
{# FIXME: str_test: {% for v in str_test %}{{ v }}{% endfor %} #}
list_12345: {% for v in list_12345 %}{{ v }}{% endfor %}
dict_abc: {% for v in dict_abc %}{{ v }}{% endfor %}

dict_abc: {% for k, v in dict_abc.items %}{{ k }}:{{ v }}{% endfor %}

{{ 0 or 0 }}
{{ 0 and 0 }}
{{ 0 or "foo" }}
{{ 0 and "foo" }}
{{ "foo" or 0 }}
{{ "foo" and 0 }}
{{ "foo" or "bar" }}
{{ "foo" and "bar" }}

""".encode('utf-8')

context = {
    'bool_true':  True,
    'bool_false': False,
    'int_0':      0,
    'int_42':     42,
    'str_empty':  "",
    'str_test':   "test",
    'list_empty': [],
    'list_12345': [1, 2, 3, 4, 5],
    'dict_empty': {},
    'dict_abc':   {'a': 'A', 'b': 'B', 'c': 'C'},
}

template = synth.Template(source, 'django')

print('Parsing succeeded!')

string = template.render_to_string(context)

print('Rendering succeeded!')

golden = """
True:   True; Good
False:  False; Good

0:      0; Good
42:     42; Good

"":     ; Good
"test": test; Good

bool_true: True; Good
bool_false: False; Good

int_0: 0; Good
int_42: 42; Good

str_empty: ; Good
str_test: test; Good

list_empty: []; Good
list_12345: [1, 2, 3, 4, 5]; Good

dict_empty: {}; Good
dict_abc: {&apos;a&apos;: &apos;A&apos;, &apos;c&apos;: &apos;C&apos;, &apos;b&apos;: &apos;B&apos;}; Good


Good
Good

"test": test

list_12345: 12345
dict_abc: acb

dict_abc: a:Ac:Cb:B

0
0
foo
0
foo
0
foo
bar

"""

if string != golden:
    diff = ''.join(unified_diff(golden.splitlines(True), string.splitlines(True)))
    raise Exception("MISMATCH:\n" + diff)

print('Matching succeeded!')
