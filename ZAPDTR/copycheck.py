#!/usr/bin/env python3

import os
from shutil import copyfile

if (os.environ.get('ZAPD_COPYDIR') != None):
    print("Copying ZAPD.out to repo...")
    #print(os.environ.get('ZAPD_COPYDIR'))
    copyfile("ZAPD.out", os.environ.get('ZAPD_COPYDIR') + "/ZAPD.out")
