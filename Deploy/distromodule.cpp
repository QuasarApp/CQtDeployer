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

QString DistroModule::name() const
{
    return _name;
}

void DistroModule::setName(const QString &name)
{
    _name = name;
}

QString DistroModule::description() const
{
    return _description;
}

void DistroModule::setDescription(const QString &description)
{
    _description = description;
}

QString DistroModule::version() const
{
    return _version;
}

void DistroModule::setVersion(const QString &version)
{
    _version = version;
}

QString DistroModule::releaseData() const
{
    return _releaseData;
}

void DistroModule::setReleaseData(const QString &releaseData)
{
    _releaseData = releaseData;
}

QString DistroModule::icon() const
{
    return _icon;
}

void DistroModule::setIcon(const QString &icon)
{
    _icon = icon;
}

QString DistroModule::publisher() const
{
    return _publisher;
}

void DistroModule::setPublisher(const QString &publisher)
{
    _publisher = publisher;
}

QSet<QString> DistroModule::enabled() const
{
    return _enabled;
}

void DistroModule::setEnabled(const QSet<QString> &enabled)
{
    _enabled = enabled;
}

void DistroModule::addEnabled(const QString &enabled) {
    _enabled += enabled;
}

QSet<QString> DistroModule::disabled() const
{
    return _disabled;
}

void DistroModule::setDisabled(const QSet<QString> &disabled)
{
    _disabled = disabled;
}

void DistroModule::addDisabled(const QString &disabled) {
    _disabled += disabled;
}

QSet<QString> DistroModule::extraPlugins() const
{
    return _extraPlugins;
}

void DistroModule::setExtraPlugins(const QSet<QString> &extraPlugins)
{
    _extraPlugins = extraPlugins;
}

void DistroModule::addExtraPlugins(const QString &extraPlugin) {
    _extraPlugins += extraPlugin;

}
