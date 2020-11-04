import sys
import os
import subprocess
import shutil

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

command = ["aqt",
            "tool",
            "--outputdir",
            PATHQIF,
            PLATFORM,
            "tools_ifw",
            VERSION,
            " "]

print("command=" + "".join(command))

subprocess.call(command)

os.rename(PATHQIF + "/Tools/QtInstallerFramework/" + VERSION[0:3] + "/bin/",
          PATHQIF + "/QIF")
shutil.rmtree(PATHQIF + "/Tools")
os.remove("aqtinstall.log")
