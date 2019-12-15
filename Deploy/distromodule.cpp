#include "distromodule.h"

DistroModule::DistroModule() {

}

QSet<QString> DistroModule::targets() const {
    return _targets;
}

void DistroModule::setTargets(const QSet<QString> &targets) {
    _targets = targets;
}

void DistroModule::addTarget(const QString &target) {
    _targets.insert(target);
}

QSet<QString> DistroModule::qmlInput() const {
    return _qmlInput;
}

void DistroModule::setQmlInput(const QSet<QString> &qmlInput) {
    _qmlInput = qmlInput;
}

void DistroModule::addQmlInput(const QString &target) {
    _qmlInput.insert(target);
}
