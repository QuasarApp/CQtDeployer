#!/bin/bash

declare -a QTLIBS

BASE_DIR=$(dirname "$(readlink -f "$0")")
QTLIBS=( libQt5Sql.so libQt5Xml.so libQt5Core.so libQt5Test.so libQt5Network.so libQt5Concurrent.so)

git submodule update --init --recursive

make clean
find $BASE_DIR -type f -name 'Makefile' -exec rm {} \;

cd $BASE_DIR/qtBase

for var in "${QTLIBS[@]}"
do
        if [ -e "$BASE_DIR/sharedQt/lib/$var" ]
	then
	    echo "$var - ok"
	else
	    echo "$var - not exits!. rebuild qt ..."

            rm -rdf $BASE_DIR/sharedQt
            git clean -xdf
            ./configure -confirm-license -prefix $BASE_DIR/sharedQt -release -shared -no-opengl -no-openssl -opensource -nomake tests -nomake examples -no-gui -no-widgets -no-dbus -no-accessibility
            make install -j$(nproc)
	    break
	fi
done
	
cd ..
rm -rdf $BASE_DIR/build
export PATH=$PATH:$BASE_DIR/sharedQt
$BASE_DIR/sharedQt/bin/qmake CQtDeployer.pro

make -j$(nproc)

mv $BASE_DIR/QuasarAppLib/build/* $BASE_DIR/build

strip build/*
chmod +x $BASE_DIR/build/cqtdeployer

$BASE_DIR/build/cqtdeployer -runScript cqtdeployer.sh -bin $BASE_DIR/build/cqtdeployer -qmake $BASE_DIR/sharedQt/bin/qmake

cd $BASE_DIR/build

tar -czvf $BASE_DIR/build/cqtdeployer.tar.gz ./*
cd $BASE_DIR

rm $BASE_DIR/build/lib -rdf $BASE_DIR/build/*.so* $BASE_DIR/build/*.sh*
