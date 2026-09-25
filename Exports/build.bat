cls
del *.obj

cl.exe /c /EHsc *.cpp imgui/*.cpp
rc.exe OGL.rc
link.exe /OUT:OGL.exe *.obj OGL.res user32.lib gdi32.lib

del *.obj
