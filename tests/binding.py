##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

import synthesis

print('Loaded synthesis; version: ' + synthesis.version())

source = """

True:   {{ True }}; {% if True %} GOOD {% else %} BAD {% endif %}
False:  {{ False }}; {% if False %} BAD {% else %} GOOD {% endif %}

0:      {{ 0 }}; {% if 0 %} BAD {% else %} GOOD {% endif %}
42:     {{ 42 }}; {% if 42 %} GOOD {% else %} BAD {% endif %}

"":     {{ "" }}; {% if "" %} BAD {% else %} GOOD {% endif %}
"test": {{ "test" }}; {% if "test" %} GOOD {% else %} BAD {% endif %}

bool_true: {{ bool_true }}; {% if bool_true %} GOOD {% else %} BAD {% endif %}
bool_false: {{ bool_false }}; {% if bool_false %} BAD {% else %} GOOD {% endif %}

int_0: {{ int_0 }}; {% if int_0 %} BAD {% else %} GOOD {% endif %}
int_42: {{ int_42 }}; {% if int_42 %} GOOD {% else %} BAD {% endif %}

str_empty: {{ str_empty }}; {#% if str_empty %} BAD {% else %} GOOD {% endif %#}
str_test: {{ str_test }}; {% if str_test %} GOOD {% else %} BAD {% endif %}

list_empty: {{ list_empty }}; {% if list_empty %} BAD {% else %} GOOD {% endif %}
list_12345: {{ list_12345 }}; {% if list_12345 %} GOOD {% else %} BAD {% endif %}

dict_empty: {{ dict_empty }}; {% if dict_empty %} BAD {% else %} GOOD {% endif %}
dict_abc: {{ dict_abc }}; {% if dict_abc %} GOOD {% else %} BAD {% endif %}

{#% for k, v in str_empty %} BAD {% empty %} GOOD {% endfor %#}
{% for k, v in list_empty %} BAD {% empty %} GOOD {% endfor %}
{% for k, v in dict_empty %} BAD {% empty %} GOOD {% endfor %}

"test": {% for v in "test" %}{{ v }}{% endfor %}
str_test: {#% for v in str_test %}{{ v }}{% endfor %#}
list_12345: {% for v in list_12345 %}{{ v }}{% endfor %}
dict_abc: {% for v in dict_abc %}{{ v }}{% endfor %}

dict_abc: {% for k, v in dict_abc.items %}{{ k }}:{{ v }}{% endfor %}

{% comment %}

{{ 0 or 0 }}
{{ 0 and 0 }}
{{ 0 or "foo" }}
{{ 0 and "foo" }}
{{ "foo" or 0 }}
{{ "foo" and 0 }}
{{ "foo" or "bar" }}
{{ "foo" and "bar" }}
{% endcomment %}
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
template = synthesis.Template(source, 'django')

print(template.render_to_string(context))

