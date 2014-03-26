
{{some_strings[2]}}, {{some_strings[1]}}

Char Pointer: {{ char_pointer }}

Uh oh {{some_strings}},

{% for x, y in an_int_vector %}
    x: {{x}}
    y: {{y}}
{% empty %}
   for: Bad
{% endfor %}

Hello {{X}} ; {{an_int_vector}} ; {{a_shared_ptr}} ; {{a_scoped_ptr}},
world, {{Y}} is great: {{a_string_array}}, {{an_auto_ptr}}, {{some_vectors}}.
A complex float: {{a_complex_float}}.
An optional float: {{an_optional_float}}.

xml_var: {{ xml_var }}
xml_var: {{ xml_var|safe }}

{% block a_block %}
This is a block
{% endblock a_block %}

{% include 'samples/django/empty.tpl' %}

Included:
{% include 'samples/django/included.tpl' %}

{% firstof nonextant a_false a_pair a_deque %}
{% firstof aa bb cc 'FALLBACK' %}
{% firstof aa a_true 'FALLBACK' %}
