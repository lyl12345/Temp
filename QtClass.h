#pragma once
#include <QString>
#include <QVector>

struct VisualStudioInstallation
{
    QString vsName;
    QString version;
    QString path;       // Main installation path
    QString vcVarsPath; // Path under which the various vc..bat are to be found
    QString vcVarsAll;
    QString vcDevenv;
};

QVector<VisualStudioInstallation> detectVisualStudioFromRegistry();
QVector<VisualStudioInstallation> detectVisualStudio();