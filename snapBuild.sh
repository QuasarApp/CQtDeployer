#!/bin/bash

#
# Copyright (C) 2018-2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

BASE_DIR=$(dirname "$(readlink -f "$0")")

echo "clean build dir"
rm -rfd $BASE_DIR/distro

chmod a+rx $BASE_DIR/wrapper/bin/desktop-launch

echo "Start static build"
$BASE_DIR/staticBuild.sh Q_OS_LINUX_SNAP

chmod a+rx $BASE_DIR/distro/cqtdeployer

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
