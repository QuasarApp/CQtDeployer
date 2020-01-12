function Component(){}

const extractFileName = (path) => {
    const fullName = path.substring(path.lastIndexOf('/'));
    return fullName.substring(0, fullName.lastIndexOf('.'));
}

const generateShortCutCmd = (cmd) => {
    if (systemInfo.kernelType === "winnt") {

        console.log("create icons!!! on Windows");
        component.addOperation("CreateShortcut",
                               "@TargetDir@/" + cmd,
                               "@DesktopDir@/" + extractFileName(cmd) + ".lnk");

    }


    if (systemInfo.kernelType === "linux") {
        console.log("create icons!!! on LINUX");
        const name = extractFileName(cmd);
        component.addOperation("CreateDesktopEntry",
                               "@HomeDir@/.local/share/applications/" + name + ".desktop",
                               "Version=@Version@\n
                                Type=Application\n
                                Terminal=false\n
                                Exec=@TargetDir@/" + cmd + "\n
                                Name=" + name + "\n
                                Icon=@TargetDir@/icons/icon.png\n // will be changed in cqtdeployer
                                Name[en_US]=" + name);

        console.log("create icons!!! on LINUX done");
    }
}

Component.prototype.createOperations = () => {
    // call default implementation to actually install README.txt!
    component.createOperations();

    const cmdArray = ["array", "of", "cmds"]; // will be changed in cqtdeployer

    cmdArray.foreach( function (item){
        generateShortCutCmd(item);
    });

}
