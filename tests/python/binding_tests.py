##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

from datetime import datetime

def get():
    return (data, golden, source, 'django', None)

data = {
    'bool_true':        True,
    'bool_false':       False,
    'int_0':            0,
    'int_42':           42,
    'str_empty':        "",
    'str_test':         "test",
    'list_empty':       [],
    'list_12345':       [1, 2, 3, 4, 5],
    'dict_empty':       {},
    'dict_abc':         {'a': 'A', 'b': 'B', 'c': 'C'},
    'datetime_one':     datetime(1, 1, 1),
    'datetime_id4':     datetime(1776, 7, 4),
    'datetime_1984':    datetime(1984, 11, 2),
}
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

{% for k, v in str_empty %}Bad{% empty %}Good{% endfor %}
{% for k, v in list_empty %}Bad{% empty %}Good{% endfor %}
{% for k, v in dict_empty %}Bad{% empty %}Good{% endfor %}

"literal": {% for v in "literal" %}{{ v }}{% endfor %}
str_test: {% for v in str_test %}{{ v }}{% endfor %}
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

{{ datetime_one }}
{{ datetime_id4 }}
{{ datetime_1984 }}

{{ datetime_1984|timesince:datetime_id4 }}
{{ datetime_id4|timesince:datetime_1984 }}
{{ datetime_id4|timesince:datetime_id4 }}
{{ datetime_1984|timesince:datetime_1984 }}

{{ datetime_1984|timeuntil:datetime_id4 }}
{{ datetime_id4|timeuntil:datetime_1984 }}
{{ datetime_id4|timeuntil:datetime_id4 }}
{{ datetime_1984|timeuntil:datetime_1984 }}

"""
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
Good

"literal": literal
str_test: test
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

0001-01-01 00:00:00
1776-07-04 00:00:00
1984-11-02 00:00:00

0\xc2\xa0minutes
208\xc2\xa0years, 5\xc2\xa0months
0\xc2\xa0minutes
0\xc2\xa0minutes

208\xc2\xa0years, 5\xc2\xa0months
0\xc2\xa0minutes
0\xc2\xa0minutes
0\xc2\xa0minutes

"""
