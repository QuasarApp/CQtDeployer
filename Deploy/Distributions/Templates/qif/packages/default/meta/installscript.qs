function Component() {

}

function extractFileName(path) {
    const fullName = path.substring(path.lastIndexOf('/') + 1);

    const index = fullName.lastIndexOf('.');
    if (index >= 0) {
        return fullName.substring(0, index)
    }

    return fullName;
}

function generateShortCutCmd(cmd) {
    if (systemInfo.kernelType === "winnt") {

        console.log("create icons!!! on Windows");

        component.addOperation(
            "CreateShortcut",
            "@TargetDir@/" + cmd + ".bat",
            "@DesktopDir@/" + extractFileName(cmd) + ".lnk",
            "iconPath=@TargetDir@/$ICON",
            "iconId=0");

    }


    if (systemInfo.kernelType === "linux") {
        console.log("create icons!!! on LINUX");
        const name = extractFileName(cmd);
        component.addOperation("CreateDesktopEntry",
                               "@HomeDir@/.local/share/applications/" + name + ".desktop",
                               "Version=@Version@\n
                                Type=Application\n
                                Terminal=false\n
                                Exec=\"@TargetDir@/" + cmd + ".sh\"\n
                                Name=" + name + "\n
                                Icon=@TargetDir@/$ICON\n
                                Name[en_US]=" + name);

        console.log("create icons!!! on LINUX done");
    }
}

Component.prototype.createOperations = function() {
    // call default implementation to actually install README.txt!
    component.createOperations();

    const cmdArray = ["array", "of", "cmds"]; // will be changed in cqtdeployer

    cmdArray.forEach( function (item){
        generateShortCutCmd(item);
    });

}
