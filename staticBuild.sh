#!/bin/bash

declare -a QTLIBS

BASE_DIR=$(dirname $0)
QTLIBS=( libQt5Sql.a libQt5Xml.a libQt5Core.a libQt5Test.a libqtpcre2.a libQt5Network.a libQt5Bootstrap.a libQt5Concurrent.a)  

cd qtBase

for var in "${QTLIBS[@]}"
do
	if [ -e "$BASE_DIR/lib/$var" ]
	then
	    echo "$var - ok"
	else
	    echo "$var - not exits!. rebuild qt ..."

	    git clean -xfd
	    
	    ./configure -confirm-license -release -optimize-size -static -no-opengl -no-openssl -opensource -nomake tests -nomake examples -no-gui -no-widgets -no-dbus -no-accessibility
	    
	    make -j$(nproc)
	    break
	fi
done
	
cd ..
export PATH=$PATH:/qtBase

./qtBase/bin/qmake CQtDeployer.pro

make -j$(nproc)

strip build/*
