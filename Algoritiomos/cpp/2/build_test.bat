@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >NUL 2>&1
cd /d "E:\TI\git\Algoritmos\Algoritiomos"
cl /EHsc /std:c++17 /Fe:"E:\TI\git\Algoritmos\Algoritiomos\C_c_plus_plus\cpp\2\1\test.exe" "E:\TI\git\Algoritmos\Algoritiomos\C_c_plus_plus\cpp\2\1\main.cpp" 2>&1
if %ERRORLEVEL% NEQ 0 exit /b 1
"E:\TI\git\Algoritmos\Algoritiomos\C_c_plus_plus\cpp\2\1\test.exe" < "E:\TI\git\Algoritmos\Algoritiomos\excercicios\2\1\Input.txt" 2>&1
del /q "E:\TI\git\Algoritmos\Algoritiomos\C_c_plus_plus\cpp\2\1\test.exe" 2>NUL
del /q "E:\TI\git\Algoritmos\Algoritiomos\C_c_plus_plus\cpp\2\1\main.obj" 2>NUL
