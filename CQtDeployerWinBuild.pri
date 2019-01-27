#
# Copyright (C) 2018-2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

win32:CONFIG(release, debug|release): {
    DEPLOYER = $$PWD/CQtDeployerBinaries/Windows/cqtdeployer.exe
    DEPLOY_TARGET = $$PWD/CQtDeployer/build/release/cqtdeployer.exe
    BASE_DEPLOY_FLAGS = clear -qmake $$QMAKE_QMAKE -targetDir $$PWD/distro -libDir $$PWD/ -recursiveDepth 5

    deployTarget.commands = $$DEPLOYER $$BASE_DEPLOY_FLAGS -bin $$DEPLOY_TARGET

    QMAKE_EXTRA_TARGETS += \
        deployTarget

}
