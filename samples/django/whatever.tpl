
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

{% for a, b in a_string_array %}a: {{a}} b: {{b}} {% endfor %}

{{X|escape}}

{{ a_string_array|first }}
{{ a_string_array|last }}
{{ a_string_array|length }}
{{ a_string_array|length_is:'5' }}
{{ a_string_array|random }}
{{ a_string_array|join:'_' }}

{{ a_few_lines|linenumbers}}
{{ a_few_lines|linebreaksbr }}
{{ more_lines|linebreaks }}
{{ a_bin_string|escapejs }}

{{ an_int_vector|slice:'0:9'}}
{{ an_int_vector|slice:':9'}}
{{ an_int_vector|slice:':'}}
{{ an_int_vector|slice:'0:'}}
{{ an_int_vector|slice:'2:6'}}
{{ an_int_vector|slice:'-6:-2'}}

{{ an_int_vector|make_list}}

{{tag_list|join:', '}}
{{tag_list|safe|join:', '}}
{{tag_list|safeseq|join:', '}}
