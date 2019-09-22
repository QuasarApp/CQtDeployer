#include "envirement.h"
#include "quasarapp.h"

#include <QDir>
#include <QFileInfo>

QStringList Envirement::deployEnvironment() const
{
    return _deployEnvironment.toList();
}

QStringList Envirement::ignoreEnvList() const
{
    return _ignoreEnvList.toList();
}

void Envirement::setIgnoreEnvList(const QStringList &ignoreEnvList)
{
    _ignoreEnvList = ignoreEnvList.toSet();
}

void Envirement::addEnv(const QString &dir, const QString &appDir, const QString& targetDir) {

    char separator = ':';

#ifdef Q_OS_WIN
    separator = ';';
#endif

    if (dir.contains(separator)) {
        auto list = dir.split(separator);
        for (auto i : list) {
            addEnv(i, appDir, targetDir);
        }
        return;
    }

    auto path = QFileInfo(dir).absoluteFilePath();

    for (QString i :_ignoreEnvList) {
        if (path.contains(i)) {
            return;
        }
    }

    if (path.contains(appDir)) {
        QuasarAppUtils::Params::verboseLog("is cqtdeployer dir!: " + path + " app dir : " + appDir);
        return;
    }

    if (!QFileInfo(path).isDir()) {
        QuasarAppUtils::Params::verboseLog("is not dir!! :" + path);
        return;
    }

    if (_deployEnvironment.contains(path)) {
        QuasarAppUtils::Params::verboseLog ("Environment alredy added: " + path);
        return;
    }

    if (path.contains(targetDir)) {
        QuasarAppUtils::Params::verboseLog ("Skip paths becouse it is target : " + path);
        return;
    }

    _deployEnvironment.insert(QDir::fromNativeSeparators(path));
}

bool Envirement::inThisEnvirement(const QString &file) const {
    //return _deployEnvironment.contains(file);
}

int Envirement::size() const {
    return _deployEnvironment.size();
}

QString Envirement::concatEnv() const {

    if (_deployEnvironment.isEmpty()) {
        return "";
    }

    QString result = *_deployEnvironment.begin();
    for (auto i: _deployEnvironment) {
#ifdef  Q_OS_UNIX
        result += (":" + i);
#else
        result += (";" + i);
#endif
    }

    return result;
}

Envirement::Envirement()
{

}
