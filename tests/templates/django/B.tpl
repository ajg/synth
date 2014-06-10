{% extends "tests/templates/django/A.tpl" %}
{% block x %}{{ block.super }}B{% endblock x %}
