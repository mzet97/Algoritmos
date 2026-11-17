@echo off
setlocal enabledelayedexpansion

call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >NUL 2>&1

set "BASE=E:\TI\git\Algoritmos\Algoritiomos"

for %%E in (3 4 5 10 17) do (
    echo === Exercise %%E ===
    cl /EHsc /std:c++17 /Fe:"%BASE%\C_c_plus_plus\cpp\2\%%E\test.exe" "%BASE%\C_c_plus_plus\cpp\2\%%E\main.cpp" >NUL 2>&1
    if !ERRORLEVEL! NEQ 0 (
        echo COMPILE FAILED
        cl /EHsc /std:c++17 /Fe:"%BASE%\C_c_plus_plus\cpp\2\%%E\test.exe" "%BASE%\C_c_plus_plus\cpp\2\%%E\main.cpp" 2>&1
    ) else (
        "%BASE%\C_c_plus_plus\cpp\2\%%E\test.exe" < "%BASE%\excercicios\2\%%E\Input.txt" > "%BASE%\C_c_plus_plus\cpp\2\%%E\output.txt" 2>&1
        echo Got:
        type "%BASE%\C_c_plus_plus\cpp\2\%%E\output.txt"
        echo Expected:
        type "%BASE%\excercicios\2\%%E\Solution.txt"
        del /q "%BASE%\C_c_plus_plus\cpp\2\%%E\test.exe" 2>NUL
        del /q "%BASE%\C_c_plus_plus\cpp\2\%%E\main.obj" 2>NUL
    )
    echo.
)
