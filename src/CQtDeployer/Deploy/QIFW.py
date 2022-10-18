import sys
import os
import subprocess
import shutil
from glob import glob

PLATFORM = sys.argv[1]
VERSION = sys.argv[2]
PATHQIF = sys.argv[3]

if len(PLATFORM) <= 0:
    PLATFORM = "linux"

if os.path.isdir(PATHQIF + "/QIF"):
    print("if target already executed")
    exit(0)

print("PLATFORM=" + PLATFORM)
print("VERSION=" + VERSION)

# Command Example : aqt tool linux tools_ifw 4.0 qt.tools.ifw.40
command = ["aqt",
           "install-tool",
           "--outputdir",
           PATHQIF,
           PLATFORM,
           "desktop",
           "tools_ifw",
           "qt.tools.ifw." + VERSION.replace('.', '')]

print("command=" + " ".join(command))

subprocess.call(command)

BASEPATHQIF = PATHQIF + "/Tools/QtInstallerFramework/" + VERSION[0:3]

for file in glob(BASEPATHQIF + "/bin/repogen*"):
    os.remove(file)
for file in glob(BASEPATHQIF + "/bin/archivegen*"):
    os.remove(file)
for file in glob(BASEPATHQIF + "/bin/devtool*"):
    os.remove(file)

os.rename(PATHQIF + "/Tools/QtInstallerFramework/" + VERSION[0:3] + "/bin/",
          PATHQIF + "/QIF")
shutil.rmtree(PATHQIF + "/Tools")
os.remove("aqtinstall.log")
