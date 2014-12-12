del *.sln
del *.lnk

cd Binaries/
del *.exe
del *.ilk
@rmdir /s /q Libraries
cd ../

@rmdir /s /q CMake
@rmdir /s /q Build
