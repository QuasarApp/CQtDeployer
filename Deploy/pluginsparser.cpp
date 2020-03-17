/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

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

quint64 PluginsParser::qtModuleForPlugin(const QString &subDirName) {
    const auto end = std::end(pluginModuleMappings);

    const auto result =
        std::find_if(std::begin(pluginModuleMappings), end,
                     [&subDirName] (const PluginModuleMapping &m) {

        return subDirName == QLatin1String(m.directoryName);
    });

    return result != end ? result->module : 0; // "designer"
}

bool PluginsParser::scan(const QString& pluginPath,
                         QStringList &resDependencies,
                         DeployCore::QtModule qtModules) {

    auto plugins = QDir(pluginPath).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    QuasarAppUtils::Params::verboseLog("Modules Number :" + QString::number(qtModules), QuasarAppUtils::Info);

    for (const auto &plugin: plugins) {
        auto module = qtModuleForPlugin(plugin.fileName());
        if (qtModules & module) {

            QuasarAppUtils::Params::verboseLog("deploye plugin : " + plugin.absoluteFilePath(), QuasarAppUtils::Info);

            resDependencies.append(plugin.absoluteFilePath());
        }
    }

    return true;
}
