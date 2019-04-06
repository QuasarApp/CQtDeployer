#!/bin/bash

#
# Copyright (C) 2018-2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

declare -a QTLIBS

BASE_DIR=$(dirname "$(readlink -f "$0")")
QTLIBS=( libQt5Sql.so libQt5Xml.so libQt5Core.so libQt5Test.so libQt5Network.so libQt5Concurrent.so)
QT_DIR=sharedQt

RELEASE_DIR=$BASE_DIR/distro
CQDEPLOYER_DIR=$BASE_DIR/CQtDeployer/build/release
DEPLOYER=$BASE_DIR/CQtDeployerBinaries/Linux/cqtdeployer.sh
cd $BASE_DIR

make clean
find $BASE_DIR -type f -name 'Makefile' -exec rm {} \;
find $BASE_DIR/QuasarAppLib -type f -name '*.so*' -exec rm {} \;
find $BASE_DIR/Deploy -type f -name '*.so*' -exec rm {} \;

rm $BASE_DIR/QuasarAppLib/Makefile.QuasarApp
rm -rdf $RELEASE_DIR

if [ -e "$1" ]

then
	echo "use qmake from params!"
	QMAKE=$1

else
	echo "use qmake from build!"
        QMAKE=$BASE_DIR/$QT_DIR/bin/qmake

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
                git checkout v5.11.3
                ./configure -confirm-license -prefix $BASE_DIR/$QT_DIR -release -shared -no-opengl -no-openssl -opensource -nomake tests -nomake examples -qt-pcre -no-gui -no-widgets -no-dbus -no-accessibility
                make install -j$(nproc)
                break
            fi
	done
	
	cd ..

        export PATH=$PATH:$BASE_DIR/$QT_DIR

fi
$QMAKE $BASE_DIR/CQtDeployer.pro -r

make -j$(nproc)

if [ $? -eq 0 ]
then    
    echo ""
    echo "Build is successfully completed!"
else
    echo ""
    echo "Build is failed!" >&2
    exit 1;
fi

$DEPLOYER -targetDir $RELEASE_DIR -bin $CQDEPLOYER_DIR -libDir $BASE_DIR -recursiveDepth 5 -qmake $QMAKE
strip $RELEASE_DIR/*


if [ -e "$1" ]
then
    echo ""
	echo "deploy done (shared mode with custom qmake)"
else
        cd $RELEASE_DIR
        tar -czvf $RELEASE_DIR/cqtdeployer.tar.gz ./*
	cd $BASE_DIR

        rm $TEMP_INSTALL_DIR -rdf
        echo ""
	echo "deploy done (shared mode with own qmake)"
	exit 0
fi


