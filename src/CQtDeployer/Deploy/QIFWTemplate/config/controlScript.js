function Controller()
{
    generateTr();

    // CQtDeployer is a console utility: on the macOS hosts it is
    // installed into the /opt directory, the same pattern as on Linux
    // (the deb package installs into /opt). The wizard default (the
    // @HomeDir@/... of the config.xml template) is retargeted into
    // /opt, an explicit --targetdir of the headless mode is kept
    // (issue #884).
    if (systemInfo.kernelType === "osx") {
        const targetDir = installer.value("TargetDir", "");
        const homeDir = installer.value("HomeDir", "");
        const prefix = homeDir + "/";
        if (targetDir.indexOf(prefix) === 0) {
            installer.setValue("TargetDir", "/opt/" + targetDir.substring(prefix.length));
        }
    }

    installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
    installer.uninstallationFinished.connect(this, Controller.prototype.uninstallationFinished);
    installer.installationFinished.connect(this, Controller.prototype.installationFinished);

}

Controller.prototype.ComponentSelectionPageCallback = function()
{
    var widget = gui.currentPageWidget(); // get the current wizard page
    if (widget !== null && installer.isInstaller()) {
        widget.deselectAll();
        widget.selectComponent('CQtDeployer.1_6');
        widget.selectComponent('QIF');

    }
}

function generateTr() {
    console.log("generate tr start ")

    installer.setValue("Name", qsTr("CQtDeployer"));
    installer.setValue("Title", qsTr("Install CQtDeployer"));
}

Controller.prototype.uninstallationFinished = function()
{

}


Controller.prototype.installationFinished = function()
{

    if (systemInfo.kernelType === "winnt") {

        QMessageBox["information"](qsTr("install in system"), qsTr("Installer"),
            qsTr("CQtDeployer successfully installed on your computer to use the call \"cqtdeployer\"."),
                 QMessageBox.Ok);

    } else {

        QMessageBox["information"](qsTr("install in system"), qsTr("Installer"),
            qsTr("CQtDeployer successfully installed on your computer to use the call \"cqtdeployer\"."),
                 QMessageBox.Ok);
    }


}
