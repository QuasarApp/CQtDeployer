unix:exec = $$PWD/UnitTests/build/release/UnitTests
win32:exec = $$PWD/UnitTests/build/release/UnitTests.exe

deployTest.commands = cqtdeployer -bin $$exec clear -qmake $$QMAKE_QMAKE -targetDir $$PWD/deployTests -libDir $$PWD -recursiveDepth 4

test.depends = deployTest
unix:test.commands = $$PWD/deployTests/UnitTests.sh
win32:test.commands = $$PWD/deployTests/UnitTests.exe

QMAKE_EXTRA_TARGETS += \
    deployTest \
    test
