SETLOCAL
SET MY_PATH=%~dp0

adb root
adb wait-for-device
adb remount
adb wait-for-device

adb push %MY_PATH%svrapi_config.txt /data/misc/qvr/svrapi_config.txt

pause