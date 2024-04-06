#include "QtClass.h"
#include <QDir>
#include <qdebug.h>
#include <QSettings>
#include <QVersionNumber>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <QJsonArray>
#include <QTextCodec>
#include <QProcess>

static QString windowsProgramFilesDir()
{
#ifdef Q_OS_WIN64
    const char programFilesC[] = "ProgramFiles(x86)";
#else
    const char programFilesC[] = "ProgramFiles";
#endif
    return QDir::fromNativeSeparators(QFile::decodeName(qgetenv(programFilesC)));
}

static QString fixRegistryPath(const QString& path)
{
    QString result = QDir::fromNativeSeparators(path);
    if (result.endsWith(QLatin1Char('/')))
        result.chop(1);
    return result;
}

static std::optional<VisualStudioInstallation> installationFromPathAndVersion(
    const QString& installationPath, const QVersionNumber& version)
{
    QString vcVarsPath = QDir::fromNativeSeparators(installationPath);
    if (!vcVarsPath.endsWith('/'))
        vcVarsPath += '/';
    if (version.majorVersion() > 14)
        vcVarsPath += QStringLiteral("VC/Auxiliary/Build");
    else
        vcVarsPath += QStringLiteral("VC");

    const QString vcVarsAllPath = vcVarsPath + QStringLiteral("/vcvarsall.bat");
    if (!QFileInfo(vcVarsAllPath).isFile()) {
        qWarning().noquote() << "Unable to find MSVC setup script "
            << QDir::toNativeSeparators(vcVarsPath) << " in version " << version;
        return std::nullopt;
    }

    const QString versionString = version.toString();
    VisualStudioInstallation installation;
    installation.path = installationPath;
    installation.version = version.toString();
    installation.vsName = versionString;
    installation.vcVarsPath = vcVarsPath;
    installation.vcVarsAll = vcVarsAllPath;
    installation.vcDevenv = installationPath + "/Common7/IDE/devenv.exe";
    return installation;
}

// Detect build tools introduced with MSVC2017
static std::optional<VisualStudioInstallation> detectCppBuildTools2017()
{
    const QString installPath = windowsProgramFilesDir()
        + "/Microsoft Visual Studio/2017/BuildTools";
    const QString vcVarsPath = installPath + "/VC/Auxiliary/Build";
    const QString vcVarsAllPath = vcVarsPath + "/vcvarsall.bat";

    if (!QFileInfo::exists(vcVarsAllPath))
        return std::nullopt;

    VisualStudioInstallation installation;
    installation.path = installPath;
    installation.vcVarsAll = vcVarsAllPath;
    installation.vcVarsPath = vcVarsPath;
    installation.version = QVersionNumber(15).toString();
    installation.vsName = "15.0";

    return installation;
}

QVector<VisualStudioInstallation> detectVisualStudioFromRegistry()
{
    QVector<VisualStudioInstallation> result;
#ifdef Q_OS_WIN64
    const QString keyRoot = QStringLiteral(
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\");
#else
    const QString keyRoot = QStringLiteral(
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\SxS\\");
#endif
    QSettings vsRegistry(keyRoot + QStringLiteral("VS7"), QSettings::NativeFormat);
    QScopedPointer<QSettings> vcRegistry;
    foreach(const QString & vsName, vsRegistry.allKeys()) {
        const QVersionNumber version = QVersionNumber::fromString(vsName);
        if (!version.isNull()) {
            const QString installationPath = fixRegistryPath(vsRegistry.value(vsName).toString());

            std::optional<VisualStudioInstallation> installation = installationFromPathAndVersion(installationPath, version);
            if (installation)
                result.append(*installation);
        }
    }

    // Detect VS 2017 Build Tools
    auto installation = detectCppBuildTools2017();
    if (installation)
        result.append(*installation);

    return result;
}

static QVector<VisualStudioInstallation> detectVisualStudioFromVsWhere(const QString& vswhere)
{
    QVector<VisualStudioInstallation> installations;
    QProcess vsWhereProcess;
    vsWhereProcess.start(vswhere, { "-products", "*", "-prerelease", "-legacy", "-format", "json", "-utf8" });
    vsWhereProcess.waitForStarted();
    vsWhereProcess.waitForFinished();
    QByteArray output = vsWhereProcess.readAllStandardOutput();

    //QtcProcess vsWhereProcess;
    //vsWhereProcess.setCodec(QTextCodec::codecForName("UTF-8"));
    //const int timeoutS = 5;
    //vsWhereProcess.setTimeoutS(timeoutS);
    //vsWhereProcess.setCommand({ vswhere,
    //                    {"-products", "*", "-prerelease", "-legacy", "-format", "json", "-utf8"} });
    //vsWhereProcess.runBlocking();
    //switch (vsWhereProcess.result()) {
    //case QtcProcess::FinishedWithSuccess:
    //    break;
    //case QtcProcess::StartFailed:
    //    qWarning().noquote() << QDir::toNativeSeparators(vswhere) << "could not be started.";
    //    return installations;
    //case QtcProcess::FinishedWithError:
    //    qWarning().noquote().nospace() << QDir::toNativeSeparators(vswhere)
    //        << " finished with exit code "
    //        << vsWhereProcess.exitCode() << ".";
    //    return installations;
    //case QtcProcess::TerminatedAbnormally:
    //    qWarning().noquote().nospace()
    //        << QDir::toNativeSeparators(vswhere) << " crashed. Exit code: " << vsWhereProcess.exitCode();
    //    return installations;
    //case QtcProcess::Hang:
    //    qWarning().noquote() << QDir::toNativeSeparators(vswhere) << "did not finish in" << timeoutS
    //        << "seconds.";
    //    return installations;
    //}

    //QByteArray output = vsWhereProcess.stdOut().toUtf8();
    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(output, &error);
    if (error.error != QJsonParseError::NoError || doc.isNull()) {
        qWarning() << "Could not parse json document from vswhere output.";
        return installations;
    }

    const QJsonArray versions = doc.array();
    if (versions.isEmpty()) {
        qWarning() << "Could not detect any versions from vswhere output.";
        return installations;
    }

    for (const QJsonValue& vsVersion : versions) {
        const QJsonObject vsVersionObj = vsVersion.toObject();
        if (vsVersionObj.isEmpty()) {
            qWarning() << "Could not obtain object from vswhere version";
            continue;
        }

        QJsonValue value = vsVersionObj.value("installationVersion");
        if (value.isUndefined()) {
            qWarning() << "Could not obtain VS version from json output";
            continue;
        }
        const QString versionString = value.toString();
        QVersionNumber version = QVersionNumber::fromString(versionString);
        value = vsVersionObj.value("installationPath");
        if (value.isUndefined()) {
            qWarning() << "Could not obtain VS installation path from json output";
            continue;
        }
        const QString installationPath = value.toString();
        std::optional<VisualStudioInstallation> installation
            = installationFromPathAndVersion(installationPath, version);

        if (installation)
            installations.append(*installation);
    }
    return installations;
}

 QVector<VisualStudioInstallation> detectVisualStudio()
{
    const QString vswhere = windowsProgramFilesDir()
        + "/Microsoft Visual Studio/Installer/vswhere.exe";
    if (QFileInfo::exists(vswhere)) {
        const QVector<VisualStudioInstallation> installations = detectVisualStudioFromVsWhere(
            vswhere);
        if (!installations.isEmpty())
            return installations;
    }

    return detectVisualStudioFromRegistry();
}