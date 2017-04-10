#!/bin/bash
cd VITA_DM/DM_IoTvity/iotivity-1.2.1/
scons
cd ../..
scons
cd ..
./m.bat
export LD_LIBRARY_PATH=VITA_DM/Executables/lib/:VITA_DM/DM_IoTvity/:VITA_DM/DM_IoTvity/iotivity-1.2.1/out/linux/x86_64/release
#./r.bat


