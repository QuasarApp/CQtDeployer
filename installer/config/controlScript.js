function Controller()
{
    generateTr();

    installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
    installer.uninstallationFinished.connect(this, Controller.prototype.uninstallationFinished);
    installer.installationFinished.connect(this, Controller.prototype.installationFinished);
}

function generateTr() {
    console.log("generate tr start ")

    installer.setValue("Name", qsTr("CQtDeployer"));
    installer.setValue("Title", qsTr("Install CQtDeployer"));
}

Controller.prototype.uninstallationFinished = function()
{
    homeDir = installer.value("HomeDir", "");

    console.log("hometDir "  + homeDir)

    if (systemInfo.kernelType === "linux") {
        installer.execute("rm", ["-f", homeDir + "/.local/bin/cqtdeployer"])
    }

}


Controller.prototype.installationFinished = function()
{

    targetDir = installer.value("TargetDir", "");
    homeDir = installer.value("HomeDir", "");

    console.log("targetDir "  + targetDir)
    console.log("hometDir "  + homeDir)

    if (systemInfo.kernelType === "linux") {
        installer.execute("ln", ["-sf", targetDir + "/cqtdeployer.sh",
                                 homeDir + "/.local/bin/cqtdeployer"])
    }

    QMessageBox["information"](qsTr("install in system"), qsTr("Installer"),
        qsTr("CQtDeployer successfully installed on your computer to use the call \"cqtdeployer\"."),
             QMessageBox.Ok);
}
