!isEmpty(INSTALL_REFIX_PRI_INCLUDED):error("install_prefix.pri already included")
INSTALL_REFIX_PRI_INCLUDED = 1

unix:libfiletype=*.so*
win32:libfiletype=*.dll
unix:runfiletype=*
win32:runfiletype=*.exe

isEmpty(PREFIX_BIN) {
    isEmpty(PREFIX) {
        PREFIX_BIN = $$PWD/distro
    } else {
        unix:PREFIX_BIN = $$PREFIX/bin
        win32:PREFIX_BIN = $$PREFIX
    }
}

isEmpty(PREFIX_LIB) {
    isEmpty(PREFIX) {
        PREFIX_LIB = $$PWD/distro
    } else {
        unix:PREFIX_LIB = $$PREFIX/lib
        win32:PREFIX_LIB = $$PREFIX
    }
}

target_bin.path = $$PREFIX_BIN
target_bin.CONFIG += no_check_exist
target_lib.path = $$PREFIX_LIB
target_lib.CONFIG += no_check_exist

INSTALLS += target_bin target_lib

