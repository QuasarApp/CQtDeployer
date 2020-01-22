OTHER_FILES += \
    $$PWD/*.md \
    $$PWD/*.sh \
    $$PWD/scripts/*.sh \
    $$PWD/packages/QIF/meta/*.xml

win32:PLATFORM = windows
unix: PLATFORM = linux

relese_qif.commnad= $$PWD/scripts/QIF.sh $$PLATFORM 3.2.0 $$PWD/packages/QIF/data

QMAKE_EXTRA_TARGETS += relese_qif

!isEmpty( ONLINE ) {

    message(prepare release QIF)
    release.depends += relese_qif
}
