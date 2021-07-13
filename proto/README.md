
# protobuf
参考 [protobuf编译、安装和简单使用C++ (Windows+VS平台)](https://www.cnblogs.com/WindSun/p/12543066.html)  

* 下载  
[protobuf-cpp-3.15.4.zip](https://github.com/protocolbuffers/protobuf/releases/download/v3.15.4/protobuf-cpp-3.15.4.zip)  
[cmake-3.20.0-rc3-windows-x86_64.zip](https://github.com/Kitware/CMake/releases/download/v3.20.0-rc3/cmake-3.20.0-rc3-windows-x86_64.zip)  

* 使用cmake构建protobuf vs项目  
打开 CMake目录/bin/cmake-gui.exe，选好 protobuf-3.15.3\cmake 源目录 和 生成目录，设置protobuf_BUILD_SHARED_LIBS，然后Configure, Generate  

* protobuf vs项目  
打开 生成目录\protobuf.sln，编译libprotobuf、protoc、libprotoc，项目设置Debug或Release，在Debug目录下会生成。  
把libprotobufd.lib拷到lib库目录，protoc.exe拷到proto\bin\目录。  

* myproto1 里写好 .proto文件，语法参考protobuf官网  

* 自己项目处理  
运行build_proto.bat，会在proto\include\pb\目录生成文件  
把protobuf-3.15.3\src\google拷贝到proto\include\目录。  

* 模块.build.cs文件修改  
参考[UE4文档 模块](https://docs.unrealengine.com/zh-CN/ProductionPipelines/BuildTools/UnrealBuildTool/ModuleFiles/index.html)  
[第三方库](https://docs.unrealengine.com/zh-CN/ProductionPipelines/BuildTools/UnrealBuildTool/ThirdPartyLibraries/index.html)  
```
PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../../proto/include"));
```

