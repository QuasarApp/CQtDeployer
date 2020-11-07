import os
import sys
import shutil   

location = sys.argv[1]

PWD = os.path.dirname(os.path.realpath(__file__))

shutil.copytree(PWD + "/packages", location)
