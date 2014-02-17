##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

import synthesis

print('Loaded synthesis; version: ' + synthesis.version())

source = """
foo: {{ foo }}; {% if foo %} yes {% else %} <p>no. {% endif %}
bar: {{ bar }}; {% if bar %} yes {% else %} <p>no. {% endif %}
qux: {{ qux }}; {% if qux %} yes {% else %} <p>no. {% endif %}

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

context = {'foo': True, 'bar': 42, 'qux': [1,2,3,4,5]}
template = synthesis.Template(source, 'django')

print(template.render_to_string(context))

