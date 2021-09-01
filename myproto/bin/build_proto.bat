@echo off

pushd %~dp0..

set outDir=%cd%\include
set protoDir=%cd%\proto
set protocExe=%cd%\bin\protoc.exe
set protoc_gen_rpc=%cd%\bin\protoc_gen_rpc.exe

for /R %protoDir% %%i in (*.proto) do ( 
    echo %%i 
    %protocExe%  --plugin=protoc-gen-rpc=%protoc_gen_rpc%  --cpp_out=%outDir%  --rpc_out=%outDir%  -I%protoDir% %%i
)

popd
