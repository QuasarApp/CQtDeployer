#!/bin/bash
BASE_DIR=$(dirname "$(readlink -f "$0")")

PLATFORM= $1
VERSION= $2
PATHQIF= $3

if [-n "$PLATFORM" ]; then
    PLATFORM= linux
fi

echo PLATFORM=$PLATFORM
echo VERSION=$VERSION

cd $PATHQIF
aqt tool linux tools_ifw $VERSION qt.tools.ifw.${${VERSION//.}:0:2}
