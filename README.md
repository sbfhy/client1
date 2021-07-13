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
[proto/README.md](proto/README.md)  




<br />

# 待做
```
把proto lib库项目集成到主项目里，  
ThirdPersonMP.uproject 打开时的问题，   

```


<br />

# 参考

