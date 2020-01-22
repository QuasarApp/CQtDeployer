#!/bin/bash
BASE_DIR=$(dirname "$(readlink -f "$0")")

PLATFORM=$1
VERSION=$2
PATHQIF=$3

if [ -z "$PLATFORM" ]; then
    PLATFORM=linux
fi

if [ -d "$PATHQIF/QIF" ]; then
    echo if target already executed
    exit 0
fi

echo PLATFORM=$PLATFORM
echo VERSION=$VERSION

cd $PATHQIF
aqt tool --outputdir $PATHQIF linux tools_ifw $VERSION " "
mkdir -p $PATHQIF/QIF
mv $PATHQIF/Tools/QtInstallerFramework/${VERSION:0:3}/bin/* $PATHQIF/QIF
rm -rdf $PATHQIF/Tools aqtinstall.log
