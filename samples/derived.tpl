{% extends "samples/base.tpl" %}

{% block content %}
Derived content: {{ a_string_array }}.
{{ content_super }}
{% endblock content %}

This is the derived template.
{{a_string_array[2]}}
