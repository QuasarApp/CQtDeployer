!isEmpty(INSTALL_REFIX_PRI_INCLUDED):error("install_prefix.pri already included")
INSTALL_REFIX_PRI_INCLUDED = 1

CONFIG(release, debug|release): {
    DESTDIR = $$PWD/build/release
} else {
    DESTDIR = $$PWD/build/debug
}

unix:libfiletype=*.so*
win32:libfiletype=*.dll
unix:runfiletype=*
win32:runfiletype=*.exe

isEmpty(PREFIX_BIN) {
    isEmpty(PREFIX) {
        PREFIX_BIN = $$PWD/distro
    } else {
        PREFIX_BIN = $$PREFIX
    }
}

install_data.path = $$PREFIX_BIN
install_data.CONFIG += no_check_exist

INSTALLS += install_data
