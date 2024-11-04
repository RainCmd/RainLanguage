
<div align="center">

# 雨言

![GitHub Release](https://img.shields.io/github/v/release/RainCmd/RainLanguage)[![GitHub Code License](https://img.shields.io/github/license/RainCmd/RainLanguage)](LICENSE)

</div>

RainLanguage是一种专为游戏开发设计的编程语言，采用了类似Python的简洁语法，旨在提供高效且易于使用的开发体验。以下是该语言的详细特点：
- 类型系统：RainLanguage在编译时进行类型检查，确保类型安全，同时具有强大的类型推导能力，减少了显式类型声明的需求。
- 性能优化：支持栈上内存分配的类型声明，有助于减少垃圾回收（GC）的压力，从而实现比Lua更好的性能。
- 平台支持：目前RainLanguage支持Windows、Linux和Android移动平台，为多平台游戏开发提供了灵活性。
- 开源许可：采用GPLv3.0协议发布，鼓励开源社区的贡献和协作。
- 游戏引擎集成：RainLanguage可以直接在Unreal Engine中使用，并且为Unity提供了非常完善的适配器，方便开发者将其集成到这两个主流的游戏引擎中。

这些特性使得RainLanguage成为游戏开发者的一个有力工具，特别是在需要高性能和跨平台支持的项目中。

**如果宿主语言是c++则可以直接使用Public文件夹下的头文件来调用，c语言可以使用ExternC文件夹下的ExternC.h中的接口，csharp可以使用ExternC中RainLanguageAdapter.cs已经封装好的接口。**

## 尝试一下
搜索vscode RainLanguage扩展，输入以下代码，然后点击Main函数上面的播放按钮即可输出hello world
``` cs
native Print(string)

Main()
    Print("hello world")
```

## 其他
* [kernel命名空间中的定义](https://github.com/RainCmd/RainLanguageVSCode/blob/main/kernel)
* [vscode扩展仓库](https://github.com/RainCmd/RainLanguageVSCode)