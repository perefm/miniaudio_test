rmdir miniaudio_test_vs2022 /s /q
mkdir miniaudio_test_vs2022
cd miniaudio_test_vs2022
cmake.exe -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=x64-windows -G "Visual Studio 17 2022" ..
cd..

pause