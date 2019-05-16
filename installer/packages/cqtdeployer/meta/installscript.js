function Component()
{
    generateTr();
}

function generateTr() {
    component.setValue("DisplayName", qsTr("CQtDeployer"));
    component.setValue("Description", qsTr("This package contains CQtDeployer"));
}


Component.prototype.createOperations = function()
{
//    // call default implementation to actually install README.txt!
    component.createOperations();

    removeFromSystem();

    if (!installer.isUninstaller()) {
        addInSystem();
    }

}

function removeFromSystem() {
    homeDir = installer.value("HomeDir", "");
    console.log("hometDir "  + homeDir)

    if (systemInfo.kernelType === "winnt") {

        if (!component.addElevatedOperation("Execute", ["DELETE",
                                                   "C:\Windows\system32\cqtdeployer.exe"])) {

            QMessageBox["warning"](qsTr("install in system"), qsTr("Installer"),
                qsTr("To uninstall cqtdeployer on your system, you need administrator rights!. "), QMessageBox.Ok);

            return;
        }

    } else {
        if (!component.addElevatedOperation("Execute", ["rm", "-f", homeDir + "/.local/bin/cqtdeployer"])) {

            QMessageBox["warning"](qsTr("install in system"), qsTr("Installer"),
                qsTr("To uninstall cqtdeployer on your system, you need administrator rights!. "), QMessageBox.Ok);

        }
    }
}

function addInSystem() {
    targetDir = installer.value("TargetDir", "");
    homeDir = installer.value("HomeDir", "");

    console.log("targetDir "  + targetDir)
    console.log("hometDir "  + homeDir)

    if (systemInfo.kernelType === "winnt") {

        if (!component.addElevatedOperation("Execute", ["mklink",
                                                   targetDir + "\cqtdeployer.exe",
                                                   "C:\Windows\system32\cqtdeployer.exe"])) {

            QMessageBox["warning"](qsTr("install in system"), qsTr("Installer"),
                qsTr("To install cqtdeployer on your system, you need administrator rights!. ") +
                qsTr("The installation was successful, but cqtdeployer will not be available from the console. ") +
                qsTr("To take advantage of this program you will need to enter the full path to it. ") +
                qsTr("Example: ") + targetDir + "\cqtdeployer.exe", QMessageBox.Ok);
            return;

        }


    } else {

        component.addOperation("Execute", ["ln",
                                           "-sf",
                                           targetDir + "/cqtdeployer.sh",
                                           homeDir + "/.local/bin/cqtdeployer"])

    }

    QMessageBox["information"](qsTr("install in system"), qsTr("Installer"),
        qsTr("CQtDeployer successfully installed on your computer to use the call \"cqtdeployer\"."),
             QMessageBox.Ok);
}
