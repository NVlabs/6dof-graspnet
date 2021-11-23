======
PySide
======

.. image:: https://img.shields.io/pypi/wheel/pyside.svg
   :target: https://pypi.python.org/pypi/PySide/
   :alt: Wheel Status

.. image:: https://img.shields.io/pypi/dm/pyside.svg
   :target: https://pypi.python.org/pypi/PySide/
   :alt: Downloads

.. image:: https://img.shields.io/pypi/v/pyside.svg
   :target: https://pypi.python.org/pypi/PySide/
   :alt: Latest Version

.. image:: https://binstar.org/asmeurer/pyside/badges/license.svg
   :target: https://pypi.python.org/pypi/PySide/
   :alt: License

.. image:: https://readthedocs.org/projects/pip/badge/
    :target: https://pyside.readthedocs.org

.. contents:: **Table of Contents** 
   :depth: 2

Introduction
============

PySide is the Python Qt bindings project, providing access the complete Qt 4.8 framework
as well as to generator tools for rapidly generating bindings for any C++ libraries.

The PySide project is developed in the open, with all facilities you'd expect
from any modern OSS project such as all code in a git repository, an open
Bugzilla for reporting bugs, and an open design process. We welcome
any contribution without requiring a transfer of copyright.

The PySide documentation is hosted at `http://pyside.github.io/docs/pyside/
<http://pyside.github.io/docs/pyside/>`_.

Compatibility
=============

PySide requires Python 2.6 or later and Qt 4.6 or better.

.. note::

   Qt 5.x is currently not supported.

Installation
============

Installing prerequisites
------------------------

Install latest ``pip`` distribution: download `get-pip.py
<https://bootstrap.pypa.io/get-pip.py>`_ and run it using
the ``python`` interpreter.

Installing PySide on a Windows System
-------------------------------------

To install PySide on Windows you can choose from the following options:

#. Use pip to install the ``wheel`` binary packages:

   ::

      pip install -U PySide

#. Use setuptools to install the ``egg`` binary packages (deprecated):

   ::

      easy_install -U PySide

.. note::

   Provided binaries are without any other external dependencies.
   All required Qt libraries, development tools and examples are included.


Installing PySide on a Mac OS X System
--------------------------------------

You need to install or build Qt 4.8 first, see the `Qt Project Documentation
<http://qt-project.org/doc/qt-4.8/install-mac.html>`_.

Alternatively you can use `Homebrew <http://brew.sh/>`_ and install Qt with

::

   $ brew install qt

To install PySide on Mac OS X you can choose from the following options:

#. Use pip to install the ``wheel`` binary packages:

   ::

      $ pip install -U PySide


Installing PySide on a Linux System
-----------------------------------

We do not provide binaries for Linux. Please read the build instructions in section
`Building PySide on a Linux System
<http://pyside.readthedocs.org/en/latest/building/linux.html>`_.


Building PySide
===============

- `Building PySide on a Windows System <http://pyside.readthedocs.org/en/latest/building/windows.html>`_.

- `Building PySide on a Mac OS X System <http://pyside.readthedocs.org/en/latest/building/macosx.html>`_.

- `Building PySide on a Linux System <http://pyside.readthedocs.org/en/latest/building/linux.html>`_.


Feedback and getting involved
=============================

- Mailing list: http://lists.qt-project.org/mailman/listinfo/pyside
- Issue tracker: https://bugreports.qt-project.org/browse/PYSIDE
- Code Repository: http://qt.gitorious.org/pyside
