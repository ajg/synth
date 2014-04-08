##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

import synth
from difflib import unified_diff

print('Loaded synth; version: ' + synth.version())

class User:
    def __init__(self):
        self.is_staff = False
        self.is_authenticated = True

directory = 'tests/templates/django/'

context = {
    'request': {
        'user': User(),
    },
    'STATIC_URL': '/static/',
    'title': 'Default Title',
    'messages': ["Foo", "Bar", "Qux"],
}
source = open(directory + 'layout.html').read().encode('utf-8')

template = synth.Template(source, 'django', True, 'INVALID_VALUE', {}, False, [directory])
print('Parsing succeeded!')

string = template.render_to_string(context)
print('Rendering succeeded!')

golden = """<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en" dir="ltr">
  <head>
      <title>Default Title</title>
      <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
      <!-- -->
      <!-- -->

      <!-- -->
      <!-- -->
      <link rel="shortcut icon" href="/static/css/images/favicon.ico" type="image/x-icon" />

      <!-- -->
      <script type="text/javascript">
        var _gaq = _gaq || [];
        _gaq.push(['_setAccount', '']);
        _gaq.push(['_trackPageview']);

        (function() {
          var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
          ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
          var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
        })();
      </script>
      <!-- -->
  </head>
  <body class="authenticated ">
    <div id="distance"></div>
    <div id="wrapper">
      <div class="container">
        <div id="header">
          <a href="/" title="Home"><img id="logo" src="/static/css/images/logo.png" alt="" /></a>
          <!-- -->
          <div id="navigation">
            <!-- -->
          </div>
          <!-- -->
        </div>
        <div id="columns">
          <div id="center">
            <div id="content" class="content">
              <div id="messages">


<ul class="F messages">

  <li>Foo</li>

</ul>


<ul class="B messages">

  <li>Bar</li>

</ul>


<ul class="Q messages">

  <li>Qux</li>

</ul>




<ul class="messages">
  <li>Foo</li>
</ul>

<ul class="messages">
  <li>Bar</li>
</ul>

<ul class="messages">
  <li>Qux</li>
</ul>

</div>
              <!-- -->
              <!-- -->
              <!-- -->
            </div>
          </div>
          <div id="left">
            <!-- -->
          </div>
          <div id="right">
            <!-- -->
          </div>
          <div class="clear"></div>
        </div>
      </div>
      <div id="push"></div>
    </div>
    <div id="footer">
      <div class="container">
        <!-- -->
      </div>
    </div>
  </body>
</html>
"""

if string != golden:
    diff = ''.join(unified_diff(golden.splitlines(True), string.splitlines(True)))
    raise Exception("MISMATCH:\n" + diff)

print('Matching succeeded!')
