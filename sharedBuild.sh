#!/bin/bash

declare -a QTLIBS

BASE_DIR=$(dirname "$(readlink -f "$0")")
QTLIBS=( libQt5Sql.so libQt5Xml.so libQt5Core.so libQt5Test.so libQt5Network.so libQt5Concurrent.so)

RELEASE_DIR=$BASE_DIR/build/release

QMAKE=$1

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RELEASE_DIR

cd $BASE_DIR

git submodule update --init --recursive

make clean
find $BASE_DIR -type f -name 'Makefile' -exec rm {} \;
rm $BASE_DIR/QuasarAppLib/Makefile.QuasarApp

if [ -e "$QMAKE" ]

then
	echo "use qmake from params!"
	$QMAKE $BASE_DIR/CQtDeployer.pro
else
	echo "use qmake from build!"
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

	export PATH=$PATH:$BASE_DIR/sharedQt
	
	$BASE_DIR/sharedQt/bin/qmake $BASE_DIR/CQtDeployer.pro

fi

rm -rdf $BASE_DIR/build

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

mv $BASE_DIR/QuasarAppLib/build/release/* $RELEASE_DIR

strip $RELEASE_DIR/*
chmod +x $RELEASE_DIR/cqtdeployer

$RELEASE_DIR/cqtdeployer -runScript cqtdeployer.sh -bin $RELEASE_DIR/cqtdeployer -qmake $BASE_DIR/sharedQt/bin/qmake


if [ -e "$QMAKE" ]
then
    echo ""
	echo "deploy done (shared mode with custom qmake)"
else
	cd $RELEASE_DIR
	tar -czvf $RELEASE_DIR/cqtdeployer.tar.gz ./*
	cd $BASE_DIR

	rm $RELEASE_DIR/lib -rdf $RELEASE_DIR/*.so* $RELEASE_DIR/*.sh*
	echo ""
	echo "deploy done (shared mode with own qmake)"
fi


