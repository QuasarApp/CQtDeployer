#ifndef QTMODULES_H
#define QTMODULES_H

#include <QStringList>

class DependenciesScanner;

struct PluginModuleMapping
{
    const char *directoryName;
    quint64 module;
};

class PluginsParser
{
private:
    DependenciesScanner *_libScaner = nullptr;

    quint64 qtModuleForPlugin(const QString &subDirName);
public:
    PluginsParser(DependenciesScanner *scaner);
    bool scan(const QString &pluginPath, QStringList& resDependencies);


};

#endif // QTMODULES_H
