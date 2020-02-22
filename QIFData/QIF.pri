OTHER_FILES += \
    $$PWD/*.md \
    $$PWD/*.sh \
    $$PWD/scripts/*.sh \
    $$PWD/packages/QIF/meta/*.xml

win32:PLATFORM = windows
unix: PLATFORM = linux

win32:PY = python
unix: PY = python3

qif.commands= $$PY $$PWD/scripts/QIF.py $$PLATFORM 3.2.0 $$PWD/packages/QIF/data

!isEmpty( ONLINE ) {

    unix:message(prepare release QIF)
    unix:deployOffline.depends += qif
    unix:buildSnap.depends += qif
}

QMAKE_EXTRA_TARGETS += \
    qif
