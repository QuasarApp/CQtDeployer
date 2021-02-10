#!/bin/bash
BASE_DIR=$(dirname "$(readlink -f "$0")")
export LD_LIBRARY_PATH="$BASE_DIR"/lib/:"$BASE_DIR":$LD_LIBRARY_PATH:"$BASE_DIR"/lib/systemLibs
export QML_IMPORT_PATH="$BASE_DIR"/q/and/q/:$QML_IMPORT_PATH
export QML2_IMPORT_PATH="$BASE_DIR"/q/and/q/:$QML2_IMPORT_PATH
export QT_PLUGIN_PATH="$BASE_DIR"/plugins/:$QT_PLUGIN_PATH
export QTWEBENGINEPROCESS_PATH="$BASE_DIR"/bin/QtWebEngineProcess
export QTDIR="$BASE_DIR"
export CQT_PKG_ROOT="$BASE_DIR"
export CQT_RUN_FILE="$BASE_DIR/TestQMLWidgets.sh"
export QT_QPA_PLATFORM_PLUGIN_PATH="$BASE_DIR"/plugins/platforms:$QT_QPA_PLATFORM_PLUGIN_PATH

"$BASE_DIR/bin/TestQMLWidgets" "$@"
