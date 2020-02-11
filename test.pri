unix:exec = shell_quote($$PWD/UnitTests/build/release/UnitTests)
win32:exec = shell_quote($$PWD/UnitTests/build/release/UnitTests.exe)

QT_DIR= $$[QT_HOST_BINS]
win32:QMAKE_BIN= $$QT_DIR/qmake.exe
contains(QMAKE_HOST.os, Linux):{
    QMAKE_BIN= $$QT_DIR/qmake
}

DEPLOYER=cqtdeployer
win32:DEPLOYER=$$cqtdeployer


deployTest.commands = shell_quote($$DEPLOYER) -bin $$exec clear -qmake shell_quote($$QMAKE_BIN) -targetDir shell_quote($$PWD/deployTests) -libDir shell_quote($$PWD) -recursiveDepth 4


test.depends = deployTest
unix:!android:test.commands = $$PWD/deployTests/UnitTests.sh -maxwarnings 100000
win32:test.commands = $$PWD/deployTests/UnitTests.exe -maxwarnings 100000 -o $$PWD/buildLog.log

contains(QMAKE_HOST.os, Linux):{
    win32:test.commands =
}


QMAKE_EXTRA_TARGETS += \
    deployTest \
    test


