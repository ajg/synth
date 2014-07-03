##  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
##  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
##  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

import synth
import sys
import tempfile
from difflib import unified_diff

print('Loaded synth; version: ' + synth.version())

# TODO: Golden-file tests like these are brittle and ugly;
#       refactor and figure out how to reuse the native unit tests.

def main():
    import python.binding_tests   as binding_tests
    import python.directory_tests as directory_tests
    import python.library_tests   as library_tests

    print 'Running test harness...'
    run_test('binding_tests',   *binding_tests.get())
    run_test('directory_tests', *directory_tests.get())
    run_test('library_tests',   *library_tests.get())

    if failures != 0:
        raise Exception('One or more tests failed')

    print '...done.'

total, failures = 0, 0

def run_test(name, data, golden, source, engine, options):
    run_test_as('default', name, data, golden, source, engine, options)
    run_test_as('utf-8',   name, data, golden, source.encode('utf-8'), engine, options)
  # TODO: run_test_as('utf-16',  name, data, golden, source.encode('utf-16'), engine, options)
  # TODO: run_test_as('utf-32',  name, data, golden, source.encode('utf-32'), engine, options)
    run_test_as('unicode', name, data, golden, unicode(source), engine, options)
    run_test_as('str',     name, data, golden, str(source), engine, options)

def run_test_as(type, name, data, golden, source, engine, options):
    global total, failures
    total += + 1
    print('  Test #%d [%s] [%s]' % (total, name, type))

    try:
        template = synth.Template(source, engine, options)
        print('    - Parsing succeeded')
    except Exception as e:
        failures += 1
        print('    x Parsing failed:\n' + str(e))
        return

    try:
        string = template.render_to_string(data)
        print('    - Rendering to string succeeded')
    except Exception as e:
        failures += 1
        print('    x Rendering to string failed:\n' + str(e))
        return

    with tempfile.TemporaryFile() as file:
        try:
            template.render_to_file(file, data)
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

    # Note: Windows doesn't support reading from an already open
    #       temporary file and has stupid newline conventions.
    if sys.platform == 'win32':
        string = string.replace('\r\n', '\n')
        golden = golden.replace('\r\n', '\n')
        print('    # Rendering to path excluded on this platform')
    else:
        with tempfile.NamedTemporaryFile() as file:
            try:
                template.render_to_path(file.name, data)
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
