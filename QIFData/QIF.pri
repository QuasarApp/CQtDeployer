OTHER_FILES += \
    $$PWD/*.md \
    $$PWD/*.sh \
    $$PWD/scripts/*.py \
    $$PWD/packages/QIF/meta/*.xml

win32:PLATFORM = windows
unix: PLATFORM = linux

win32:PY = python
unix: PY = python3

qif.commands= $$PY $$PWD/scripts/QIF.py $$PLATFORM 4.0.1-202012091043 $$PWD/packages/QIF/data
alias.commands= $$PY $$PWD/scripts/alias.py

!isEmpty( ONLINE ) {

    message(prepare release QIF)
    deployOffline.depends += qif
    buildSnap.depends += qif
}

unix:deployOffline.depends += alias

QMAKE_EXTRA_TARGETS += \
    qif \
    alias
