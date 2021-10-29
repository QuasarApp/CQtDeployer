VERSION = 1.5.4.9

include($$PWD/InstallerBase.pri);
mkpath( $$PWD/../Distro)

message(QMAKE_HOST.arch = $$QMAKE_HOST.arch)

win32:OUT_FILE = CQtDeployer_'$$VERSION'_Installer_Win'_$$QMAKE_HOST.arch'.exe
unix:OUT_FILE = CQtDeployer_'$$VERSION'_Installer_Linux'_$$QMAKE_HOST.arch'.run

win32:OUT_FILE_OFF = CQtDeployer_'$$VERSION'_OfflineInstaller_Win'_$$QMAKE_HOST.arch'.exe
unix:OUT_FILE_OFF = CQtDeployer_'$$VERSION'_OfflineInstaller_Linux'_$$QMAKE_HOST.arch'.run

DEPLOY_TARGET = $$PWD/../CQtDeployer/build/release

DATA_DIR = $$PWD/packages/cqtdeployer.1_5/data/1.5
META_DIR = $$PWD/packages/cqtdeployer.1_5/meta/

win32:OUT_LIB= -libOut lib
win32:OUT_BIN= -binOut bin

BASE_DEPLOY_FLAGS = clear -qmake $$QMAKE_BIN -libDir $$PWD/../ -recursiveDepth 4 -ignoreEnv $$DEPLOY_TARGET -extraLibs icu
BASE_DEPLOY_FLAGS_CQT = $$BASE_DEPLOY_FLAGS -targetDir $$DATA_DIR $$OUT_LIB $$OUT_BIN


win32:CQT_ICON = -icon $$PWD/config/icon.ico
unix:CQT_ICON = -icon $$PWD/config/logo.png
BASE_DEPLOY_FLAGS_DEB = $$BASE_DEPLOY_FLAGS -targetDir $$PWD/../Distro $$OUT_LIB $$OUT_BIN deb zip -name CQtDeployer -publisher QuasarApp $$CQT_ICON -deployVersion 1.5.4.9 -debOut CQtDeployer_'$$VERSION'_Linux'_$$QMAKE_HOST.arch'.deb -zipOut CQtDeployer_'$$VERSION'_Linux'_$$QMAKE_HOST.arch'.zip

DEPLOY_TARGET_DEB = $$DEPLOY_TARGET,$$PWD/packages/QIF/data/QIF
deploy_dep.commands += $$DEPLOYER -bin $$DEPLOY_TARGET $$BASE_DEPLOY_FLAGS_CQT
deploy_deb.commands += $$DEPLOYER -bin $$DEPLOY_TARGET_DEB $$BASE_DEPLOY_FLAGS_DEB

win32:CONFIG_FILE = $$PWD/config/configWin.xml
unix:CONFIG_FILE = $$PWD/config/configLinux.xml

deployOffline.commands = $$EXEC \
                       --offline-only \
                       -c $$CONFIG_FILE \
                       -p $$PWD/packages \
                       $$PWD/../Distro/$$OUT_FILE_OFF


!contains(QMAKE_HOST.arch, arm.*):{
    deploy.depends = deploy_dep
    deploy.depends += deployOffline

}

unix:deploy.depends += deploy_deb

win32:ONLINE_REPO_DIR = $$ONLINE/CQtDeployer/Windows
unix:ONLINE_REPO_DIR = $$ONLINE/CQtDeployer/Linux

win32:PY = python
unix: PY = python3

create_repo.commands = $$PY $$PWD/../QuasarAppScripts/cp.py $$PWD/packages/ $$PWD/../Repo

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
clearSnap2.commands = rm parts prime stage -rdf

deploySnap.commands = rm *.snap -rdf && chmod 777 -R $$PWD/../prime && snapcraft && cp *.snap $$PWD/../Distro/
releaseSnap.commands = snapcraft push *.snap # bad patern



!isEmpty( ONLINE ) {

    !contains(QMAKE_HOST.arch, arm.*):{

        message(Snap)
        unix:deploy.depends += clearSnap
        unix:deploy.depends += buildSnap
        unix:deploy.depends += deploySnap
        unix:deploy.depends += clearSnap2
        unix:release.depends += releaseSnap
    }
}

OTHER_FILES += \
    $$META_DIR/* \
    $$PWD/config/*.*

include(QIF.pri)

QMAKE_EXTRA_TARGETS += \
    deploy_dep \
    deploy_deb \
    deployOffline \
    deploy \
    create_repo \
    release \
    clearSnap \
    clearSnap2 \
    deploySnap \
    releaseSnap \
    buildSnap \
    chmodSnap


!contains(DEFINES, WITHOUT_TR) {

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
}
