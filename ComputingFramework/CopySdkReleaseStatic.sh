#!/bin/tcsh
mkdir -p ../SDK/Include
mkdir -p ../SDK/Lib
mkdir -p ../SDK/Lib/Release

set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir && $dir != ComputingFramework.xcodeproj && $dir != build) then
        echo $dir
        cd $dir
        set CURDIR = `pwd`
        set HEADERS = `ls *.h *.inl`
        foreach header (${HEADERS})
            cp -fp "${CURDIR}"/$header ../../SDK/Include
        end
        cd ..
    endif
end

set RHEADERS = `ls *.h *.inl`
foreach rheader (${RHEADERS})
    cp -fp "${SRCROOT}"/$rheader ../SDK/Include
end

cp -fp "${BUILT_PRODUCTS_DIR}"/libComputingFramework.a ../SDK/Lib/Release/libComputingFramework.a
ranlib ../SDK/Lib/Release/libComputingFramework.a
