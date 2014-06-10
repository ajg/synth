{% extends "tests/templates/django/C.tpl" %}
{% block x %}{{ block.super }}D{% endblock x %}
