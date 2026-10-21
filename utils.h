#ifndef UTILS_H
#define UTILS_H

#include "constants.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QRegularExpression>

namespace Utils {

namespace detail {

[[nodiscard]] inline QString hostWithoutPort(const QString &host)
{
    const int colon = host.indexOf(':');
    return colon < 0 ? host : host.left(colon);
}

}

[[nodiscard]] inline bool isNumericHost(const QString &host)
{
    const QString name = detail::hostWithoutPort(host);
    for (const QChar c : name) {
        if ((c < '0' || c > '9') && c != '.')
            return false;
    }
    return !name.isEmpty();
}

[[nodiscard]] inline bool isIpv4Host(const QString &host)
{
    const QStringList parts = detail::hostWithoutPort(host).split('.');
    if (parts.size() != 4)
        return false;
    for (const QString &part : parts) {
        bool ok = false;
        const int value = part.toInt(&ok);
        if (!ok || value < 0 || value > 255 || part.size() > 3)
            return false;
    }
    return true;
}

[[nodiscard]] inline QString formatBytes(qint64 bytes)
{
    using namespace AppConstants;

    constexpr double ROUNDED_UNIT_LIMIT = 1023.995;

    if (bytes < BYTES_PER_KB)
        return QString("%1 B").arg(bytes);
    if (static_cast<double>(bytes) / BYTES_PER_KB < ROUNDED_UNIT_LIMIT)
        return QString("%1 KB").arg(static_cast<double>(bytes) / BYTES_PER_KB, 0, 'f', 2);
    if (static_cast<double>(bytes) / BYTES_PER_MB < ROUNDED_UNIT_LIMIT)
        return QString("%1 MB").arg(static_cast<double>(bytes) / BYTES_PER_MB, 0, 'f', 2);
    return QString("%1 GB").arg(static_cast<double>(bytes) / BYTES_PER_GB, 0, 'f', 2);
}

[[nodiscard]] inline bool isLaunchableProgram(const QString &fileName)
{
    static const char *const kExtensions[] = {
        "exe", "msi", "bat", "cmd", "com", "scr", "pif", "lnk", "reg", "dll", "cpl",
        "ps1", "psm1", "vbs", "vbe", "js", "jse", "wsf", "wsh", "hta", "jar",
        "sh", "bash", "run", "bin", "command", "desktop", "appimage", "app",
        "apk", "deb", "rpm", "pkg", "dmg", "py", "pl", "rb", "php"
    };
    const int dot = fileName.lastIndexOf('.');
    if (dot < 0)
        return false;
    const QString extension = fileName.mid(dot + 1).trimmed();
    for (const char *candidate : kExtensions) {
        if (extension.compare(QLatin1String(candidate), Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

[[nodiscard]] inline bool looksLikeUrl(const QString &input)
{
    if (input.contains(' '))
        return false;
    static const char *const kSchemes[] = {
        "http://", "https://", "file://", "ftp://", "ftps://", "about:", "view-source:",
        "data:", "blob:", "qrc:", "chrome://"
    };
    for (const char *scheme : kSchemes) {
        if (input.startsWith(QLatin1String(scheme), Qt::CaseInsensitive))
            return true;
    }
    if (input.startsWith('[') && input.contains("]"))
        return true;
    QString host = input.left(input.indexOf(QRegularExpression("[/?#]")));
    if (host.endsWith('.'))
        host.chop(1);
    if (host.contains('.') && !host.split('.').contains(QString()))
        return !isNumericHost(host) || isIpv4Host(host);
    if (host.compare(QLatin1String("localhost"), Qt::CaseInsensitive) == 0
        || host.startsWith(QLatin1String("localhost:"), Qt::CaseInsensitive))
        return true;
    return false;
}

}

#endif // UTILS_H
