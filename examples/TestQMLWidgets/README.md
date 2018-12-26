# this example for packing apps to snap 

# you nead build your project with own qt version

```
    git clone https://github.com/QuasarApp/Console-QtDeployer.git
    Console-QtDeployer
    cd examples/TestQMLWidgets
    /path/to/your/qmake/bin/qmake TestQMLWidgets.pro
    make 
    cqtdeployer -bin ./build/TestQMLWidgets -qmake /path/to/your/qmake/bin/qmake -qmlDir ./ -targetDir ./distro
    snapcraft
    snap install cqtdeployerexample_0.1_amd64.snap --dangerous
    cqtdeployerexample
``` 
