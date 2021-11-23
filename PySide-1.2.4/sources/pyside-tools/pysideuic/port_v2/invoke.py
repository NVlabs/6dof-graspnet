# This file is part of the PySide project.
#
# Copyright (C) 2009-2011 Nokia Corporation and/or its subsidiary(-ies).
# Copyright (C) 2010 Riverbank Computing Limited.
# Copyright (C) 2009 Torsten Marek
#
# Contact: PySide team <pyside@openbossa.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301 USA

from pysideuic.exceptions import NoSuchWidgetError


def invoke(driver):
    """ Invoke the given command line driver.  Return the exit status to be
    passed back to the parent process.
    """

    exit_status = 1

    try:
        exit_status = driver.invoke()

    except IOError, e:
        driver.on_IOError(e)

    except SyntaxError, e:
        driver.on_SyntaxError(e)

    except NoSuchWidgetError, e:
        driver.on_NoSuchWidgetError(e)

    except Exception, e:
        driver.on_Exception(e)

    return exit_status
