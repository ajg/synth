
Char Pointer: {{ char_pointer }}

{% ifequal 6 6 %} THEY EQUAL {% endifequal %}
{% ifequal 5 6 %} {% else %} THEY DON'T EQUAL {% endifequal %}
{% ifnotequal 5 6 %} THEY EQUAL {% endifnotequal %}
{% ifnotequal 6 6 %} {% else %} THEY DON'T EQUAL {% endifnotequal %}

Uh oh {{some_strings}},

{##}
{# a #}
{# {# #}
{% if 0 %}{% else %}HEYA!{% endif %}

{# foo #}

{% comment %} a
multiline
comment
whoo {% endcomment %}

{% autoescape on %}

    A deque: {{a_deque}}.
    A pair: {{a_pair}}.
    A map: {{a_map}}.
    A scoped array: {{a_scoped_array}}.

{% endautoescape %}

{% for x, y in an_int_vector %}
    x: {{x}}
    y: {{y}}
{% empty %}
   for: Bad
{% endfor %}

Date: {% now "%Y-%m-%d %H:%M:%S%F%Q" %}
Hello {{X}} ; {{an_int_vector}} ; {{a_shared_ptr}} ; {{a_scoped_ptr}},
    {% if True %} Good {% endif %}{% if False %} Bad {% endif %}
    {% if True %} Good {% else %} Bad {% endif %}
    {% if False %} Bad {% else %} Good {% endif %}
    {% if 1 %} Good {% endif %}{% if False %} Bad {% endif %}
    {% if 1 %} Good {% else %} Bad {% endif %}
    {% if 0 %} Bad {% else %} Good {% endif %}
world, {{Y}} is great: {{a_string_array}}, {{an_auto_ptr}}, {{some_vectors}}.
A complex float: {{a_complex_float}}.
An optional float: {{an_optional_float}}.

xml_var: {{ xml_var }}
xml_var: {{ xml_var|safe }}

Included:
{% include "samples/django/included.tpl" %}


{% firstof nonextant a_false a_pair a_deque %}
{% firstof aa bb cc "FALLBACK" %}
{% firstof aa a_true "FALLBACK" %}


{{X|upper}}
{{X|escape}}
{# {{X|pluralize:"en"}} #}

{{21|divisibleby:"3"}}
{{"1-800-COLLECT"|phone2numeric}}
{{"/this should/be encoded ^ because @ is not an option $ ()"|urlencode}}

{% for a, b in a_string_array %}a: {{a}} b: {{b}} {% endfor %}
{{ a_string_array|first }}
{{ a_string_array|last }}
{{ a_string_array|length }}
{{ a_string_array|length_is:"5" }}
{{ a_string_array|random }}
{{ a_string_array|join:"_" }}

{{ "Joel is a slug"|slugify }}
{{ "foo fa fa"|capfirst }}
{{ "5"|add:6 }}
{{ 3|add:"8" }}
{{ "String with spaces" | cut:" "}}
{{ "String with 'quotes'." |addslashes }}

ox{{ "" | pluralize:"en" }}
ox{{ " " | pluralize:"en" }}

tank{{ "" | pluralize }}
tank{{ " " | pluralize }}

cris{{ "" | pluralize:"is,es" }}
cris{{ " " | pluralize:"is,es" }}

ferr{{ "" | pluralize:"y,ies" }}
ferr{{ " " | pluralize:"y,ies" }}

{{ "joel is a slug" | title }}

{{ True  |yesno:"yeah,no,maybe" }}
{{ False |yesno:"yeah,no,maybe" }}
{{ None  |yesno:"yeah,no,maybe" }}
{{ None  |yesno:"yeah,no" }}

{{ "joel is a slug" | wordcount }}

{{ True  |default:"default" }}
{{ False |default:"default" }}
{{ None  |default:"default" }}

{{ True  |default_if_none:"default" }}
{{ False |default_if_none:"default" }}
{{ None  |default_if_none:"default" }}

{{123456789|filesizeformat}}

{% csrf_token %}
{{ 123456789|get_digit:"2" }}
{{ -123456789|get_digit:"2" }}
{{ "foobar"|get_digit:"2" }}

{{ a_few_lines|linebreaksbr }}
{{ more_lines|linebreaks }}
var s = "{{ a_bin_string|escapejs }}";

{{ "String & with & ampersands, but not &apos; or &#1234;" |fix_ampersands}}

{{ an_int_vector|slice:"0:9"}}
{{ an_int_vector|slice:":9"}}
{{ an_int_vector|slice:":"}}
{{ an_int_vector|slice:"0:"}}
{{ an_int_vector|slice:"2:6"}}
{{ an_int_vector|slice:"-6:-2"}}

{{ "Joel is a slug"|wordwrap:5 }}
{{ "<b>Joel</b> <button>is</button> a <span>slug</span>"|striptags}}
{{tag_list|join:", "}}
{{tag_list|safe|join:", "}}
{{tag_list|safeseq|join:", "}}

{{255|stringformat:'x'}}

{{a_few_lines|linenumbers}}
{{ "abc"|unordered_list}}
{{ places|unordered_list}}

{{ an_int_vector|make_list}}
{{ 12345|make_list}}

{% block a_block %}
This is a block
{% endblock a_block %}

{% include "samples/django/empty.tpl" %}

