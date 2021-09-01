[TOC]

<br />

# 1 概述
个人UE4客户端demo1.  
[服务端链接]()  

## 1.1 版本
VS 2019,  
UE4 4.25.4,  


<br />

# 2 编译  
* 生成VS项目  
修改GenerateProjectFiles.bat中UnrealBuildToolPath路径，然后执行。  
* 打开ThirdPersonMP.sln，选择平台Win64，编译。  
* 打开ThirdPersonMP.uproject。  
如果打开有问题，可以下载Epic Games打开。  


<br />

# 3 Network
## 3.1 Asio (弃用)  
[Standalone Asio C++ library Standalone Asio](http://think-async.com/Asio/AsioStandalone)  
[asio-1.18.1.zip 下载](https://udomain.dl.sourceforge.net/project/asio/asio/1.18.1%20%28Stable%29/asio-1.18.1.zip)  
```
1. 把 asio-1.18.1/include 目录拷到 ThirdParty/Asio  
2. 在 projectName.Build.cs 中添加：  
    PublicDefinitions.Add("ASIO_STANDALONE");  
    PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../../ThirdParty/Asio/include"));  
```


<br />

# 4 protobuf
参考 [protobuf编译、安装和简单使用C++ (Windows+VS平台)](https://www.cnblogs.com/WindSun/p/12543066.html)  

## 4.1 步骤
* 下载  
[protobuf-cpp-3.15.4.zip](https://github.com/protocolbuffers/protobuf/releases/download/v3.15.4/protobuf-cpp-3.15.4.zip)  
[cmake-3.20.0-rc3-windows-x86_64.zip](https://github.com/Kitware/CMake/releases/download/v3.20.0-rc3/cmake-3.20.0-rc3-windows-x86_64.zip)  
把protobuf-3.15.3\src\google拷贝到proto\include\头文件目录。  

* 使用cmake构建protobuf vs项目  
打开 CMake目录/bin/cmake-gui.exe，选好 protobuf-3.15.3\cmake 源目录 和 生成目录，设置protobuf_BUILD_SHARED_LIBS，然后Configure, Generate  

* protobuf vs项目  
打开 生成目录\protobuf.sln，编译libprotobuf、protoc、libprotoc，项目设置Release，编译。  
libprotoc.lib和libprotobuf.lib拷到lib目录下，protoc.exe、libprotoc.dll和libprotobuf.dll拷到myproto/bin目录下。  
```
编译前要修改google\protobuf\compiler\cpp\cpp_helpers.h里的 inline std::string ClassName(const Descriptor* descriptor, bool qualified)，在这一行最前面加上PROTOC_EXPORT。    
    因为myproto\proto\protoc_gen_rpc\里要用到libprotoc库里的全局函数，链接静态库编不过。  
    链接动态库，函数ClassName需要加上PROTOC_EXPORT表示导出。  
```

* myproto/proto 里写好 .proto文件。  

* myproto/myprotolib.sln  
这里面有两个项目，myprotolib是proto文件打包成静态库，protoc_gen_rpc是定制protobuf插件。  
打开编译protoc_gen_rpc项目，会在myproto/bin目录生成myproto/bin/protoc_gen_rpc.exe 。  
运行myproto/bin/build_proto.bat，会在myproto/include目录生成pb文件。  
在myproto项目中添加生成的pb文件，编译，在lib目录生成myprotolib.lib。  

* 模块.build.cs文件修改  
参考[UE4文档 模块](https://docs.unrealengine.com/zh-CN/ProductionPipelines/BuildTools/UnrealBuildTool/ModuleFiles/index.html)  
[第三方库](https://docs.unrealengine.com/zh-CN/ProductionPipelines/BuildTools/UnrealBuildTool/ThirdPartyLibraries/index.html)  
```
PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../../proto/include"));
```


<br />

# 待解决
```
把proto lib库项目集成到主项目里，Generate问题，
ThirdPersonMP.uproject 打开时的问题，   
静态库动态库问题，两个静态库链接问题(按理应该链接一个就好了)，  
Navmesh插件处理，插件整合，  
```


<br />

# 问题记录
```
生成protoc_gen_rpc插件
    gen.cc里用到protoc库里的全局变量和全局函数。 如果用静态库会有问题，因为是分开编译的，静态库里的全局数据没有初始化。 要用动态库。   
    cpp_helpers是在libprotoc里。   
    error LNK2001: 无法解析的外部符号。 这是因为VS项目的设置中仍然需要指定DLL对应的.lib的文件名称, DLL库文件只告诉工程到哪里去找这个DLL, 但并没有指定要寻找哪一个文件. 于是就需要手动指定.lib的名称
        [LNK2001: 无法解析的外部符号的几种情况](https://blog.csdn.net/c_base_jin/article/details/83010745)  
            1、引用第三方库时，确认lib库是否使用相同编译平台
            2、确认编译选项是否一致。比如运行时库和字符集
            3、若需要则添加指定编译宏
            4、添加附加库目录和附加依赖项
        windows下生成protoc动态库文件时，cpp_helpers.h里的ClassName(2)函数定义要加PROTOC_EXPORT。  
    字符集改成多字符扩展。  
    动态库入口点，库全局变量和全局函数问题。  

无法解析的外部符号 "class google::protobuf::internal::ExplicitlyConstructed fixed_address_empty_string
    添加宏 -DPROTOBUF_USE_DLLS  
```


<br />

# 参考

