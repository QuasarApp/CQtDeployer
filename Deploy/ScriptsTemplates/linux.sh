#!/bin/sh

# This is default bat run script of The CQtDeployer project.
# This file contains key word that will replaced after deploy project.
#
# ####################################################################
#
# CQT_LIB_PATH - are releative path to libraryes of a deployed distribution.
# CQT_QML_PATH - are releative path to qml libraryes of a deployed distribution.
# CQT_PLUGIN_PATH - are releative path to qt plugins of a deployed distribution.
# CQT_BIN_PATH - are releative path to targets of a deployed distribution.

# CQT_SYSTEM_LIB_PATH - are releative path to system libraryes of a deployed distribution.
# CQT_BASE_NAME - are base name of the executable that will be launched after run this script.
# CQT_CUSTOM_SCRIPT_BLOCK - This is code from the customScript option
# CQT_RUN_COMMAND - This is command for run application. Requred BASE_DIR variable.
#
# ####################################################################

BASE_DIR=$(dirname \"$(readlink -f \"$0\")\")
export LD_LIBRARY_PATH=\"$BASE_DIR\"CQT_LIB_PATH:\"$BASE_DIR\":$LD_LIBRARY_PATH
export QML_IMPORT_PATH=\"$BASE_DIR\"CQT_QML_PATH:$QML_IMPORT_PATH
export QML2_IMPORT_PATH=\"$BASE_DIR\"CQT_QML_PATH:$QML2_IMPORT_PATH
export QT_PLUGIN_PATH=\"$BASE_DIR\"CQT_PLUGIN_PATH:$QT_PLUGIN_PATH
export QTWEBENGINEPROCESS_PATH=\"$BASE_DIR\"CQT_BIN_PATH/QtWebEngineProcess
export QTDIR=\"$BASE_DIR\"
export CQT_PKG_ROOT=\"$BASE_DIR\"
export CQT_RUN_FILE=\"$BASE_DIR/CQT_BASE_NAME.sh\"

export QT_QPA_PLATFORM_PLUGIN_PATH=\"$BASE_DIR\"CQT_PLUGIN_PATH/platforms:$QT_QPA_PLATFORM_PLUGIN_PATH

CQT_CUSTOM_SCRIPT_BLOCK

CQT_RUN_COMMAND
