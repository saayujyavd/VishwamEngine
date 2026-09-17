del *.obj

cl.exe /c /EHsc *.cpp
link.exe *.obj user32.lib

del *.obj
