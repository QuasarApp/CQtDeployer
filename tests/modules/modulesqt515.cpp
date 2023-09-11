/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

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
    res += utils.createTree({
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
