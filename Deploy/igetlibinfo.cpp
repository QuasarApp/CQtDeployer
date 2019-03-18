#include "igetlibinfo.h"


IGetLibInfo::IGetLibInfo(){

}

LibInfo &&IGetLibInfo::getLibInfo(const QString &lib) {
    Q_UNUSED(lib);
    return std::move(LibInfo());
}

IGetLibInfo::~IGetLibInfo() {

}
