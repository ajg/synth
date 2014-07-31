#!/bin/bash -eux

python ./setup.py sdist upload

python ./setup.py bdist        upload
python ./setup.py bdist_egg    upload
python ./setup.py bdist_wheek  upload

python3 ./setup.py bdist        upload
python3 ./setup.py bdist_egg    upload
python3 ./setup.py bdist_wheek  upload
