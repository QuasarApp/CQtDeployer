/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "windependenciesscanner.h"
#include "deployutils.h"

#include <QList>
#include <QDir>
#include <QDebug>


struct Options {
    enum DebugDetection {
        DebugDetectionAuto,
        DebugDetectionForceDebug,
        DebugDetectionForceRelease
    };

    enum AngleDetection {
        AngleDetectionAuto,
        AngleDetectionForceOn,
        AngleDetectionForceOff
    };

    bool plugins = true;
    bool libraries = true;
    bool quickImports = true;
    bool translations = true;
    bool systemD3dCompiler = true;
    bool compilerRunTime = false;
    AngleDetection angleDetection = AngleDetectionAuto;
    bool softwareRasterizer = true;
    Platform platform = Windows;
    quint64 additionalLibraries = 0;
    quint64 disabledLibraries = 0;
    unsigned updateFileFlags = 0;
    QStringList qmlDirectories; // Project's QML files.
    QString directory;
    QString translationsDirectory; // Translations target directory
    QString libraryDirectory;
    QString pluginDirectory;
    QStringList binaries;
    JsonOutput *json = nullptr;
    ListOption list = ListNone;
    DebugDetection debugDetection = DebugDetectionAuto;
    bool deployPdb = false;
    bool dryRun = false;
    bool patchQt = true;

    inline bool isWinRt() const {
        return platform == WinRtArm || platform == WinRtIntel;
    }
};

WinDependenciesScanner::WinDependenciesScanner() {}

//QMap<QString, QString> WinDependenciesScanner::qMakeAll(QString *errorMessage, const QString& binary)
//{
//    QByteArray stdOut;
//    QByteArray stdErr;
//    unsigned long exitCode = 0;
//    if (!runProcess(binary, QStringList(QStringLiteral("-query")), QString(), &exitCode, &stdOut, &stdErr, errorMessage))
//        return QMap<QString, QString>();
//    if (exitCode) {
//        *errorMessage = binary + QStringLiteral(" returns ") + QString::number(exitCode)
//            + QStringLiteral(": ") + QString::fromLocal8Bit(stdErr);
//        return QMap<QString, QString>();
//    }
//    const QString output = QString::fromLocal8Bit(stdOut).trimmed().remove(QLatin1Char('\r'));
//    QMap<QString, QString> result;
//    const int size = output.size();
//    for (int pos = 0; pos < size; ) {
//        const int colonPos = output.indexOf(QLatin1Char(':'), pos);
//        if (colonPos < 0)
//            break;
//        int endPos = output.indexOf(QLatin1Char('\n'), colonPos + 1);
//        if (endPos < 0)
//            endPos = size;
//        const QString key = output.mid(pos, colonPos - pos);
//        const QString value = output.mid(colonPos + 1, endPos - colonPos - 1);
//        result.insert(key, value);
//        pos = endPos + 1;
//    }
//    QFile qconfigPriFile(result.value(QStringLiteral("QT_HOST_DATA")) + QStringLiteral("/mkspecs/qconfig.pri"));
//    if (qconfigPriFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        while (true) {
//            const QByteArray line = qconfigPriFile.readLine();
//            if (line.isEmpty())
//                break;
//            if (line.startsWith("QT_LIBINFIX")) {
//                const int pos = line.indexOf('=');
//                if (pos >= 0) {
//                    const QString infix = QString::fromUtf8(line.right(line.size() - pos - 1).trimmed());
//                    if (!infix.isEmpty())
//                        result.insert(QLatin1String(qmakeInfixKey), infix);
//                }
//                break;
//            }
//        }
//    } else {
//        std::wcerr << "Warning: Unable to read " << QDir::toNativeSeparators(qconfigPriFile.fileName())
//            << ": " << qconfigPriFile.errorString()<< '\n';
//    }
//    return result;
//}

bool WinDependenciesScanner::fillLibInfo(LibInfo &info, const QString &file) {

}

void WinDependenciesScanner::setEnvironment(const QStringList &env) {
    QDir dir;
    for (auto i : env) {

        dir.setPath(i);
        if (!dir.exists()) {
            continue;
        }

        auto list = dir.entryInfoList(QStringList() << "*.dll",
                                  QDir::Files| QDir::NoDotAndDotDot);

        for (auto i : list) {

            auto newPriority = DeployUtils::getLibPriority(i.absoluteFilePath());
            auto oldPriority = DeployUtils::getLibPriority(_EnvLibs.value(i.fileName(), ""));

            if (newPriority > oldPriority)
                _EnvLibs.insert(i.fileName(), i.absoluteFilePath());
        }

    }

}

Platform WinDependenciesScanner::platformFromMkSpec(const QString &xSpec)
{
    if (xSpec == QLatin1String("linux-g++"))
        return Unix;
    if (xSpec.startsWith(QLatin1String("win32-")))
        return xSpec.contains(QLatin1String("g++")) ? WindowsMinGW : Windows;
    if (xSpec.startsWith(QLatin1String("winrt-x")))
        return WinRtIntel;
    if (xSpec.startsWith(QLatin1String("winrt-arm")))
        return WinRtArm;
    if (xSpec.startsWith(QLatin1String("wince"))) {
        if (xSpec.contains(QLatin1String("-x86-")))
            return WinCEIntel;
        if (xSpec.contains(QLatin1String("-arm")))
            return WinCEArm;
    }
    return UnknownPlatform;
}

QStringList WinDependenciesScanner::scan(const QString &path, Platform platfr,
                                         const QString& qmake) {
    QStringList result;

    QString errorMessage;

    if (platfr == Platform::UnknownPlatform) {

        const auto qmakeVariables = qMakeAll(&errorMessage, qmake);
        const QString xSpec = qmakeVariables.value(QStringLiteral("QMAKE_XSPEC"));
        platfr = platformFromMkSpec(xSpec);
    }

    QStringList dep;
    readExecutable(path, platfr, &errorMessage, &dep);

    if (!errorMessage.isEmpty()) {
        qCritical() << errorMessage;
        return result;
    }

    for (auto i : dep) {
        QString lib(i);
        if (_EnvLibs.count(lib)) {
            result.push_back(_EnvLibs.value(lib));
        }
    }

    return result;
}

WinDependenciesScanner::~WinDependenciesScanner() {

}

bool LibInfo::operator ==(const LibInfo &other) {
    return platform == other.platform &&
            name == other.name &&
            is32bit == other.is32bit;
}

QString LibInfo::fullPath() {
    return path + "/" + name;
}
