OTHER_FILES += \
    $$PWD/*.md \
    $$PWD/*.sh \
    $$PWD/scripts/*.sh \
    $$PWD/packages/QIF/meta/*.xml

win32:PLATFORM = windows
unix: PLATFORM = linux

qif.commands= chmod +x $$PWD/scripts/QIF.sh; $$PWD/scripts/QIF.sh $$PLATFORM 3.2.0 $$PWD/packages/QIF/data

!isEmpty( ONLINE ) {

    unix:message(prepare release QIF)
    unix:release.depends += qif
    unix:buildSnap.depends += qif
}

QMAKE_EXTRA_TARGETS += \
    qif
