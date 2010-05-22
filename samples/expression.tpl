{{some_strings[2]}}, {{some_strings[1]}}
{{3.3}}
{{3.30}}

{{34.23234|floatformat }} 	== 34.2
{{34.00000|floatformat }} 	== 34
{{34.26000|floatformat }} 	== 34.3

{{34.23234|floatformat:3 }} == 34.232
{{34.00000|floatformat:3 }} == 34.000
{{34.26000|floatformat:3 }} == 34.260

{{34.23234|floatformat:"-3" }}  == 34.232
{{34.00000|floatformat:"-3" }} 	== 34
{{34.26000|floatformat:"-3" }} 	== 34.260


{% for k, v in a_string_array %}
    {% cycle k v as ttt %} hello. {{ttt}}
{% endfor %}
hi.

{{"Joel is a slug"|truncatewords:2}}

{{"This is some text containing a http://www.url.com sir and also another.url.com."|urlize}}
{{"This is some text containing a http://www.url.com sir and also another.url.com."|urlizetrunc:15}}

{{"<b>Joel</b> <button>is</button> a <span>slug</span>"|removetags:"b span"|safe }}
{{"<b>Begin</b> <foo /> <foo/> </foo> <foo> <span attr='value'>End</span>"|removetags:"b span foo"|safe }}
?
