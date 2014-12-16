del *.sln
del *.lnk

cd Binaries/
del *.exe
del *.ilk
del *.dll
@rmdir /s /q Libraries
@rmdir /s /q Shaders
cd ../

@rmdir /s /q CMake
@rmdir /s /q Build
