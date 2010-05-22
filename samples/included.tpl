{# Bad Bad Bad #}
This is the included file {{X}}, {{Y}} : {{a_char}}, {{an_int}}, {{a_pair}}, {{a_boost_array}}, {{an_optional_float}}, {{a_pointer}}.
{% if not 0 %} IF Good {% else %} IF Bad {% endif %}

{% with "this_is_a_long_string" as ls %} {{ls}} {% endwith %}

{% templatetag openbrace %}
{% templatetag closevariable %}

{% spaceless %}
     <p>
        <a href="foo/">Foo</a>
    </p>
{% endspaceless %}

{% spaceless %}
    <strong>
        Hello
    </strong>
{% endspaceless %}

{% filter upper %}
{% debug %}
{% endfilter %}

<img src="bar.gif" height="10" width="{% widthratio this_value max_value 100 %}" />

{#% ssi /etc/adjtime parsed % -- unavailable on Windows #}
{#% ssi /etc/adjtime % -- unavailable on Windows #}
