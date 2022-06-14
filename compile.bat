@echo off

echo "Compiling the Windows version of the library"
cl.exe /c /Ithirdparty/sqlite/ /Fo"build/windows/sqlite3.obj" thirdparty/sqlite/sqlite3.c
cl.exe /LD /DBUILD_LIB /std:c++17 /EHsc /Fo"build/windows/" /Fe"bin/windows/local_sqlite_database_w.dll" thirdparty\popstationmdg\src\plugins\export.cpp src\local_sqlite_database.cpp build/windows/sqlite3.obj /Iinclude /Ithirdparty\popstationmdg\src\plugins\ /Ithirdparty/sqlite/