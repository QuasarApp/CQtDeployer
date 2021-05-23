function Component() {

}

function generateShortCutCmd(cmd) {

    var prefix = "$PREFIX";

    if (systemInfo.kernelType === "winnt") {

        console.log("create icons!!! on Windows");

        component.addOperation(
            "CreateShortcut",
            "@TargetDir@/" + prefix + "/" + cmd + ".bat",
            "@DesktopDir@/" + cmd + ".lnk",
            "iconPath=@TargetDir@/$ICON",
            "iconId=0");

    }


    if (systemInfo.kernelType === "linux") {
        console.log("create icons!!! on LINUX");
        component.addOperation("CreateDesktopEntry",
                               "@HomeDir@/.local/share/applications/" + cmd + ".desktop",
                               "Version=@Version@\n
                                Type=Application\n
                                Terminal=false\n
                                Exec=\"@TargetDir@/" + prefix + "/" + cmd + ".sh\"\n
                                Name=" + cmd + "\n
                                Icon=@TargetDir@/$ICON\n
                                Name[en_US]=" + cmd);

        console.log("create icons!!! on LINUX done");
    }
}

Component.prototype.createOperations = function() {
    // call default implementation to actually install README.txt!
    component.createOperations();

    var cmdArray = ["array", "of", "cmds"]; // will be changed in cqtdeployer

    cmdArray.forEach( function (item){
        generateShortCutCmd(item);
    });

}
