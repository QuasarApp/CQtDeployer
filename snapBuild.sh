#!/bin/bash

BASE_DIR=$(dirname "$(readlink -f "$0")")

echo "clean build dir"
rm -rfd $BASE_DIR/distro

chmod a+rx $BASE_DIR/wrapper/bin/desktop-launch

echo "Start static build"
$BASE_DIR/staticBuild.sh Q_OS_LINUX_SNAP

if [ $? -eq 0 ]
then
    echo ""
    echo "staticBuild is successfully completed!"
else
    echo ""
    echo "staticBuild is failed!" >&2
    exit 1
fi

snapcraft clean cqtdeployer -s pull

snapcraft
