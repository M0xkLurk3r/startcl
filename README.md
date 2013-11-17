startcl
=======

A connector makes DEV-C++ complies software via CL.exe

这个软件允许您在DEV-C++集成环境里编译CL.exe的软件。
作者认为，在Windows下可以使用的C/C++编译器，迄今为止，最好的仍然是VS2010的CL.exe。但是DEV-C++没有对CL.exe的原生支持，而且在Tools->Compiler Options里面不能直接添加CL.exe作为编译器使用（DEV-C++仅支持调用gcc）。
为了解决这个问题，本人编写了STARTCL。
此软件设计的初衷是为了能在DEV-C++里面使用CL.exe。当然，也可以在其他兼容的IDE里面使用本软件。

========================================================================================================

版权声明：
本软件遵循GNU GPLv3的开源协议。更多内容，请查看下面的协议原文。

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

STARTCL 是一个自由软件，您可以自由修改代码并再发布，但您在做这些的同时必须遵守GNU GPLv3协议的各项条款。
STARTCL 作为自由软件没有担保。更多版权信息，请参阅COPYING。

========================================================================================================

使用方法：
命令参数：startcl [srcfile]-o[output]-a[arch][arg][arg_value]


[srcfile]       输入文件(*.c;*.cpp;*.h;*.hpp;*.obj;*.o; etc).
[output]        输出文件(*.exe;*.obj;*.dll; etc).
[arch]  CPU架构。
        可用值：x86,x86_amd64,amd64,x86_ia64
其他选项及其有效值（可选）:
        CPMode: [Debug][Release][another], 设置编译器模式。
        VCINSTALLDIR: [PATH],设置指向你的Visual C++安装地址。
        VSINSTALLDIR: [PATH],设置指向你的Visual Studio安装地址。
        VS100COMNTOOLS: [PATH],设置指向你的Visual C++ IDE安装地址。
        WindowsSdkDir: [PATH],设置指向你的Windows SDK安装地址。
        FrameworkDir: [PATH],设置指向你的.NET Framework安装地址。

可选的值个人建议不必设置！除非您认为您的Visual Studio安装上有点问题 -- 只要这些目录是对的且CL的文件没有被破坏，即使在注册表中的安装信息丢失，此程序也可以正常工作。


一般使用方法（命令行下的操作，个人建议在IDE里面操作）：

>startcl .\foo1.cpp -o foo1.exe -a x86 CPMode Release VERSION 10.0
（VS版本为10.0，Release模式，x86架构）

>startcl .\foo1.cpp -o foo1.exe -a x86 CPMode Debug VERSION 10.0
（VS版本为10.0，Debug模式，x86架构）

>startcl .\foo1.cpp -o foo1.exe -a amd64 CPMode Release VERSION 10.0
（VS版本为10.0，Release模式，amd64架构）（保证您的电脑是x64架构且...额，还有，您是否安装了VC x64编译器，这个应该是主角的）


>startcl .\foo1.cpp -o foo1.exe -a x86 CPMode Release VERSION 10.0 VCINSTALLDIR C:\Dev-Cpp\bin VCINSTALLDIR C:\Dev-Cpp\
（VS版本为10.0，Release模式，x86架构。指定目录会导致软件放弃查找注册表中的默认目录，设置成用户给定的值。唔，这个好像不太对的节奏。。。。）



在DEV-C++中的操作（本人用的是中文版的DEV-C++）：
1. 打开DEV-C++，选择工具(T)->编译选项(C)，为STARTCL新建一个编译器方案。
2. 勾选“编译时加入以下命令”，并在下面添加这些内容：
  
  -a x86 CPMode Release VERSION 10.0
  
3. 将本软件复制到<GCCHOME>/bin，<GCCHOME>代表gcc工具的目录。（为什么这么做，稍后会有说明）
4. 选择“目录”选项卡，打开“二进制”，并添加<GCCHOME>/bin。
5. 选择“程序”选项卡，设置make 为gcc默认的GNU Make，Windres亦如此，其余均修改成startcl.exe。
6. 设置完成。

第二步填入的内容在不同的编译模式和生成目标可执行文件的情况下有差别。这些均参照上面命令行设置的格式即可。具体如何设置，请参阅上面的命令参数。

===============================================================================================================================================

编译：

STARTCL 是一个自由软件，您可以自由修改代码并再发布，但您在做这些的同时必须遵守GNU GPLv3协议的各项条款。
关于如何编译本软件，唔，首先参阅github如何把源码下载到你的硬盘上的方法下载我的源码。
下载完毕后应该有这么几个文件：

>Root:
>  -main.cpp           // 主程序文件。
>   -main.res           // 资源文件，编成以后会在EXE里填写本软件的version信息啦。
>  -Makefile.bat       // Makefile脚本。
>  -COPYING            // 版权声明。
>  -README.md          // 就是您现在看到的这个了咩~

解压完毕后，请使用Visual Studio命令提示运行目录下的Makefile.bat。关于如何使用Visual Studio命令提示，请参阅微软的MSDN文档：http://msdn.microsoft.com/zh-cn/library/ms229859.aspx

运行后将在目录下创建新目录 Release ，并把生成的可执行文件放到这个目录下。之后您可以使用啦~



编写于 2013/11/17

1eekai

请将bug反馈至lkphantom1995@gmail.com






