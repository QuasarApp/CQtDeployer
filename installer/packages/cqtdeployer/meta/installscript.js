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

    if (systemInfo.kernelType === "winnt") {

        console.log("create icons!!! on Windows");

        component.addElevatedOperation("CreateShortcut",
                               "@TargetDir@/cqtdeployer.exe",
                               "C:\Windows\system32\cqtdeployer");

        component.addElevatedOperation("CreateShortcut",
                               "@TargetDir@/cqtdeployer.exe",
                               "C:\Windows\system32\cqtdeployer");
    }



}
