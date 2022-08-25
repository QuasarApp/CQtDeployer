#include "modulesqt6_3.h"

ModulesQt63::ModulesQt63() {

}

QSet<QString> ModulesQt63::qmlLibs(const QString &distDir) const {
    TestUtils utils;

    auto res = ModulesQt5152::qmlLibs(distDir);
    return res;
}

QSet<QString> ModulesQt63::testDistroLibs(const QString &distDir) const {
    TestUtils utils;

    auto res = ModulesQt5152::testDistroLibs(distDir);
    return res;
}

QSet<QString> ModulesQt63::testOutLibs(const QString &distDir) const {
    TestUtils utils;

    auto res = ModulesQt5152::testOutLibs(distDir);

    return res;


}

QSet<QString> ModulesQt63::onlyC(const QString &distDir) const {
    return ModulesQt5152::onlyC(distDir);
}

QSet<QString> ModulesQt63::qtLibs(const QString &distDir) const {

        TestUtils utils;

        auto Tree = onlyC(distDir);

        Tree = ignoreFilter(Tree, "/TestOnlyC");

#ifdef Q_OS_WIN
        Tree += utils.createTree(
            {
                "./" + distDir + "/QtWidgetsProject.sh",
                "./" + distDir + "/bin/QtWidgetsProject",
                "./" + distDir + "/bin/qt.conf",
                "./" + distDir + "/Qt6Core.so",
                "./" + distDir + "/Qt6DBus.so",
                "./" + distDir + "/Qt6EglFSDeviceIntegration.so",
                "./" + distDir + "/Qt6Gui.so",
                "./" + distDir + "/Qt6Network.so",
                "./" + distDir + "/Qt6OpenGL.so",
                "./" + distDir + "/Qt6Pdf.so",
                "./" + distDir + "/Qt6Svg.so",
                "./" + distDir + "/Qt6WaylandClient.so",
                "./" + distDir + "/Qt6WaylandEglClientHwIntegration.so",
                "./" + distDir + "/Qt6Widgets.so",
                "./" + distDir + "/Qt6WlShellIntegration.so",
                "./" + distDir + "/Qt6XcbQpa.so",
                "./" + distDir + "/lib/libicudata.so",
                "./" + distDir + "/lib/libicui18n.so",
                "./" + distDir + "/lib/libicuuc.so",
                "./" + distDir + "/plugins/iconengines/libqsvgicon.so",
                "./" + distDir + "/plugins/imageformats/libqgif.so",
                "./" + distDir + "/plugins/imageformats/libqicns.so",
                "./" + distDir + "/plugins/imageformats/libqico.so",
                "./" + distDir + "/plugins/imageformats/libqjpeg.so",
                "./" + distDir + "/plugins/imageformats/libqpdf.so",
                "./" + distDir + "/plugins/imageformats/libqsvg.so",
                "./" + distDir + "/plugins/imageformats/libqtga.so",
                "./" + distDir + "/plugins/imageformats/libqtiff.so",
                "./" + distDir + "/plugins/imageformats/libqwbmp.so",
                "./" + distDir + "/plugins/imageformats/libqwebp.so",
                "./" + distDir + "/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so",
                "./" + distDir + "/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so",
                "./" + distDir + "/plugins/platforms/libqeglfs.so",
                "./" + distDir + "/plugins/platforms/libqlinuxfb.so",
                "./" + distDir + "/plugins/platforms/libqminimal.so",
                "./" + distDir + "/plugins/platforms/libqminimalegl.so",
                "./" + distDir + "/plugins/platforms/libqoffscreen.so",
                "./" + distDir + "/plugins/platforms/libqwayland-egl.so",
                "./" + distDir + "/plugins/platforms/libqwayland-generic.so",
                "./" + distDir + "/plugins/platforms/libqxcb.so",
                "./" + distDir + "/plugins/platformthemes/libqgtk3.so",
                "./" + distDir + "/plugins/platformthemes/libqxdgdesktopportal.so",
                "./" + distDir + "/plugins/wayland-shell-integration/libfullscreen-shell-v1.so",
                "./" + distDir + "/plugins/wayland-shell-integration/libivi-shell.so",
                "./" + distDir + "/plugins/wayland-shell-integration/libqt-shell.so",
                "./" + distDir + "/plugins/wayland-shell-integration/libwl-shell-plugin.so",
                "./" + distDir + "/plugins/wayland-shell-integration/libxdg-shell.so",
                "./" + distDir + "/plugins/xcbglintegrations/libqxcb-egl-integration.so",
                "./" + distDir + "/plugins/xcbglintegrations/libqxcb-glx-integration.so",
                "./" + distDir + "/translations/qtbase_ar.qm",
                "./" + distDir + "/translations/qtbase_bg.qm",
                "./" + distDir + "/translations/qtbase_ca.qm",
                "./" + distDir + "/translations/qtbase_cs.qm",
                "./" + distDir + "/translations/qtbase_da.qm",
                "./" + distDir + "/translations/qtbase_de.qm",
                "./" + distDir + "/translations/qtbase_en.qm",
                "./" + distDir + "/translations/qtbase_es.qm",
                "./" + distDir + "/translations/qtbase_fa.qm",
                "./" + distDir + "/translations/qtbase_fi.qm",
                "./" + distDir + "/translations/qtbase_fr.qm",
                "./" + distDir + "/translations/qtbase_gd.qm",
                "./" + distDir + "/translations/qtbase_he.qm",
                "./" + distDir + "/translations/qtbase_hr.qm",
                "./" + distDir + "/translations/qtbase_hu.qm",
                "./" + distDir + "/translations/qtbase_it.qm",
                "./" + distDir + "/translations/qtbase_ja.qm",
                "./" + distDir + "/translations/qtbase_ko.qm",
                "./" + distDir + "/translations/qtbase_lv.qm",
                "./" + distDir + "/translations/qtbase_nl.qm",
                "./" + distDir + "/translations/qtbase_nn.qm",
                "./" + distDir + "/translations/qtbase_pl.qm",
                "./" + distDir + "/translations/qtbase_pt_BR.qm",
                "./" + distDir + "/translations/qtbase_ru.qm",
                "./" + distDir + "/translations/qtbase_sk.qm",
                "./" + distDir + "/translations/qtbase_tr.qm",
                "./" + distDir + "/translations/qtbase_uk.qm",
                "./" + distDir + "/translations/qtbase_zh_CN.qm",
                "./" + distDir + "/translations/qtbase_zh_TW.qm"

             });

#else
        Tree += utils.createTree(
            {
                "./" + distDir + "/QtWidgetsProject.sh",
                "./" + distDir + "/bin/QtWidgetsProject",
                "./" + distDir + "/bin/qt.conf",
                "./" + distDir + "/lib/libQt6Core.so",
                "./" + distDir + "/lib/libQt6DBus.so",
                "./" + distDir + "/lib/libQt6EglFSDeviceIntegration.so",
                "./" + distDir + "/lib/libQt6Gui.so",
                "./" + distDir + "/lib/libQt6Network.so",
                "./" + distDir + "/lib/libQt6OpenGL.so",
                "./" + distDir + "/lib/libQt6Pdf.so",
                "./" + distDir + "/lib/libQt6Svg.so",
                "./" + distDir + "/lib/libQt6WaylandClient.so",
                "./" + distDir + "/lib/libQt6WaylandEglClientHwIntegration.so",
                "./" + distDir + "/lib/libQt6Widgets.so",
                "./" + distDir + "/lib/libQt6WlShellIntegration.so",
                "./" + distDir + "/lib/libQt6XcbQpa.so",
                "./" + distDir + "/lib/libicudata.so",
                "./" + distDir + "/lib/libicui18n.so",
                "./" + distDir + "/lib/libicuuc.so",
                "./" + distDir + "/plugins/iconengines/libqsvgicon.so",
                "./" + distDir + "/plugins/imageformats/libqgif.so",
                "./" + distDir + "/plugins/imageformats/libqicns.so",
                "./" + distDir + "/plugins/imageformats/libqico.so",
                "./" + distDir + "/plugins/imageformats/libqjpeg.so",
                "./" + distDir + "/plugins/imageformats/libqpdf.so",
                "./" + distDir + "/plugins/imageformats/libqsvg.so",
                "./" + distDir + "/plugins/imageformats/libqtga.so",
                "./" + distDir + "/plugins/imageformats/libqtiff.so",
                "./" + distDir + "/plugins/imageformats/libqwbmp.so",
                "./" + distDir + "/plugins/imageformats/libqwebp.so",
                "./" + distDir + "/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so",
                "./" + distDir + "/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so",
                "./" + distDir + "/plugins/platforms/libqeglfs.so",
                "./" + distDir + "/plugins/platforms/libqlinuxfb.so",
                "./" + distDir + "/plugins/platforms/libqminimal.so",
                "./" + distDir + "/plugins/platforms/libqminimalegl.so",
                "./" + distDir + "/plugins/platforms/libqoffscreen.so",
                "./" + distDir + "/plugins/platforms/libqwayland-egl.so",
                "./" + distDir + "/plugins/platforms/libqwayland-generic.so",
                "./" + distDir + "/plugins/platforms/libqxcb.so",
                "./" + distDir + "/plugins/platformthemes/libqgtk3.so",
                "./" + distDir + "/plugins/platformthemes/libqxdgdesktopportal.so",
                "./" + distDir + "/plugins/wayland-shell-integration/libfullscreen-shell-v1.so",
                "./" + distDir + "/plugins/wayland-shell-integration/libivi-shell.so",
                "./" + distDir + "/plugins/wayland-shell-integration/libqt-shell.so",
                "./" + distDir + "/plugins/wayland-shell-integration/libwl-shell-plugin.so",
                "./" + distDir + "/plugins/wayland-shell-integration/libxdg-shell.so",
                "./" + distDir + "/plugins/xcbglintegrations/libqxcb-egl-integration.so",
                "./" + distDir + "/plugins/xcbglintegrations/libqxcb-glx-integration.so",
                "./" + distDir + "/translations/qtbase_ar.qm",
                "./" + distDir + "/translations/qtbase_bg.qm",
                "./" + distDir + "/translations/qtbase_ca.qm",
                "./" + distDir + "/translations/qtbase_cs.qm",
                "./" + distDir + "/translations/qtbase_da.qm",
                "./" + distDir + "/translations/qtbase_de.qm",
                "./" + distDir + "/translations/qtbase_en.qm",
                "./" + distDir + "/translations/qtbase_es.qm",
                "./" + distDir + "/translations/qtbase_fa.qm",
                "./" + distDir + "/translations/qtbase_fi.qm",
                "./" + distDir + "/translations/qtbase_fr.qm",
                "./" + distDir + "/translations/qtbase_gd.qm",
                "./" + distDir + "/translations/qtbase_he.qm",
                "./" + distDir + "/translations/qtbase_hr.qm",
                "./" + distDir + "/translations/qtbase_hu.qm",
                "./" + distDir + "/translations/qtbase_it.qm",
                "./" + distDir + "/translations/qtbase_ja.qm",
                "./" + distDir + "/translations/qtbase_ko.qm",
                "./" + distDir + "/translations/qtbase_lv.qm",
                "./" + distDir + "/translations/qtbase_nl.qm",
                "./" + distDir + "/translations/qtbase_nn.qm",
                "./" + distDir + "/translations/qtbase_pl.qm",
                "./" + distDir + "/translations/qtbase_pt_BR.qm",
                "./" + distDir + "/translations/qtbase_ru.qm",
                "./" + distDir + "/translations/qtbase_sk.qm",
                "./" + distDir + "/translations/qtbase_tr.qm",
                "./" + distDir + "/translations/qtbase_uk.qm",
                "./" + distDir + "/translations/qtbase_zh_CN.qm",
                "./" + distDir + "/translations/qtbase_zh_TW.qm"
            }
            );
#endif
        return Tree;
}

QSet<QString> ModulesQt63::qmlVirtualKeyBoadrLibs(const QString &distDir) const
{

}

QSet<QString> ModulesQt63::qtWithoutTr(const QString &distDir) const
{

}

QSet<QString> ModulesQt63::qtWebEngine(const QString &distDir) const
{

}

QSet<QString> ModulesQt63::qtWebEngineWidgets(const QString &distDir) const
{

}

QSet<QString> ModulesQt63::testEmptyParamsTree(const QString &distDir) const
{

}
