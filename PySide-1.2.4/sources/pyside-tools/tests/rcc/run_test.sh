#!/bin/sh

# This is a nasty workaround of a CTest limitation
# of setting the environment variables for the test.

# $1: pyside-rcc
# $2: python test
# $3: qrc file

export PYTHONPATH=$PYTHONPATH:`pwd`
$1 -o `basename $3 .qrc`_rc.py $3
`pkg-config shiboken --variable=python_interpreter` $2

