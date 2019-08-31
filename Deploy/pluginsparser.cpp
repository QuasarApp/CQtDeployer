#include "pluginsparser.h"
#include <QDir>
#include <dependenciesscanner.h>

PluginsParser::PluginsParser(DependenciesScanner* scaner)
{
    assert(scaner);

    _libScaner = scaner;
}

static const PluginModuleMapping pluginModuleMappings[] =
{
    {"qml1tooling", DeployUtils::QtModule::QtDeclarativeModule},
    {"gamepads", DeployUtils::QtModule::QtGamePadModule},
    {"accessible", DeployUtils::QtModule::QtGuiModule},
    {"iconengines", DeployUtils::QtModule::QtGuiModule},
    {"imageformats", DeployUtils::QtModule::QtGuiModule},
    {"platforms", DeployUtils::QtModule::QtGuiModule},
    {"platforminputcontexts", DeployUtils::QtModule::QtGuiModule},
    {"virtualkeyboard", DeployUtils::QtModule::QtGuiModule},
    {"geoservices", DeployUtils::QtModule::QtLocationModule},
    {"audio", DeployUtils::QtModule::QtMultimediaModule},
    {"mediaservice", DeployUtils::QtModule::QtMultimediaModule},
    {"playlistformats", DeployUtils::QtModule::QtMultimediaModule},
    {"bearer", DeployUtils::QtModule::QtNetworkModule},
    {"position", DeployUtils::QtModule::QtPositioningModule},
    {"printsupport", DeployUtils::QtModule::QtPrintSupportModule},
    {"scenegraph", DeployUtils::QtModule::QtQuickModule},
    {"qmltooling", DeployUtils::QtModule::QtQuickModule | DeployUtils::QtModule::QtQmlToolingModule},
    {"sensors", DeployUtils::QtModule::QtSensorsModule},
    {"sensorgestures", DeployUtils::QtModule::QtSensorsModule},
    {"canbus", DeployUtils::QtModule::QtSerialBusModule},
    {"sqldrivers", DeployUtils::QtModule::QtSqlModule},
    {"texttospeech", DeployUtils::QtModule::QtTextToSpeechModule},
    {"qtwebengine", DeployUtils::QtModule::QtWebEngineModule | DeployUtils::QtModule::QtWebEngineCoreModule | DeployUtils::QtModule::QtWebEngineWidgetsModule},
    {"styles", DeployUtils::QtModule::QtWidgetsModule},
    {"sceneparsers", DeployUtils::QtModule::Qt3DRendererModule},
    {"renderplugins", DeployUtils::QtModule::Qt3DRendererModule},
    {"geometryloaders", DeployUtils::QtModule::Qt3DRendererModule},
    {"webview", DeployUtils::QtModule::QtWebViewModule}
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
                         QStringList &resDependencies) {

    auto modules = _libScaner->getQtModules();
    auto plugins = QDir(pluginPath).entryInfoList(QDir::Dirs);

    for (auto &&plugin: plugins) {
        if (modules & qtModuleForPlugin(plugin.fileName())) {
            resDependencies.append(plugin.absoluteFilePath());
        }
    }

    return true;
}
