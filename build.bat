cls
cd ./Vishwam/
del *.obj

cl.exe /c /EHsc ./src/*.cpp
rc.exe ./src/Icon.rc

cd ./src/
move ./Icon.res ../rsc/
cd..

link.exe /OUT:Vishwam.exe ./*.obj ./rsc/Icon.res user32.lib
del *.obj

move ./Vishwam.exe ../
cd..
