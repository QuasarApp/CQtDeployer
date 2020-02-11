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

create_repo.commands = python3 shell_quote($$PWD/QuasarAppScripts/cp.py) $$PWD/packages/ $$PWD/../Repo

chmodSnap.commands = chmod 777 -R $$DATA_DIR
unix:deploy.depends += chmodSnap


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

buildSnap.commands = snapcraft
clearSnap.commands = rm parts prime stage *.snap -rdf
deploySnap.commands = rm *.snap -rdf && chmod 777 -R $$PWD/../prime && snapcraft && cp *.snap $$PWD/../Distro/
releaseSnap.commands = snapcraft push *.snap # bad patern



!isEmpty( ONLINE ) {

    message(Snap)
    unix:deploy.depends += clearSnap
    unix:deploy.depends += buildSnap
    unix:deploy.depends += deploySnap
    unix:release.depends += releaseSnap
}

OTHER_FILES += \
    $$META_DIR/* \
    $$PWD/config/*.*

include(QIF.pri)

!isEmpty( ONLINE ) {

    message(Snap)
    unix:deploy.depends += clearSnap
    unix:deploy.depends += buildSnap
    unix:deploy.depends += deploySnap
    unix:release.depends += releaseSnap
}

QMAKE_EXTRA_TARGETS += \
    deploy_dep \
    deployOffline \
    deploy \
    create_repo \
    release \
    clearSnap \
    deploySnap \
    releaseSnap \
    buildSnap \
    chmodSnap


# Translations
SUPPORT_LANGS = ru

defineReplace(findFiles) {
    patern = $$1
    path = $$2

    all_files = $$files(*$${patern}, true)
    win32:all_files ~= s|\\\\|/|g
    win32:path ~= s|\\\\|/|g

    for(file, all_files) {
        result += $$find(file, $$path)
    }

    return($$result)
}

XML_FILES = $$files(*.xml, true)

for(LANG, SUPPORT_LANGS) {
    for(XML, XML_FILES) {
        FILE_PATH = $$dirname(XML)

        JS_FILES = $$findFiles(".js", $$FILE_PATH)
        UI_FILES = $$findFiles(".ui", $$FILE_PATH)

        commands += "$$LUPDATE $$JS_FILES $$UI_FILES -ts $$FILE_PATH/$${LANG}.ts"
        TS_FILES += $$FILE_PATH/$${LANG}.ts

    }

    for(TS, TS_FILES) {
        commands += "$$LRELEASE $$TS"
    }
}

for(command, commands) {
    system($$command)|error("Failed to run: $$command")
}
