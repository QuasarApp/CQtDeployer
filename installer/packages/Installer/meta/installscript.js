// Constructor
function Component()
{
    generateTr();
}

function generateTr() {
    console.log("generate tr start ")

    component.setValue("DisplayName", qsTr("Installer"));
    component.setValue("Description", qsTr("This package contains information of installer"));
}

