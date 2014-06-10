{% extends "tests/templates/django/B.tpl" %}
{% block x %}{{ block.super }}C{% endblock x %}
