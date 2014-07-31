REM ================== Starting
:: Pre-requirements:
:: cinst python
:: cinst python2
:: cinst python-x86_32
:: cinst python2-x86_32
::
:: c:\tools\python2\python.exe        get-pip.py
:: c:\tools\python2-x86_32\python.exe get-pip.py
::
:: c:\tools\python\Scripts\pip.exe         install setuptools wheel --upgrade
:: c:\tools\python2\Scripts\pip.exe        install setuptools wheel --upgrade
:: c:\tools\python-x86_32\Scripts\pip.exe  install setuptools wheel --upgrade
:: c:\tools\python2-x86_32\Scripts\pip.exe install setuptools wheel --upgrade

REM ================== Building and Uploading Synth for Python 3 (x64)
c:\tools\python\python.exe .\setup.py clean --all
c:\tools\python\python.exe .\setup.py bdist         upload
c:\tools\python\python.exe .\setup.py bdist_egg     upload
c:\tools\python\python.exe .\setup.py bdist_wininst upload
c:\tools\python\python.exe .\setup.py bdist_wheel   upload
REM ================== Done

REM ================== Building and Uploading Synth for Python 2 (x64)
c:\tools\python2\python.exe .\setup.py clean --all
c:\tools\python2\python.exe .\setup.py bdist         upload
c:\tools\python2\python.exe .\setup.py bdist_egg     upload
c:\tools\python2\python.exe .\setup.py bdist_wininst upload
c:\tools\python2\python.exe .\setup.py bdist_wheel   upload
REM ================== Done

REM ================== Building and Uploading Synth for Python 3 (x86)
c:\tools\python-x86_32\python.exe .\setup.py clean --all
c:\tools\python-x86_32\python.exe .\setup.py bdist         upload
c:\tools\python-x86_32\python.exe .\setup.py bdist_egg     upload
c:\tools\python-x86_32\python.exe .\setup.py bdist_wininst upload
c:\tools\python-x86_32\python.exe .\setup.py bdist_wheel   upload
REM ================== Done

REM ================== Building and Uploading Synth for Python 2 (x86)
c:\tools\python2-x86_32\python.exe .\setup.py clean --all
c:\tools\python2-x86_32\python.exe .\setup.py bdist         upload
c:\tools\python2-x86_32\python.exe .\setup.py bdist_egg     upload
c:\tools\python2-x86_32\python.exe .\setup.py bdist_wininst upload
c:\tools\python2-x86_32\python.exe .\setup.py bdist_wheel   upload
REM ================== Done

REM ================== Finishing
