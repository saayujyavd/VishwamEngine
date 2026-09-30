cls
del *.obj

cl.exe /c /EHsc ./src/*.cpp ./src/imgui/*.cpp
rc.exe ./src/OGL.rc
link.exe /OUT:OGL.exe *.obj ./src/OGL.res user32.lib gdi32.lib

del *.obj
