##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
##  http://www.boost.org/LICENSE_1_0.txt).

import synthesis

print('Loaded synthesis; version: ' + synthesis.version())

source = """
foo: {{ foo }}; {% if foo %} yes {% else %} <p>no {% endif %}
bar: {{ bar }}; {% if bar %} yes {% else %} <p>no {% endif %}
qux: {{ qux }}; {% if qux %} yes {% else %} <p>no {% endif %}
True: {{ True }}; {% if True %} yes {% else %} <p>no {% endif %}
False: {{ False }}; {% if False %} yes {% else %} <p>no {% endif %}
"": {{ "" }}; {% if "" %} yes {% else %} <p>no {% endif %}
0: {{ 0 }}; {% if 0 %} yes {% else %} <p>no {% endif %}

{% for k, v in "test" %} {{ k }} - {{ v }} {% endfor %}

{% for k, v in qux %} {{ k }} - {{ v }} {% endfor %}

{% for k, v in qux2 %} {{ k }} - {{ v }} {% endfor %}

{% for k, v in empty1 %} {{ k }} - {{ v }} {% empty %} empty1 {% endfor %}
{% for k, v in empty2 %} {{ k }} - {{ v }} {% empty %} empty2 {% endfor %}

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

context = {'foo': True, 'bar': 42, 'qux': [1,2,3,4,5], 'qux2': {'a': 'A', 'b': 'B', 'c': 'C'}, 'empty1': [], 'empty2': {}}
template = synthesis.Template(source, 'django')

print(template.render_to_string(context))

