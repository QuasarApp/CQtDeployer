#!/bin/bash

#
# Copyright (C) 2018-2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

IS_SNAP="$1"
QT_DIR=staticQt

if [ -e "$IS_SNAP"]
then
    echo "build for snap"
    SNAP_DEFINES="$IS_SNAP"
fi

declare -a QTLIBS

BASE_DIR=$(dirname "$(readlink -f "$0")")
QTLIBS=( libQt5Sql.a libQt5Xml.a libQt5Core.a libQt5Test.a libQt5Network.a libQt5Bootstrap.a libQt5Concurrent.a)

RELEASE_DIR=$BASE_DIR/distro

if [ -e "$PREFIX"]
then
    echo "PREFIX is empty, use default install path $RELEASE_DIR"
else
    echo "use PREFIX path!"
    RELEASE_DIR=$PREFIX
fi

cd $BASE_DIR

git submodule update --init --recursive

make clean
find $BASE_DIR -type f -name 'Makefile' -exec rm {} \;
find $BASE_DIR/QuasarAppLib -type f -name '*.a*' -exec rm {} \;
find $BASE_DIR/Deploy -type f -name '*.a*' -exec rm {} \;

rm $BASE_DIR/QuasarAppLib/Makefile.QuasarApp
rm -rdf $RELEASE_DIR

cd $BASE_DIR/qtBase

for var in "${QTLIBS[@]}"
do
        if [ -e "$BASE_DIR/$QT_DIR/lib/$var" ]
	then
	    echo "$var - ok"
	else
	    echo "$var - not exits!. rebuild qt ..."
            rm -rdf $BASE_DIR/$QT_DIR
            git clean -xdf
            git checkout .
            ./configure -confirm-license -prefix $BASE_DIR/$QT_DIR -release -static -optimize-size -qt-pcre -qt-zlib -no-opengl -no-openssl -opensource -nomake tests -nomake examples -no-gui -no-widgets -no-dbus -no-accessibility
            make install -j$(nproc)
	    break
	fi
done
	
cd ..
rm -rdf $BASE_DIR/build

export PATH=$PATH:$BASE_DIR/$QT_DIR
$BASE_DIR/$QT_DIR/bin/qmake -r QMAKE_LFLAGS+="-static -static-libgcc -static-libstdc++" $BASE_DIR/CQtDeployer.pro DEFINES+="$SNAP_DEFINES" DEFINES+=WITHOUT_BASE_TESTS

make -j$(nproc)

if [ $? -eq 0 ]
then    
    echo ""
    echo "Build is successfully completed!"
else
    echo ""
    echo "Build is failed!" >&2
    exit 1
fi

mkdir -p $RELEASE_DIR

cp $BASE_DIR/CQtDeployer/build/release/cqtdeployer $RELEASE_DIR/

strip $RELEASE_DIR/*


