:: This is default bat run script of The CQtDeployer project.
:: This file contains key word that will replaced after deploy project.

:: ####################################################################
:: All variables has the CQT_ prefix
:: BIN_PATH - are releative path to executable files of a deployed distribution.
:: LIB_PATH - are releative path to libraryes of a deployed distribution.
:: SYSTEM_LIB_PATH - are releative path to system libraryes of a deployed distribution.
:: BASE_NAME - are base name of the executable that will be launched after run this script.
:: CUSTOM_SCRIPT_BLOCK - This is code from the customScript option
:: RUN_COMMAND - This is command for run application. Requred BASE_DIR variable.

:: ####################################################################

@echo off
SET BASE_DIR=%~dp0
SET PATH=%BASE_DIR%CQT_BIN_PATH;%BASE_DIR%CQT_LIB_PATH;%PATH%;%BASE_DIR%CQT_SYSTEM_LIB_PATH
SET CQT_PKG_ROOT=%BASE_DIR%
SET CQT_RUN_FILE=%BASE_DIR%CQT_BASE_NAME.bat

CQT_CUSTOM_SCRIPT_BLOCK

CQT_RUN_COMMAND
