#include "templateinfo.h"
#include "quasarapp.h"

QString TemplateInfo::InstallDeirQIFW() const {
    return QuasarAppUtils::Params::getArg("installDirQIFW", "@HomeDir@");
}
