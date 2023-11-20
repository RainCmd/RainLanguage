# 雨言2.0
**雨言（RainLanguage）是一门专为游戏开发而设计的编程语言，是面向对象和面向过程的混合型语言，拥有极简的语法和安全的类型检查，编译器还具有强大的类型推导能力，只要是通过表达式能推导出类型的地方都可以让编译器推导，而不用手动显式声明，熟悉C语言系列的程序员几乎可以无成本上手。**

**如果宿主语言是c++则可以直接使用Public文件夹下的头文件来调用，c语言可以使用ExternC文件夹下的ExternC.h中的接口，csharp可以使用ExternC中RainLanguageAdapter.cs已经封装好的接口。**

## Hello World
下面是使用`雨言`输出"Hello World"的示例代码：
``` rs
native Print(string)

Main()
    Print("Hello World")
```
native声明说明Print函数是在宿主语言中的实现的函数，它只有一个字符串参数，没有返回值，在调用时宿主语言会首先收到一个绑定函数的事件，事件包含需要绑定的函数的完整名称和参数类型列表，绑定完成后后续调用会直接调用被绑定的函数。

Main函数是定义在声明缩进空间的，所以它是一个全局函数，成员函数需要创建实例才能访问，而全局函数可以直接访问。雨言在虚拟机启动后没有默认的函数入口点，所以实际上是需要启动虚拟机后手动查找函数并调用。

## 功能和特性
- 通过缩进确定代码块，使代码更加整齐整洁。
- 统一的元组概念，元组之间可以任意组合，函数的参数和返回值本质上都是元组。
- 统一的类型系统，所有类型的基类都是handle类型，通过装箱拆箱操作，值类型也可以赋值给句柄类型
- 优化后的向量成员访问方式，可以通过向量的成员直接组合成其他向量。
- 支持面向对象和垃圾回收功能，开发时无需关心内存的释放。
- 支持编译为定点数版本，做帧同步游戏时无需再担心浮点数导致的不同步问题。
- 所有函数都是异步函数，可以在代码的任意位置插入wait语句，达到与线程休眠同样的效果。
- 支持热更新，可以在运行时编译运行代码文件，也可以将代码编译成库文件再发布，在运行时直接加载编译后的字节码文件，从而保证源码安全。
- 可以编译成多个库文件，运行时可以同时加载这些库文件，库文件内接口可以互相调用。更新库文件时只要被引用的公开接口或类型没有发生变化，被依赖的库文件就无需再次编译也能正常运行。
- 支持值类型，在高频调用的代码块中可以用来消除GC带来的影响，用值类型的数组，数据在堆中的排列也会更加紧凑，从而降低堆内存的开销。值类型的成员在编译时还会进行引用折叠，从而达到与局部变量一样的访问速度。
- 支持枚举类型，编译时可以进行常量折叠，拥有类型检查的同时也无需担心对代码运行效率的影响。
- 支持try catch finally语句，用于处理运行时异常，捕获异常时可以获取到异常的调用栈地址列表，配合debug模式下编译出的pdb文件可以还原出调用栈。
- 支持反射功能，任何定义前都可以通过方括号添加属性，运行时能获取到当前虚拟机中所有定义和他们的属性，可以根据类型的信息创建类型的未初始化对象，也可以通过反射获取到的函数信息创建委托或直接调用函数。
- 支持lambda表达式，可以实现函数式编程技术。

## 基本数据类型
* 值类型
  * `bool`:布尔类型
  * `byte`:字节类型
  * `char`:字符类型(2字节)
  * `integer`:整数类型(8字节)
  * `real`:实数类型(64位浮点数或16位小数48位整数的定点数)
  * `real2`:二维向量
  * `real3`:三维向量
  * `real4`:四维向量
  * `type`:指代运行时枚举，值类型，托管类型的类型(16字节)
  * `string`:字符串类型,只能长度为0不可为null，拥有独立的字符串堆(4字节)
  * `entity`:实体类型，相当于宿主语言中对象的指针，当虚拟机中没有实体的引用时会触发释放实体的时间，从而方便地实现声明周期的控制(4字节)
  * `enum`:枚举类型，除获取枚举名称外虚拟机会直接当整数类型处理(8字节)
* 托管类型
  * `handle`:所有类型的基类，存储与托管堆中，生命周期受垃圾回收机制控制，值类型赋值给句柄类型时会进行装箱操作
  * `interface`:所有接口类型的基类
  * `delegate`:委托类型，可以指向除构造和析构函数外的任意函数
  * `task`:任务类型，通过*start*和*new*可以创建异步任务
  * `array`:所有数组类型的基类

 ## 关键字
| 关键字 | 作用 | 关键字 | 作用 |
| :---: | :---: | :---: | :---: |
| namespace | 定义命名空间 | import | 导入命名空间 |
| native | 与宿主语言交互的函数 | public | 任意位置都可访问 |
| internal | 当前程序集内可访问 | space | 当前命名空间及子命名空间可访问 |
| protected | 当前类和子类可访问 | private | 仅当前类可访问 |
| enum | 枚举类型 | struct | 值类型 |
| class | 托管类型 | interface | 接口类型 |
| const | 声明常量 | global | 访问全局定义 |
| base | 基类 | this | 当前类 |
| true | 常量真 | false | 常量假 |
| null | 托管类型和实体类型的默认值 | var | 声明类型由编译器自动推导的变量 |
| bool | 布尔类型 | byte | 字节类型 |
| char | 字符类型 | integer | 整数类型 |
| real | 实数类型 | real2 | 二维向量 |
| real3 | 三维向量 | real4 | 四维向量 |
| type | 类型的类型 | string | 字符串类型 |
| handle | 所有类型的基类 | entity | 用于代指宿主语言对象的实体类型 |
| delegate | 委托类型 | task | 用于异步执行的任务类型 |
| array | 所有数组类型的基类 | if | 条件判断语句 |
| elseif | 连续条件判断语句 | else | 条件为假时执行的语句分支 |
| while | 循环语句 | for | 循环语句 |
| break | 跳出循环 | continue | 跳转到下一次循环 |
| return | 函数返回 | is | 判断对象的类型 |
| as | 判断对象类型并返回对象 | start | 开始执行异步任务 |
| new | 创建异步任务但不执行 | wait | 等待执行 |
| exit | 引发异常 | try | 尝试执行代码块 |
| catch | 捕获异常 | finally | 无论是否发生异常都会执行的代码块 |

## 程序结构
雨言中关键结构概念包括`程序集`、`命名空间`、`定义`和`成员`，除程序集外相同概念的申明缩进必须相同。声明在定义中的都是成员，包含构造函数，成员函数，析构函数和字段，定义声明在命名空间中，命名空间下可以有多个子命名空间，子命名空间的缩进与定义相同。编译时所有的命名空间下的定义都会打包到程序集中。

下面是包含这些结构的示例代码：
``` java
native Print(string)

namespace Earth
    class Preson
        public integer age
        public Preson(integer age)
            this.age = age
        public SayHello()
            Print("歪比巴卜")
    
    class Chinese Preson
        public Chinese() base(30)
        public SayHello()
            Print("你好")
    
    class Foreigner Preson
        public Foreigner() base(10)
        public SayHello()
            Print("Hello")
```

## 基本语法

### 注释
双斜杠后的内容都是注释，目前只支持单行注释。
``` java
//注释内容
```

### 运算符
支持几乎所有基础运算：&、&&、&=、|、||、|=、^、^=、<、<=、<<、<<=、>、>=、>>、>>=、+、++、+=、-、--、-=、\*、\*=、/、/=、%、%=、!、!=。需要特别说明的是位运算的优先级调整为比逻辑运算符和比较运算符高了，当位运算与这些运算符一起使用时不在需要括号来控制运算顺序了

#### 支持的其他运算符：
##### 条件运算符 `'?:'`
``` java
Func(condition ? exp1 : exp2)      //如果condition为真则执行exp1否则执行exp2，表达式的返回值类型必须一致
condition ? Func1() : Func2()       //可以丢弃表达式的返回值，表达式的返回值类型必须一致
condition ? Func()                  //可以省略第二个表达式，如果表达式为假，则返回第一个表达式返回值类型列表的默认值组成的元组
```
##### 空值传播运算符 `'?.'`、`'?->'`、`'?()'`和`'?[]'`
当空值传播判断前面为空时会返回元组类型列表默认值组成的元组
``` java
delegate handle[] TestDelegate()
class C
    public TestDelegate Func()
        return null

Main()
    C c = null
    var h = c?.Func()?()?[123]
```
##### 空值合并运算符 `'??'`
``` java
Main()
    var h = null
    var h2 = h ?? handle[1]
```

### 运算符重载
所有基础运算符都支持重载，自定义的运算符会覆盖默认运算符。重载的运算符规则与函数一样，只有可以访问时才会被调用生效。
``` java
public string *(string s, integer c)
    var result = ""
    while c-- > 0
        result += s
    return result

public integer +(integer[] values)
    var result = 0
    for var i, var length = 0, values.GetLength(); i < length; i++
        result += values[i]
    return result

public string *(integer a, integer b)
    return "Mul(" + a + ", " + b +")"

Main()
    Print("Abc" * 3)                //输出  AbcAbcAbc
    var arr = integer{2, 3, 4, 5}
    Print((+arr).ToString())        //输出 14
    Print(3 * 4)                    //输出 "Mul(3, 4)"
```


### 元组
元组是由一组数据通过逗号'`,`'或分号'`;`'组合而成的，逗号和分号的区别是组合的优先级不同，`';'`的计算优先级比lambda表达式符号(`=>`)、条件运算符(`?:`)、赋值运算符(`=`)低，在很多情况下可以省去多余的括号。元组还可以通过索引来重新组合成新的元组。
``` java
Func(integer a, integer b)
    Print(a + ", " + b)
Func(integer a, integer b, integer c)
    Print(a + ", " + b + ", " + c)

integer, bool, string GetTuple()
    return 1, true, "abc"

SetTuple(bool, integer, string)

Main()
    var a = 1
    var b = 2
    Func(a; b = 3; 4)   //输出 1, 3, 4
    Func(a, b)          //输出 1, 3
    Func(a, b = 3, 4)   //输出 3, 4
    Func(a, b)          //输出 3, 4

    Func(GetTuple()[1, 0, 2])   //将GetTuple返回的元组重新组合成新的元组并作为参数调用SetTuple
```

### 循环
- while：省略表达式则默认一直循环
- for：循环表达式用分号'`;`'分隔，第一个为初始化表达式，第二是循环条件，第三个及后续所有表达式会在循环结束后判断条件执行前执行，第三个及后续表达式可以省略
- elseif和else可以和循环组合使用，在循环条件为false时执行，如果用break跳出循环则不会执行
- break和continue后可以跟返回值类型为bool的表达式，当表达式为true时才会生效，省略表达式则表示默认为true
``` js
for var a, var b = 1, 2; a < 10 && b < 10; a += b, b++
    b -= a
    break b < 0
else
    Print("Exec else expression")
```

### 向量计算
内置的向量类型有`real2`,`real3`和`real4`。这几个向量的成员字段可任意组合成新的向量字段
``` java
Main()
    var v1 = real3(1, 2, 3)
    var v2 = v1.xyyz + v1.xxzz  //v2的类型是real4，值为(2, 3, 5, 6)
    var v3 = v2.wy              //v3的类型是real2，值为(6, 3)
```

### 枚举
枚举类型定义了一组整数常数值，可以省略赋值，如果省略赋值，则枚举的值会从0或第一个有值的枚举值递增。
``` kotlin
enum Sex
    Man
    Woman
    WalmartBag = 114514
    Gunship             //它的值是114515

enum Season
    Spring = 1
    Summer = 2
    Autumn = 4
    Winter = 8
    All = Spring | Summer | Autumn | Winter

```

### 结构体
结构体类型是存放在栈上的对象，它不能声明基类也不能继承接口，也不能派生其他类型。它有两个默认构造函数，一个是无参的，一个是包含所有字段的，结构体不能声明自定义的构造函数，结构体也没有析构函数，它可以包含自定义的字段和成员函数。
#### 字段
结构体的字段只能是公开的并且不能添加访问修饰符，结构体成员字段既可以通过字段名来访问，也可以通过解构后的索引访问，结构体字段的访问会在编译时进行引用折叠，不管嵌套多少层都能达到与局部变量相同的访问速度。
#### 成员函数
结构体的成员函数可以自定义访问修饰符，需要注意的是每次调用结构体的成员函数实际上都会将结构体进行拷贝，也就是说成员函数里的this指的不是调用方的对象，只是它的副本。
``` cs

struct S
    integer i
    public Set(integer i)
        this.i = i
        Print("this.i = " + this.i)

Main()
    var s = S(1)
    s.Set(2)
    Print("s.i = " + s.i)
```
以上代码会输出：
```
this.i = 2
si.i = 1
```
#### 构造和解构
结构体后加方括号可以进行解构，解构出来的是由它每个字段组合成的元组
``` cs
struct S1
    bool b
    integer i
    string s

struct S2
    string s
    integer i1
    integer i2

Main()
    var s1 = S1(false, 123, "abc")
    var s2 = S2(s1[2, 1, 1])            //s2的内容为 s:"abc" i1:123 i2:123
```

### 托管类型
托管类型是分配在托管堆上的对象，它可以继承一个基类和多个接口，也可以作为其他托管类型的基类，如果没有声明构造函数则编译器会自动生成一个默认的没有参数的构造函数。托管对象可以自定义构造函数，成员字段，成员函数和析构函数，除析构函数外的成员都可以自定义访问修饰符。
``` java
class C
    public string field
    public C(string value)
        Print("构造函数")
        field = value
    public MemberFunc()
        Print("成员函数")
    ~
        Print("析构函数")
```
#### 继承和override
托管类型的所有成员函数都是虚函数，都可以被override，需要函数可以被子类访问并且函数名、参数列表以及返回值列表一样就算override，如果参数列表不一样就只算重载，如果只有返回值列表不一致则会编译报错。
#### 虚调用和实调用
由于托管类型所有成员函数都是虚函数，所以对于某些不想被override的函数就需要额外的调用方式，于是便区分了实调用和虚调用，所有用`'.'`运算符调用的都是虚调用，如果函数被override则会调用到子类的函数中去，如果用`'->'`运算符调用则为实调用，调用的函数是编译时确定的，即使子类有override也不会受影响，因此实调用效率也会比虚调用稍高。
``` java
class Base
    public Func()
        Print("Base Func")
class Child Base
    public Func()
        Print("Child Func")

Main()
    Base b = Child()
    b.Func()            //输出 "Base Func"
    b->Func()           //输出 "Child Func"
```
#### 类型转换
有3中方式进行类型转换，分别为强制类型转换、is和as。
``` java
class A
class B A
    public Func()
Main()
    A a = B()
    B b1 = a as B
    if a is B b2        //如果只是判断类型，b2是可以省略的
        b2.Func()
    B b3 = B& a
```

### 接口
接口是一组函数的集合，接口内的成员函数都是公开的且不能添加访问修饰符。托管类型继承接口后必须实现接口的所有函数，否则编译不会通过。接口可以继承多个接口，但是不能继承托管类型。无法通过接口来创建对象。
``` java
interface Person
    integer Age()
    real Weight()
    Speed(string, real volume)
```

### 委托和lambda表达式
委托相当于是一个函数的类型的声明，可以用声明的类型去声明指向一类函数的变量，lambda表达式是用于创建简单的匿名函数。
``` js
delegate bool Comparison(C, C)

class C
    public integer i
    public integer value
    public C(integer i, integer value)
        this.i = i
        this.value = value

BubbleSort(C[] cs, Comparison c)
    var length = cs.Getlength()
    for var x = 0; x < length - 1; x++
        for var y = x + 1; y < length; y++
            if c(cs[y], cs[y -1])
                var tmp = cs[y]
                cs[y] = cs[y - 1]
                cs[y - 1] = tmp

bool CompC(C c1, C c2)
    return c1.value > c2.value
Main()
    var prime = 79
    var cs = C[64]
    for var i, var len = cs.GetLength(); i < len; i++
        cs[i] =  C(i, i * prime % len)
    BubbleSort(cs, CompC)               //将cs以i升序排列
    //cs后面跟的是分号，会优先于lambda表达式分割出来，
    //所以不影响后面lambda表达式的解析
    BubbleSort(cs; a, b => a.i < b.i)   //将cs以i降序排列
```

### 任务
任务是异步执行函数时产生的对象，用于获取执行状态以及执行结果等。所有任务都继承自kernel命名空间下的task类。当任务执行完成后可以通过方括号获取异步函数的返回值。
#### task定义：
``` java
namespace kernel
    public enum TaskState
		Unstart     //未开始执行
		Running     //执行中
		Completed   //已完成
		Aborted     //因抛异常而终止
		Invalid     //任务已失效

    public class task
        //开始执行异步函数，如果是使用start创建的task会自动调用Start(true, false)
        //如果当前任务不是未运行状态则会抛异常
        public Start(bool immediately, bool ignoreWait)
        //终止异步函数，会在异步函数当前执行的位置主动引发一次异常，参数为异常消息
        //如果任务当前不是运行状态则会抛异常
		public Abort(string message)
        //获取任务当前状态
		public TaskState GetState()
        //任务异常终止时获取终止信息
		public string GetExitCode()
        //任务是暂停状态，这个为true不影响GetState的返回值为Running
		public bool IsPause()
        //暂停任务
		public Pause()
        //恢复任务
		public Resume()
```
#### 创建并执行task
``` java
task string, integer TestTask

string, integer Func(string s, integer i)
    var result = 0
    while i-- > 0
        result += i
        Print(s + i)
        wait
    return result 

Main()
    TestTask t1 = start Func("s:", 3)
    TestTask t2 = new Func("n:", 4)
    t2.Start(false, false)
    //wait 后面跟执行中的任务会等待直到任务不是运行状态
    wait t1
    var s, var r = t1[]
    Print("result " + s + r)
    wait t2
    s, r = t2[]
    Print("result " + s + r)
```
以上代码将输出：
```
s:2
s:1
n:3
s:0
n:2
result s:3
n:1
n:0
result n:6
```

### 反射功能
反射相关功能都在kernel.Reflection命名空间下，可以通过kernel.System.GetAssembles()获取当前虚拟机中所有程序集，从而遍历程序集下所有定义，也可以获取定义的属性。
#### 属性的声明
如果属性后没有定义则属性会附加到当前命名空间上
``` cpp
["这个属性会附加到Space1上"]
namespace Space1
    
    ["这个属性会附加到GlobalValue1上", "这样可以同时附加多个属性"]
    ["这样与同一个方括号内用逗号分隔的效果一样"]
    integer GloablValue1

    ["这个属性也会附加到Space1上"]

["这个属性会附加到GlobalValue2上"]
integer GlobalValue2

["这个属性会附加到Func上"]
Func()

["这个属性会附加到当前程序集上"]
```
#### 直接获取类型
使用尖括号可以直接获取类型，这个类型信息是在编译时就生成，运行时没有查找类型信息的开销
``` java
class TestC

Main()
    var intType = <integer>
    Print(<TestC>.GetName())
```

### 异常处理
异常处理功能有助于处理在程序运行期间发生的任何意外或异常情况。异常处理功能使用 try, catch 和 finally关键字来尝试执行可能失败的操作、在你确定合理的情况下处理故障，以及事后清理资源。异常是使用exit创建的。虚拟机已使用的异常定义在kernel.System.Exceptions命名空间下。
``` java
import kernel.System.Exceptions
Func()
    exit "Func Exception!!"
    Print("Func End")
Main()
    var a = 1
    var b = 0
    try
        a /= b
    catch DivideByZero
        Print("catch Div by zero")
    finally
        Print("exec finally")

    try
        Func()
    catch string e
        Print("catch " + e)
```
以上代码将会输出：
```
catch Div by zero
exec finally
catch Func Exception!!
```


## 其他
* [kernel命名空间中的定义](./RainLanguage/RainLanguage/kernel.rain)
* [vscode雨言语法高亮插件](./RainLanguage/rain-script-0.0.3.vsix)