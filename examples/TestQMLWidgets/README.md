# This example for packing apps to snap 
 You need to build your project with your own qt version

1. cd ~/                                                           # go to home dir 
2. snap install cqtdeployer                                        # install cqtdeployer from snap store 
3. git clone https://github.com/QuasarApp/CQtDeployer.git   # clone this project
4. cd Console-QtDeployer/examples/TestQMLWidgets                   # go to folder with example
5. /path/to/your/qmake/bin/qmake TestQMLWidgets.pro                # run your qmake for this example 
6. make                                                            # build example
7. cqtdeployer -bin ./build/TestQMLWidgets -qmake /path/to/your/qmake/bin/qmake -qmlDir ./ -targetDir ./distro
                                                                    # run cqtdeployer for deploy example project
8. snapcraft                                                       # create snapcraft build
9. snap install cqtdeployerexample_0.1_amd64.snap --dangerous      # install this example 
10.cqtdeployerexample                                              # check 
Done!

