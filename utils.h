#ifndef UTILS_H
#define UTILS_H

#include "constants.h"

#include <QtCore/QString>

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
    if (input.contains('.') && !input.contains(' '))
        return true;
    if (input == "localhost" || input.startsWith("localhost:"))
        return true;
    return false;
}

}

#endif // UTILS_H
