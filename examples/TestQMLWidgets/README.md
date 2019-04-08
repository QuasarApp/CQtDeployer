# This example for packing apps to snap 
 You need to build your project with your own qt version

```bash
    cd ~/
    snap install cqtdeployer
    git clone https://github.com/QuasarApp/Console-QtDeployer.git
    cd Console-QtDeployer/examples/TestQMLWidgets
    /path/to/your/qmake/bin/qmake TestQMLWidgets.pro
    make 
    cqtdeployer -bin ./build/TestQMLWidgets -qmake /path/to/your/qmake/bin/qmake -qmlDir ./ -targetDir ./distro
    snapcraft
    snap install cqtdeployerexample_0.1_amd64.snap --dangerous
    cqtdeployerexample
``` 
Done!


# Warning 
Snap install cqtdeployer --classic - not work because, install on classic mode not supported, use install application from [github](https://github.com/QuasarApp/Console-QtDeployer/releases)
