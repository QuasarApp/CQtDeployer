#!/bin/bash

BASE_DIR=$(dirname $0)

echo "clean build dir"
rm -rfd $BASE_DIR/build

echo "Start static build"
$BASE_DIR/staticBuild.sh

snapcraft clean cqtdeployer -s pull

snapcraft
