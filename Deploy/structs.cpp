#include "structs.h"

bool LibInfo::operator ==(const LibInfo &other) {
    return platform == other.platform &&
            name == other.name;
}

QString LibInfo::fullPath() {
    return path + "/" + name;
}

bool LibInfo::isValid() const {
    return platform != Platform::UnknownPlatform &&
            name.size() && path.size();
}
