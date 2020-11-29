TEMPLATE = app

QT += webenginewidgets

HEADERS += \
    webuihandler.h

SOURCES += \
    main.cpp \
    webuihandler.cpp

RESOURCES += \
    webui.qrc

DESTDIR="$$PWD/../build"
