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
    console.log("create icons!!! on Windows begin");

    if (systemInfo.kernelType === "winnt") {


        component.addElevatedOperation("CreateShortcut",
                               "@TargetDir@/cqtdeployer.exe",
                               "C:\Windows\system32\cqtdeployer.lnk");

        component.addElevatedOperation("CreateShortcut",
                               "@TargetDir@/cqtdeployer.exe",
                               "C:\Windows\system32\cqtdeployer.lnk");
        console.log("create icons!!! on Windows end");

    }



}
