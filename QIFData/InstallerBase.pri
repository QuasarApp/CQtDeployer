
QT_DIR= $$[QT_HOST_BINS]

win32:QMAKE_BIN= $$QT_DIR/qmake.exe
win32:LUPDATE = $$QT_DIR/lupdate.exe
win32:LRELEASE = $$QT_DIR/lrelease.exe
win32:DEPLOYER=cqtdeployer


contains(QMAKE_HOST.os, Linux):{
    QMAKE_BIN= $$QT_DIR/qmake
    LUPDATE = $$QT_DIR/lupdate
    LRELEASE = $$QT_DIR/lrelease
    DEPLOYER = cqtdeployer
}

android {
    DEPLOYER = $$QT_DIR/androiddeployqt
}

message( PWD :$$PWD)

message( Configuration variables :)
message(QT_DIR = $$QT_DIR)
message(QMAKE_BIN = $$QMAKE_BIN)
message(LUPDATE = $$LUPDATE)
message(LRELEASE = $$LRELEASE)
message(DEPLOYER = $$DEPLOYER)

BINARY_LIST
REPO_LIST

sopprted_versions = 4.5 4.4 4.3 4.2 4.1 4.0 3.2 3.1 3.0
for(val, sopprted_versions) {

    exists( $$QT_DIR/../../../Tools/QtInstallerFramework/$$val/bin/ ) {
          message( "QtInstallerFramework v$$val: yes" )
          BINARY_LIST += $$QT_DIR/../../../Tools/QtInstallerFramework/$$val/bin/binarycreator
          REPO_LIST += $$QT_DIR/../../../Tools/QtInstallerFramework/$$val/bin/repogen
    }
}

isEmpty (BINARY_LIST) {
      warning( "QtInstallerFramework not found! use binaries from PATH." )
      EXEC=binarycreator
      REPOGEN=repogen

} else: {
    win32:EXEC=$$first(BINARY_LIST).exe
    win32:REPOGEN=$$first(REPO_LIST).exe

    contains(QMAKE_HOST.os, Linux):{
        unix:EXEC=$$first(BINARY_LIST)
        win32:EXEC=wine $$first(BINARY_LIST).exe

        REPOGEN=$$first(REPO_LIST)
    }
}



message( selected $$EXEC and $$REPOGEN)


