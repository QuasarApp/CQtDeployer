# This example for packing apps to snap 
 You need to build your project with your own qt version

```bash
    cd ~/                                                           # go to home dir 
    snap install cqtdeployer                                        # install cqtdeployer from snap store 
    git clone https://github.com/QuasarApp/Console-QtDeployer.git   # clone this project
    cd Console-QtDeployer/examples/TestQMLWidgets                   # go to folder with example
    /path/to/your/qmake/bin/qmake TestQMLWidgets.pro                # run your qmake for this example 
    make                                                            # build example
    cqtdeployer -bin ./build/TestQMLWidgets -qmake /path/to/your/qmake/bin/qmake -qmlDir ./ -targetDir ./distro
                                                                    # run cqtdeployer for deploy example project
    snapcraft                                                       # create snapcraft build
    snap install cqtdeployerexample_0.1_amd64.snap --dangerous      # install this example 
    cqtdeployerexample                                              # check 
``` 
Done!


# Warning 
Snap install cqtdeployer --classic - not work because, install on classic mode not supported, use install application from [github](https://github.com/QuasarApp/Console-QtDeployer/releases)
