include($$PWD/../installer/InstallerBase.pri);
mkpath( $$PWD/../Distro)
win32:OUT_FILE = CQtDeployerInstaller.exe
unix:OUT_FILE = CQtDeployerInstaller.run

win32:OUT_FILE_OFF = CQtDeployerOfflineInstaller.exe
unix:OUT_FILE_OFF = CQtDeployerOfflineInstaller.run

DEPLOY_TARGET = $$PWD/../CQtDeployer/build/release

DATA_DIR = $$PWD/packages/cqtdeployer_1_4/data/1.4
META_DIR = $$PWD/packages/cqtdeployer_1_4/meta/

win32:OUT_LIB= -libOut .
win32:OUT_BIN= -binOut .

BASE_DEPLOY_FLAGS = clear -qmake $$QMAKE_BIN -libDir $$PWD/../ -recursiveDepth 4 -ignoreEnv $$DEPLOY_TARGET
BASE_DEPLOY_FLAGS_CQT = $$BASE_DEPLOY_FLAGS -targetDir $$DATA_DIR $$OUT_LIB $$OUT_BIN

deploy_dep.commands += $$DEPLOYER -bin $$DEPLOY_TARGET $$BASE_DEPLOY_FLAGS_CQT

mkpath( $$PWD/../Distro)

win32:CONFIG_FILE = $$PWD/config/configWin.xml
unix:CONFIG_FILE = $$PWD/config/configLinux.xml

deployOffline.commands = $$EXEC \
                       --offline-only \
                       -c $$CONFIG_FILE \
                       -p $$PWD/packages \
                       $$PWD/../Distro/$$OUT_FILE_OFF

deploy.depends = deploy_dep
deploy.depends += deployOffline

win32:ONLINE_REPO_DIR = $$ONLINE/CQtDeployer/Windows
unix:ONLINE_REPO_DIR = $$ONLINE/CQtDeployer/Linux

create_repo.commands = $$REPOGEN \
                        --update-new-components \
                        -p $$PWD/packages \
                        $$ONLINE_REPO_DIR

chmodSnap.commands = chmod 777 -R $$DATA_DIR
unix:release.depends += chmodSnap


message( ONLINE_REPO_DIR $$ONLINE_REPO_DIR)
!isEmpty( ONLINE ) {

    message(online)

    release.depends = create_repo

    deploy.commands = $$EXEC \
                           --online-only \
                           -c $$CONFIG_FILE \
                           -p $$PWD/packages \
                           $$PWD/../Distro/$$OUT_FILE
}

releaseSnap.commands = rm *.snap -rdf && chmod 777 -R $$PWD/../prime && snapcraft && snapcraft push *.snap # bad patern
buildSnap.commands = snapcraft
clearSnap.commands = rm parts prime stage *.snap -rdf

unix:release.depends += clearSnap
unix:release.depends += buildSnap
unix:release.depends += releaseSnap

OTHER_FILES += \
    $$META_DIR/* \
    $$PWD/config/*.*


QMAKE_EXTRA_TARGETS += \
    deploy_dep \
    deployOffline \
    deploy \
    create_repo \
    release \
    clearSnap \
    releaseSnap \
    buildSnap \
    chmodSnap
