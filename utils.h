#ifndef UTILS_H
#define UTILS_H

#include "constants.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QRegularExpression>

namespace Utils {

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

[[nodiscard]] inline bool looksLikeUrl(const QString &input)
{
    if (input.contains(' '))
        return false;
    if (input.startsWith("http://") || input.startsWith("https://") || input.startsWith("file://"))
        return true;
    static const char *const kSchemes[] = {
        "ftp://", "ftps://", "about:", "view-source:", "data:", "blob:", "qrc:", "chrome://"
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
        return true;
    if (input == "localhost" || input.startsWith("localhost:"))
        return true;
    return false;
}

}

#endif // UTILS_H
