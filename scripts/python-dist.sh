#!/bin/bash -eux

python2 ./setup.py clean --all
python2 ./setup.py bdist        upload
python2 ./setup.py bdist_egg    upload
python2 ./setup.py bdist_wheel  upload

python3 ./setup.py clean --all
python3 ./setup.py bdist        upload
python3 ./setup.py bdist_egg    upload
python3 ./setup.py bdist_wheel  upload

python2 ./setup.py clean --all
python2 ./setup.py sdist upload
