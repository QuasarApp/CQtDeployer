/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deployconfig.h"
#include "pluginsparser.h"
#include <QDir>
#include <dependenciesscanner.h>
#include <quasarapp.h>

PluginsParser::PluginsParser(){
}

// This maping using for the plugins group. But not all plugins from group should be copyed.
// For example the platforminputcontexts group plugins contains the qtvirtualkeyboardplugin. This plugin depends of the QtVirtualKeyboadr library.
// But this plugin should be copied only when the deistribution has a QtVirtualKeyboard module.
// If the qtvirtualkeyboardplugin plugin will be copied befor the adding QtVirtualKeyboard then in the  distribution will have a not used Qt module.
// For fix this issue we need to add plugins into ditalsSluginModuleMappings list.
// all plugins from array ditalsSluginModuleMappings will have their modules updated.

static const PluginModuleMapping pluginModuleMappings[] =
{
    {"qml1tooling", DeployCore::QtModule::QtDeclarativeModule},
    {"gamepads", DeployCore::QtModule::QtGamePadModule},
    {"accessible", DeployCore::QtModule::QtGuiModule},
    {"iconengines", DeployCore::QtModule::QtGuiModule},
    {"imageformats", DeployCore::QtModule::QtGuiModule},
    {"platforms", DeployCore::QtModule::QtGuiModule},
    {"platformthemes", DeployCore::QtModule::QtGuiModule},
    {"platforminputcontexts", DeployCore::QtModule::QtGuiModule},
    {"virtualkeyboard", DeployCore::QtModule::QtVirtualKeyboard},
    {"geoservices", DeployCore::QtModule::QtLocationModule},
    {"audio", DeployCore::QtModule::QtMultimediaModule},
    {"mediaservice", DeployCore::QtModule::QtMultimediaModule},
    {"playlistformats", DeployCore::QtModule::QtMultimediaModule},
    {"bearer", DeployCore::QtModule::QtNetworkModule},
    {"position", DeployCore::QtModule::QtPositioningModule},
    {"printsupport", DeployCore::QtModule::QtPrintSupportModule},
    {"scenegraph", DeployCore::QtModule::QtQuickModule},
    {"qmltooling", DeployCore::QtModule::QtQmlToolingModule},
    {"sensors", DeployCore::QtModule::QtSensorsModule},
    {"sensorgestures", DeployCore::QtModule::QtSensorsModule},
    {"canbus", DeployCore::QtModule::QtSerialBusModule},
    {"sqldrivers", DeployCore::QtModule::QtSqlModule},
    {"texttospeech", DeployCore::QtModule::QtTextToSpeechModule},
    {"qtwebengine", DeployCore::QtModule::QtWebEngineModule | DeployCore::QtModule::QtWebEngineCoreModule | DeployCore::QtModule::QtWebEngineWidgetsModule},
    {"styles", DeployCore::QtModule::QtWidgetsModule},
    {"sceneparsers", DeployCore::QtModule::Qt3DRendererModule},
    {"renderplugins", DeployCore::QtModule::Qt3DRendererModule},
    {"renderers", DeployCore::QtModule::Qt3DRendererModule},
    {"geometryloaders", DeployCore::QtModule::Qt3DRendererModule},
    {"webview", DeployCore::QtModule::QtWebViewModule},
    {"xcbglintegrations", DeployCore::QtModule::QtGuiModule},
    {"wayland-decoration-client", DeployCore::QtModule::QtQuickModule},
    {"wayland-graphics-integration-client", DeployCore::QtModule::QtQuickModule},
    {"wayland-graphics-integration-server", DeployCore::QtModule::QtQuickModule},
    {"wayland-shell-integration", DeployCore::QtModule::QtGuiModule},


};


static const PluginModuleMapping ditalsSluginModuleMappings[] = {
    {"qtvirtualkeyboardplugin", DeployCore::QtModule::QtVirtualKeyboard}
};


static const PlatformMapping platformMappings[] =
{
    {"qminimal",                Unix | Win },
    {"qminimalegl",             Unix | Win },
    {"qoffscreen",              Unix | Win},
    {"qandroid",                UnknownPlatform },
    {"qbsdfb",                  UnknownPlatform },
    {"qcocoa",                  UnknownPlatform },
    {"qdirect2d",               Win },
    {"qdirectfb",               UnknownPlatform },
    {"qeglfs",                  Unix_ARM },
    {"qhaiku",                  UnknownPlatform },
    {"qios",                    UnknownPlatform },
    {"qlinuxfb",                Unix_ARM },
    {"qmirclient",              Unix },
    {"qopenwf",                 Unix },
    {"qqnx",                    UnknownPlatform },
    {"qvnc",                    WebRemote },
    {"qwasm",                   UnknownPlatform },
    {"qwindows",                Win },
    {"qwinrt",                  Win },
    {"qxcb",                    Unix },
    {"webgl",                   WebGl },
    {"qwayland-xcomposite-glx", Unix_x86_64},
    {"qwayland-xcomposite-egl", Unix_x86_64},
    {"qwayland-generic",        Unix_x86_64},
    {"qwayland-egl",            Unix_x86_64}

};

quint64 PluginsParser::qtModuleForPluginGroup(const QString &subDirName) const {
    const auto end = std::end(pluginModuleMappings);

    const auto result =
            std::find_if(std::begin(pluginModuleMappings), end,
                         [&subDirName] (const PluginModuleMapping &m) {

        return subDirName == QLatin1String(m.directoryName);
    });

    return result != end ? result->module : 0; // "designer"
}

quint64 PluginsParser::qtModuleForPlugin(const QString &plugin) const {
    const auto end = std::end(ditalsSluginModuleMappings);

    const auto result =
            std::find_if(std::begin(ditalsSluginModuleMappings), end,
                         [&plugin] (const PluginModuleMapping &m) {

        return plugin == QLatin1String(m.directoryName);
    });

    return result != end ? result->module : 0; // "designer"
}

Platform PluginsParser::platformForPlugin(const QString &name) const {
    const auto end = std::end(platformMappings);
    const auto result =
            std::find_if(std::begin(platformMappings), end,
                         [&name] (const PlatformMapping &m) {

        return name == QLatin1String(m._pluginName);
    });

    return (result != end) ? result->_platform : Platform::UnknownPlatform; // "designer"
}

QString PluginsParser::getPluginNameFromFile(const QString &baseNaem) const {

    if (baseNaem.left(3) == "lib") {
        return baseNaem.right(baseNaem.size() - 3);
    }

    return baseNaem;
}

bool PluginsParser::scan(const QString& pluginPath,
                         QStringList &resDependencies,
                         DeployCore::QtModule qtModules,
                         const QString& package) {

    auto plugins = QDir(pluginPath).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const auto &plugin: plugins) {
        scanPluginGroup(plugin, resDependencies, package, qtModules);
    }

    return true;
}

void PluginsParser::addPlugins(const QStringList& list, const QString& package,
                               QHash<QString, QSet<QString>>& container) {
    const DeployConfig* cnf = DeployCore::_config;

    for (const auto plugin: list) {
        if (QFileInfo(cnf->qtDir.getPlugins() + "/" + plugin).isDir()) {
             auto listPlugins = QDir(cnf->qtDir.getPlugins() + "/" + plugin).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

             for (const auto &plugin: listPlugins) {
                 container[package].insert(getPluginNameFromFile( plugin.baseName()));
             }

        } else {
            container[package].insert(getPluginNameFromFile(plugin));
        }
    }
}

bool PluginsParser::initDeployPluginsList() {
    const DeployConfig* cnf = DeployCore::_config;
    for (auto package = cnf->packages().cbegin(); package != cnf->packages().cend(); ++package) {

        auto distro = cnf->getDistroFromPackage(package.key());

        QList<QString> desabledFromPlatform;
        scanPlatforms(package.key(), desabledFromPlatform);

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        auto enablePlugins = distro.enabledPlugins().toList();
        auto disablePlugins = distro.disabledPlugins().toList();
#else
        auto enablePlugins =  QStringList(distro.enabledPlugins().cbegin(), distro.enabledPlugins().cend());
        auto disablePlugins = QStringList(distro.disabledPlugins().cbegin(), distro.disabledPlugins().cend());
#endif

        auto forbidenPlugins = defaultForbidenPlugins() + disablePlugins + desabledFromPlatform;

        addPlugins(forbidenPlugins, package.key(), _disabledPlugins);
        addPlugins(enablePlugins, package.key(), _enabledPlugins);
    }

    return true;

}

void PluginsParser::scanPlatforms(const QString& package, QList<QString>& disabledPlugins) {
    const DeployConfig* cnf = DeployCore::_config;
    auto platform = cnf->getPlatform(package);

    QString platformPluginPath = cnf->qtDir.getPlugins() + "/platforms";
    auto plugins = QDir(platformPluginPath).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const auto &plugin: plugins) {

        auto pluginPlatform = platformForPlugin(getPluginNameFromFile(plugin.baseName()));
        if (!(platform & pluginPlatform)) {

            QuasarAppUtils::Params::log("platform : " + plugin.baseName() + " is disabled", QuasarAppUtils::Info);
            disabledPlugins += getPluginNameFromFile(plugin.baseName());
        }
    }
}

void PluginsParser::scanPluginGroup(const QFileInfo& plugin,
                                    QStringList &result,
                                    const QString &package,
                                    DeployCore::QtModule qtModules) const {

    auto plugins = QDir(plugin.absoluteFilePath()).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    auto groupModule = qtModuleForPluginGroup(plugin.fileName());

    for (const auto& info: plugins) {
        bool fEnabled = isEnabledPlugin(getPluginNameFromFile(info.baseName()), package);
        bool fDisabled = isDisabledPlugin(getPluginNameFromFile(info.baseName()), package);

        auto pluginModule = qtModuleForPlugin(DeployCore::getLibCoreName(info));
        auto module = (pluginModule)? pluginModule : groupModule;

        if (fEnabled || (!fDisabled && (qtModules & module))) {
            result += info.absoluteFilePath();
        }
    }
}

bool PluginsParser::isDisabledPlugin(const QString &plugin, const QString &package) const {
    return _disabledPlugins[package].contains(plugin);
}

bool PluginsParser::isEnabledPlugin(const QString &plugin, const QString &package) const {
    return _enabledPlugins[package].contains(plugin);
}

QStringList PluginsParser::defaultForbidenPlugins() {
    return {
    };
}


