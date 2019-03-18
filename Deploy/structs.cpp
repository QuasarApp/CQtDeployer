#include "structs.h"

bool LibInfo::operator ==(const LibInfo &other) {
    return platform == other.platform &&
            name == other.name;
}

QString LibInfo::fullPath() {
    return path + "/" + name;
}
