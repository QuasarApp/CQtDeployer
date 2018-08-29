#!/bin/bash

cd qtBase
git clean -xfd

./configure -confirm-license -release -optimize-size -static -no-opengl -no-openssl -opensource -nomake tests -nomake examples -no-gui -no-widgets -no-dbus -no-accessibility

make -j$(nproc)
cd ..
export PATH=$PATH:/qtBase

./qtBase/bin/qmake CQTDeployer.pro

make -j$(nproc)

strip build/*
