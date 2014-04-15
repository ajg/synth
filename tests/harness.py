##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

import synth
import tempfile
from difflib import unified_diff

print('Loaded synth; version: ' + synth.version())

# TODO: Golden-file tests like these are brittle and ugly;
#       refactor and figure out how to reuse the native unit tests.

def main():
    import python.binding_test   as binding_test
    import python.directory_test as directory_test
    import python.loader_test    as loader_test

    print 'Running test harness...'
    run_test('binding_test',   *binding_test.get())
    run_test('directory_test', *directory_test.get())
    run_test('loader_test',    *loader_test.get())

    if failures != 0:
        raise Exception('One or more tests failed')

    print '...done.'

total, failures = 0, 0

def run_test(name, context, golden, source, engine, args=()):
    global total, failures
    total += + 1
    print('  Test #%d [%s]' % (total, name))

    try:
        template = synth.Template(source.encode('utf-8'), engine, *args)
        print('    - Parsing succeeded')
    except Exception as e:
        failures += 1
        print('    x Parsing failed:\n' + str(e))
        return

    try:
        string = template.render_to_string(context)
        print('    - Rendering to string succeeded')
    except Exception as e:
        failures += 1
        print('    x Rendering to string failed:\n' + str(e))
        return

    with tempfile.TemporaryFile() as file:
        try:
            template.render_to_file(file, context)
            print('    - Rendering to file succeeded')
        except Exception as e:
            failures += 1
            print('    x Rendering to file failed:\n' + str(e))
            return
        else:
            file.flush()
            file.seek(0)
            if string != file.read():
                print('    x Rendering to file failed: mismatch')

    with tempfile.NamedTemporaryFile() as file:
        try:
            template.render_to_path(file.name, context)
            print('    - Rendering to path succeeded')
        except Exception as e:
            failures += 1
            print('    x Rendering to path failed:\n' + str(e))
            return
        else:
            file.flush()
            file.seek(0)
            if string != file.read():
                print('    x Rendering to path failed: mismatch')

    if string == golden:
        print('    - Matching succeeded')
    else:
        failures += 1
        diff = ''.join(unified_diff(golden.splitlines(True), string.splitlines(True)))
        print('    x Matching failed:\n' + diff)
        return

if __name__ == "__main__":
    main()
