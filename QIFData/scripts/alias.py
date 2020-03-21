import os
import shutil
import subprocess

dir_path = os.path.dirname(os.path.realpath(__file__))
dest = dir_path + "/../packages/cqtdeployer.1_4/data/1.4/"
if not os.path.isdir(dest):
    os.makedirs(dest)


shutil.copyfile(dir_path + "/alias/cqt.sh", dest + "/cqt.sh")
subprocess.run(["chmod", "777", dest + "/cqt.sh"])
