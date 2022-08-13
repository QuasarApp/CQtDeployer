function Controller()
{
    generateTr();

    installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
    installer.uninstallationFinished.connect(this, Controller.prototype.uninstallationFinished);
    installer.installationFinished.connect(this, Controller.prototype.installationFinished);


}

Controller.prototype.ComponentSelectionPageCallback = function()
{
    var widget = gui.currentPageWidget(); // get the current wizard page
    if (widget !== null && installer.isInstaller()) {
        widget.deselectAll();
        widget.selectComponent('cqtdeployer.1_6');
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
            qsTr("CQtDeployer successfully installed on your computer to use the call \"cqtdeployer\", cqt or cqtdeployer.cqt."),
                 QMessageBox.Ok);
    }


}
