#!/usr/bin/env python

#
# =========================================================================
# This file is part of six.sicd-python
# =========================================================================
#
# (C) Copyright 2004 - 2016, MDA Information Systems LLC
#
# six.sicd-python is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; If not,
# see <http://www.gnu.org/licenses/>.
#

import os
import subprocess
import sys

import runPythonScripts
import checkNITFs
import utils

utils.setPaths()

if runPythonScripts.run() == False:
    print "Error running a python script"
    sys.exit(1)

if checkNITFs.run() == False:
    print "test in checkNITFS.py failed"
    sys.exit(1)


if subprocess.call([utils.executableName(os.path.join(
        utils.installPath(), 'tests', 'six.sidd', 'test_byte_swap'))]) != 0:
    print "Failed ByteSwap test in six.sidd/tests/test_byte_swap"
    sys.exit(1)

print "All passed"
sys.exit(0)
