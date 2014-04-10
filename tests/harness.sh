#!/bin/bash -eu
##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

function main {
    expect=$(echo -e -n 'foo: 1\nbar: 2\nqux: 3\n')
    actual=$(cat tests/templates/django/variables.tpl | ./synth -e django -c tests/data/variables.json)

    if [[ expect == actual ]]
    then echo -e 'Success';
    else echo -e "Failure\nExpect: `$expect`\nActual: `$actual`";
    fi
}

main
