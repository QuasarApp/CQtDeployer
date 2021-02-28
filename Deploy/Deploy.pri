#
# Copyright (C) 2018-2021 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

!isEmpty(DEPLOY_LIB):error("Deploy.pri already included")
DEPLOY_LIB = 1

#DEPENDS
CONFIG(release, debug|release): {
    DEPLOY_LIB_OUTPUT_DIR="$$PWD/build/release"
} else {
    DEPLOY_LIB_OUTPUT_DIR="$$PWD/build/debug"
}

unix:LIBS += -L$$DEPLOY_LIB_OUTPUT_DIR -lDeploy
win32:LIBS += -L$$DEPLOY_LIB_OUTPUT_DIR -lDeploy1

include('$$PWD/../LIEF/includeLIEF.pri')

INCLUDEPATH += "$$PWD/"



