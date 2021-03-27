#include "modulesqt515.h"

ModulesQt515::ModulesQt515()
{

}

QSet<QString> ModulesQt515::onlyC(const QString &distDir) const {

    TestUtils utils;
    auto res = ModulesQt514::onlyC(distDir);
    return res;
}

QSet<QString> ModulesQt515::qtLibs(const QString &distDir) const {
    TestUtils utils;
    auto res = ModulesQt514::qtLibs(distDir);

#ifdef Q_OS_WIN
    res -= utils.createTree({
                                "./" + distDir + "/translations/qtbase_tr.qm",
    });
#else
    res += utils.createTree({
                                "./" + distDir + "/translations/qtbase_tr.qm",
    });
#endif

    return res;
}

QSet<QString> ModulesQt515::qmlLibs(const QString &distDir) const {

    TestUtils utils;

    auto res = ModulesQt514::qmlLibs(distDir);

#ifdef Q_OS_WIN
    res += utils.createTree({
                                "./" + distDir + "/qml/QtQuick/Controls.2/Fusion/VerticalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Fusion/HorizontalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/HorizontalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Imagine/HorizontalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Imagine/VerticalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Material/HorizontalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Material/VerticalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Universal/HorizontalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Universal/VerticalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/VerticalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/designer/InsetSection.qml",
                                "./" + distDir + "/translations/qtdeclarative_tr.qm",
                                "./" + distDir + "/lib/Qt5RemoteObjects.dll",
                                "./" + distDir + "/qml/QtQml/Models.2/modelsplugin.dll",
                                "./" + distDir + "/qml/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/Models.2/qmldir",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/qtqmlremoteobjects.dll",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/qml/QtQml/StateMachine/qtqmlstatemachine.dll",
                                "./" + distDir + "/qml/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/StateMachine/qmldir",
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/workerscriptplugin.dll",
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/qml/QtQml/qmlplugin.dll",
                                "./" + distDir + "/qml/QtQml/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/qmldir",
    });
#else
    res -= utils.createTree({
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libdmabuf-server.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libdrm-egl-server.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/liblinux-dmabuf-unstable-v1.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libqt-plugin-wayland-egl.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libshm-emulation-server.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libvulkan-server.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libwayland-eglstream-controller.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libxcomposite-egl.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libxcomposite-glx.so"
                            });

    res += utils.createTree({
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-dmabuf-server-buffer.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-drm-egl-server-buffer.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-linux-dmabuf-unstable-v1.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-shm-emulation-server.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-vulkan-server.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-wayland-egl.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-wayland-eglstream-controller.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-xcomposite-egl.so",
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-xcomposite-glx.so",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Fusion/HorizontalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Fusion/VerticalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/HorizontalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Imagine/HorizontalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Imagine/VerticalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Material/HorizontalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Material/VerticalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Universal/HorizontalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/Universal/VerticalHeaderView.qml",
                                "./" + distDir + "/qml/QtQuick/Controls.2/VerticalHeaderView.qml",

                                "./" + distDir + "/qml/QtQuick/Controls.2/designer/InsetSection.qml",
                                "./" + distDir + "/translations/qtdeclarative_tr.qm",
                                "./" + distDir + "/lib/libQt5RemoteObjects.so",
                                "./" + distDir + "/qml/QtQml/Models.2/libmodelsplugin.so",
                                "./" + distDir + "/qml/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/Models.2/qmldir",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/libqtqmlremoteobjects.so",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/qml/QtQml/StateMachine/libqtqmlstatemachine.so",
                                "./" + distDir + "/qml/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/StateMachine/qmldir",
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/libworkerscriptplugin.so",
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/qml/QtQml/libqmlplugin.so",
                                "./" + distDir + "/qml/QtQml/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/qmldir",
                            }
);
#endif
    return res;
}

QSet<QString> ModulesQt515::qtWithoutTr(const QString &distDir) const {
    TestUtils utils;
    auto res = ModulesQt514::qtWithoutTr(distDir);

#ifdef Q_OS_WIN
    res += utils.createTree({
    });
#else
    res += utils.createTree({
    });
#endif
    return res;
}

QSet<QString> ModulesQt515::qtWebEngine(const QString &distDir) const {
    TestUtils utils;

    auto res = ModulesQt514::qtWebEngine(distDir);

#ifdef Q_OS_WIN
#else
    res -= utils.createTree({
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libvulkan-server.so"
    });
#endif
    return res;
}

QSet<QString> ModulesQt515::testDistroLibs(const QString &distDir) const {
    TestUtils utils;

    auto res = ModulesQt514::testDistroLibs(distDir);

#ifdef Q_OS_WIN

    res += utils.createTree({
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Fusion/VerticalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Fusion/HorizontalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/HorizontalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Imagine/HorizontalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Imagine/VerticalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Material/HorizontalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Material/VerticalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Universal/HorizontalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Universal/VerticalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/VerticalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/designer/InsetSection.qml",
                                "./" + distDir + "/package2/ZzZ/translations/qtdeclarative_tr.qm",
                                "./" + distDir + "/package2/ZzZ/translations/qtbase_tr.qm",
                                "./" + distDir + "/lolTr/qtbase_tr.qm",
                                "./" + distDir + "/package2/ZzZ/Qt5RemoteObjects.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/modelsplugin.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/qtqmlremoteobjects.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/qtqmlstatemachine.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/workerscriptplugin.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/qmlplugin.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/qmldir",


    });

#else
    res += utils.createTree({
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-dmabuf-server-buffer.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-drm-egl-server-buffer.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-linux-dmabuf-unstable-v1.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-shm-emulation-server.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-vulkan-server.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-wayland-egl.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-wayland-eglstream-controller.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-xcomposite-egl.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-xcomposite-glx.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Fusion/HorizontalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Fusion/VerticalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/HorizontalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Imagine/HorizontalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Imagine/VerticalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Material/HorizontalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Material/VerticalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Universal/HorizontalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/Universal/VerticalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/VerticalHeaderView.qml",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQuick/Controls.2/designer/InsetSection.qml",
                                "./" + distDir + "/package2/ZzZ/translations/qtdeclarative_tr.qm",
                                "./" + distDir + "/package2/ZzZ/translations/qtbase_tr.qm",
                                "./" + distDir + "/lolTr/qtbase_tr.qm",
                                "./" + distDir + "/package2/ZzZ/lib/libQt5RemoteObjects.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/libmodelsplugin.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/libqtqmlremoteobjects.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/libqtqmlstatemachine.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/libworkerscriptplugin.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/libqmlplugin.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/qmldir",

                            });

    res -= utils.createTree({
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libdmabuf-server.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libdrm-egl-server.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/liblinux-dmabuf-unstable-v1.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libqt-plugin-wayland-egl.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libshm-emulation-server.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libvulkan-server.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libwayland-eglstream-controller.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libxcomposite-egl.so",
                                "./" + distDir + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libxcomposite-glx.so"
                            });
#endif
    return res;
}

QSet<QString> ModulesQt515::testOutLibs(const QString &distDir) const {
    TestUtils utils;

    auto res = ModulesQt514::testOutLibs(distDir);

#ifdef Q_OS_WIN
    res += utils.createTree({
                                "./" + distDir + "/q/QtQuick/Controls.2/Fusion/VerticalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Fusion/HorizontalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/HorizontalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Imagine/HorizontalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Imagine/VerticalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Material/HorizontalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Material/VerticalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Universal/HorizontalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Universal/VerticalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/VerticalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/designer/InsetSection.qml",
                                "./" + distDir + "/lolTr/qtdeclarative_tr.qm",
                                "./" + distDir + "/lolTr/qtbase_tr.qm",
                                "./" + distDir + "/lolLib/Qt5RemoteObjects.dll",
                                "./" + distDir + "/q/QtQml/Models.2/modelsplugin.dll",
                                "./" + distDir + "/q/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/Models.2/qmldir",
                                "./" + distDir + "/q/QtQml/RemoteObjects/qtqmlremoteobjects.dll",
                                "./" + distDir + "/q/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/q/QtQml/StateMachine/qtqmlstatemachine.dll",
                                "./" + distDir + "/q/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/StateMachine/qmldir",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/workerscriptplugin.dll",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/q/QtQml/qmlplugin.dll",
                                "./" + distDir + "/q/QtQml/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/qmldir",

    });
#else
    res += utils.createTree({
                                "./" + distDir + "/q/QtQuick/Controls.2/Fusion/HorizontalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Fusion/VerticalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/HorizontalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Imagine/HorizontalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Imagine/VerticalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Material/HorizontalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Material/VerticalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Universal/HorizontalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/Universal/VerticalHeaderView.qml",
                                "./" + distDir + "/q/QtQuick/Controls.2/VerticalHeaderView.qml",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libqt-wayland-compositor-dmabuf-server-buffer.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libqt-wayland-compositor-drm-egl-server-buffer.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libqt-wayland-compositor-linux-dmabuf-unstable-v1.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libqt-wayland-compositor-shm-emulation-server.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libqt-wayland-compositor-vulkan-server.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libqt-wayland-compositor-wayland-egl.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libqt-wayland-compositor-wayland-eglstream-controller.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libqt-wayland-compositor-xcomposite-egl.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libqt-wayland-compositor-xcomposite-glx.so",
                                "./" + distDir + "/q/QtQuick/Controls.2/designer/InsetSection.qml",
                                "./" + distDir + "/lolTr/qtdeclarative_tr.qm",
                                "./" + distDir + "/lolTr/qtbase_tr.qm",
                                "./" + distDir + "/lolLib/libQt5RemoteObjects.so",
                                "./" + distDir + "/q/QtQml/Models.2/libmodelsplugin.so",
                                "./" + distDir + "/q/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/Models.2/qmldir",
                                "./" + distDir + "/q/QtQml/RemoteObjects/libqtqmlremoteobjects.so",
                                "./" + distDir + "/q/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/q/QtQml/StateMachine/libqtqmlstatemachine.so",
                                "./" + distDir + "/q/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/StateMachine/qmldir",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/libworkerscriptplugin.so",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/q/QtQml/libqmlplugin.so",
                                "./" + distDir + "/q/QtQml/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/qmldir",
                            });

    res -= utils.createTree({
                                "./" + distDir + "/p/wayland-graphics-integration-server/libdmabuf-server.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libdrm-egl-server.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/liblinux-dmabuf-unstable-v1.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libqt-plugin-wayland-egl.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libshm-emulation-server.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libvulkan-server.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libwayland-eglstream-controller.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libxcomposite-egl.so",
                                "./" + distDir + "/p/wayland-graphics-integration-server/libxcomposite-glx.so",

                            });
#endif
    return res;
}

QSet<QString> ModulesQt515::qtWebEngineWidgets(const QString &distDir) const
{
    TestUtils utils;

    auto res = ModulesQt514::qtWebEngineWidgets(distDir);

#ifdef Q_OS_WIN
#else
    res -= utils.createTree({
                                "./" + distDir + "/plugins/wayland-graphics-integration-server/libvulkan-server.so"
    });
#endif
    return res;
}
