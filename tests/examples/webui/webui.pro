TEMPLATE = app

QT += webenginewidgets

HEADERS += \
    webuihandler.h

SOURCES += \
    main.cpp \
    webuihandler.cpp

RESOURCES += \
    webui.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/webenginewidgets/webui
INSTALLS += target
