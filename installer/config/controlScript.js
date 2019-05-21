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

    if (systemInfo.kernelType === "winnt") {
        if (!installer.gainAdminRights()) {
            QMessageBox["warning"](qsTr("install in system"), qsTr("Installer"),
                qsTr("To uninstall cqtdeployer on your system, you need administrator rights!. "), QMessageBox.Ok);

            return;
        }

        installer.execute("DELETE", ["C:\Windows\system32\cqtdeployer.exe"])

        installer.dropAdminRights();

    } else {
        installer.execute("rm", ["-f", homeDir + "/.local/bin/cqtdeployer"])
    }

}


Controller.prototype.installationFinished = function()
{

    targetDir = installer.value("TargetDir", "");
    homeDir = installer.value("HomeDir", "");

    console.log("targetDir "  + targetDir)
    console.log("hometDir "  + homeDir)

    if (systemInfo.kernelType === "winnt") {

        installer.execute("SETX", [ "cqtdeployer", targetDir + "\cqtdeployer.exe"])

        QMessageBox["information"](qsTr("install in system"), qsTr("Installer"),
            qsTr("CQtDeployer successfully installed on your computer to use the call \"%cqtdeployer%\"."),
                 QMessageBox.Ok);

    } else {

        if (!installer.fileExists(homeDir + "/.local/bin")) {
            installer.execute("mkpath", ["-p", homeDir + "/.local/bin"]);

            QMessageBox["warning"](qsTr("install in system"), qsTr("Installer"),
                qsTr("The \"~/local/bin\" folder was not initialized, you may need to reboot to work correctly!"),
                                   QMessageBox.Ok);

        }

        installer.execute("ln", ["-sf", targetDir + "/cqtdeployer.sh",
                                 homeDir + "/.local/bin/cqtdeployer"])

        QMessageBox["information"](qsTr("install in system"), qsTr("Installer"),
            qsTr("CQtDeployer successfully installed on your computer to use the call \"cqtdeployer\"."),
                 QMessageBox.Ok);
    }


}
