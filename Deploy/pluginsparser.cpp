/*
 * Copyright (C) 2018-2020 QuasarApp.
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
    {"virtualkeyboard", DeployCore::QtModule::QtGuiModule},
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
    {"geometryloaders", DeployCore::QtModule::Qt3DRendererModule},
    {"webview", DeployCore::QtModule::QtWebViewModule},
    {"xcbglintegrations", DeployCore::QtModule::QtGuiModule},
    {"wayland-decoration-client", DeployCore::QtModule::QtGuiModule},
    {"wayland-graphics-integration-client", DeployCore::QtModule::QtGuiModule},
    {"wayland-graphics-integration-server", DeployCore::QtModule::QtGuiModule},
    {"wayland-shell-integration", DeployCore::QtModule::QtGuiModule},


};

static const PlatformMapping platformMappings[] =
{
    {"qminimal",                Unix | Win },
    {"qminimalegl",             Unix | Win },
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

quint64 PluginsParser::qtModuleForPlugin(const QString &subDirName) {
    const auto end = std::end(pluginModuleMappings);

    const auto result =
            std::find_if(std::begin(pluginModuleMappings), end,
                         [&subDirName] (const PluginModuleMapping &m) {

        return subDirName == QLatin1String(m.directoryName);
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

    QuasarAppUtils::Params::log("Modules Number :" + QString::number(qtModules), QuasarAppUtils::Info);

    for (const auto &plugin: plugins) {
        auto module = qtModuleForPlugin(plugin.fileName());
        if (qtModules & module) {
            scanPluginGroup(plugin.absoluteFilePath(), resDependencies, package);
        }
    }

    return true;
}

bool PluginsParser::initDeployPluginsList() {
    const DeployConfig* cnf = DeployCore::_config;
    for (auto package = cnf->packages().cbegin(); package != cnf->packages().cend(); ++package) {

        QList<QString> desabledFromPlatform;
        scanPlatforms(package.key(), desabledFromPlatform);

        auto enablePlugins = QuasarAppUtils::Params::getStrArg("enablePlugins");
        auto disablePlugins = QuasarAppUtils::Params::getStrArg("disablePlugins");

        auto forbidenPlugins = defaultForbidenPlugins() + disablePlugins.split(',') + desabledFromPlatform;

        for (const auto plugin: forbidenPlugins) {
            if (!enablePlugins.contains(plugin)) {
                if (QFileInfo(cnf->qtDir.getPlugins() + "/" + plugin).isDir()) {
                     auto listPlugins = QDir(cnf->qtDir.getPlugins() + "/" + plugin).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

                     for (const auto &plugin: listPlugins) {
                         QuasarAppUtils::Params::log("Disable plugin: " + plugin.baseName(), QuasarAppUtils::Debug);
                         _disabledPlugins[package.key()].insert(getPluginNameFromFile( plugin.baseName()));
                     }

                } else {
                    QuasarAppUtils::Params::log("Disable plugin: " + plugin, QuasarAppUtils::Debug);
                    _disabledPlugins[package.key()].insert(getPluginNameFromFile(plugin));
                }
            }
        }

    }

    return true;

}

void PluginsParser::scanPlatforms(const QString& package, QList<QString>& disabledPlugins) {
    const DeployConfig* cnf = DeployCore::_config;
    auto platform = cnf->getPlatform(package);

    QString platformPluginPath = cnf->qtDir.getPlugins() + "/platforms";
    auto plugins = QDir(platformPluginPath).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const auto &plugin: plugins) {
        QStringList ignoreList = {{".so.debug"}, {"d.dll"}, {".pdb"}};

        bool ignore = false;
        for (const auto& i: ignoreList) {
            if (plugin.fileName().contains(i, ONLY_WIN_CASE_INSENSIATIVE)) {
                ignore = true;
                break;
            }
        }

        if (ignore) {
            continue;
        }

        auto pluginPlatform = platformForPlugin(getPluginNameFromFile(plugin.baseName()));
        if (!(platform & pluginPlatform)) {

            QuasarAppUtils::Params::log("platform : " + plugin.baseName() + " is disabled", QuasarAppUtils::Info);
            disabledPlugins += getPluginNameFromFile(plugin.baseName());
        }
    }
}

void PluginsParser::scanPluginGroup(const QString& pluginFolder, QStringList &result, const QString &package) const {
    auto plugins = QDir(pluginFolder).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const auto& info: plugins) {
        if (!isDisavledPlugin(getPluginNameFromFile(info.baseName()), package)) {
            result += info.absoluteFilePath();
        }
    }
}

bool PluginsParser::isDisavledPlugin(const QString &plugin, const QString &package) const {
    return _disabledPlugins[package].contains(plugin);
}

QStringList PluginsParser::defaultForbidenPlugins() const {
    return {
        "qtvirtualkeyboardplugin",
        "virtualkeyboard",
    };
}


