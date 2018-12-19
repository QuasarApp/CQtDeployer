set QMAKE=%1

SET BASE_DIR=%~dp0


IF NOT EXIST %QMAKE% (
    echo "Error! win build support only native qmake, use ./sharedBuid.bat Path/to/qmake.exe" 
    exit(1)
)

%QMAKE% %BASE_DIR%\CQtDeployer.pro
