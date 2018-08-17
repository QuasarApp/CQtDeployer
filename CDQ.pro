TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += CDQ \
    QuasarAppLib

QuasarAppLib.file = $$PWD/QuasarAppLib/QuasarApp.pro
