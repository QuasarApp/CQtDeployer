#
# Copyright (C) 2018 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

!isEmpty(WINDEPENDENCIESSCANNER_LIB):error("WinDependenciesScanner.pri already included")
WINDEPENDENCIESSCANNER_LIB = 1

#DEPENDS
CONFIG(release, debug|release): {
    WINDEPENDENCIESSCANNER_LIB_OUTPUT_DIR="$$PWD/build/release"
} else {
    WINDEPENDENCIESSCANNER_LIB_OUTPUT_DIR="$$PWD/build/debug"
}

LIBS += -L$$WINDEPENDENCIESSCANNER_LIB_OUTPUT_DIR -lWinDependenciesScanner

INCLUDEPATH += "$$PWD/"
#INCLUDEPATH += "$$PWD/../qtTools/src/windeployqt"



