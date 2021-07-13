@echo off

for /R %cd%\myproto1 %%i in (*.proto) do ( 
    echo %%i 
    .\bin\protoc.exe  --cpp_out=.\include\pb -I%cd%\myproto1\ %%i
)

