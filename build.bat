cls

cl.exe /EHsc ./Vishwam/Src/*.cpp ./Sandbox/Src/*.cpp
del *.obj

move *.exe ./Sandbox & move *.exp ./Vishwam & move *.lib ./Vishwam
