#include "envirement.h"

QStringList Envirement::deployEnvironment() const
{
    return _deployEnvironment;
}

void Envirement::setDeployEnvironment(const QStringList &deployEnvironment)
{
    _deployEnvironment = deployEnvironment;
}

QStringList Envirement::ignoreEnvList() const
{
    return _ignoreEnvList;
}

void Envirement::setIgnoreEnvList(const QStringList &ignoreEnvList)
{
    _ignoreEnvList = ignoreEnvList;
}

Envirement::Envirement()
{

}
