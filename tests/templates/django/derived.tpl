{% extends "tests/templates/django/base.tpl" %}
Derived template
{% block content %}{{ block.super }} + {{ numbers }}{% endblock content %}
