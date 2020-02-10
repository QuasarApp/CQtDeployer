unix:exec = $$PWD/UnitTests/build/release/UnitTests
win32:exec = $$PWD/UnitTests/build/release/UnitTests.exe

QT_DIR= $$[QT_HOST_BINS]
win32:QMAKE_BIN= $$QT_DIR/qmake.exe
contains(QMAKE_HOST.os, Linux):{
    QMAKE_BIN= $$QT_DIR/qmake
}

deployTest.commands = cqtdeployer -bin $$exec clear -qmake $$QMAKE_BIN -targetDir $$PWD/deployTests -libDir $$PWD -recursiveDepth 4


test.depends = deployTest
unix:!android:test.commands = $$PWD/deployTests/UnitTests.sh -maxwarnings 100000
win32:test.commands = $$PWD/deployTests/UnitTests.exe -maxwarnings 100000 -o $$PWD/_logTemp/build.log

contains(QMAKE_HOST.os, Linux):{
    win32:test.commands =
}


QMAKE_EXTRA_TARGETS += \
    deployTest \
    test


