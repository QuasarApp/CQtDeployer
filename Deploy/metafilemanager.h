#ifndef METAFILEMANAGER_H
#define METAFILEMANAGER_H

#include <QString>

class FileManager;

class MetaFileManager
{

private:
    bool createRunScriptWindows(const QString &target);
    bool createRunScriptLinux(const QString &target);


    bool createRunScript(const QString &target);
    bool createQConf();

    FileManager* _fileManager = nullptr;

public:
    MetaFileManager(FileManager* manager);

    void createRunMetaFiles();
};

#endif // METAFILEMANAGER_H
