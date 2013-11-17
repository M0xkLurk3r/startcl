//  This file is part of STARTCL.

//  STARTCL is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.

//  STARTCL is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with STARTCL.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <cstring>
#include <process.h>
#include <windows.h>
#include <io.h>
#include <winerror.h>
#include <tchar.h>

#pragma comment(lib,"Advapi32.lib")

using namespace std;

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL isWow64(){
    BOOL bIsWow64 = FALSE;
    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
    if(NULL != fnIsWow64Process){
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64)){
            //handle error
        }
    }
    return bIsWow64;
}

void clrchararray(char ch[]){           /* 清除数组内容，原理是使用循环将'\0'字符填入数组的每个元素来清空字符数组。   */ 
	int a;
	for (a=0;a<strlen(ch)+1;a++){
		ch[a]='\0';
	}
} 

void progra_exit(int exitcode){
	cout<<"STARTCL_KERNEL: 程序中止。退出码："<<exitcode<<"."<<endl;
	exit(exitcode);
} 

char* pathconnect(char* path, char* pathvalue){
	char ch[255]="\0";
	strcpy(ch,path);
	strcat(ch,"=");
	strcat(ch,pathvalue);
	return ch;
}

bool registeryRead(char* MJKey, char* pathToReg_raw, char* keyWords_raw, unsigned char *resultvalue){
	HKEY MJkey, hkey;
	unsigned char *getvalue=new unsigned char[255];
	LPCTSTR pathToReg=(LPCTSTR)pathToReg_raw;
	LPCTSTR keyWords=(LPCTSTR)keyWords_raw;
	DWORD dwLong, ddwSize;
	DWORD dwtype, sl = 256, i=0;
	int counter;
	if (strcmp(MJKey,"HKLM")==0) MJkey=HKEY_LOCAL_MACHINE;
	if (strcmp(MJKey,"HKCU")==0) MJkey=HKEY_CURRENT_USER;
	if (strcmp(MJKey,"HKLM")!=0 && strcmp(MJKey,"HKCU")!=0){
		cout<<"安全警告：此软件不支持外部函数调用。"<<endl;
		progra_exit(10);
		}
	if (RegOpenKeyEx(MJkey,pathToReg,0,KEY_READ,&hkey)==ERROR_SUCCESS){
		if (RegQueryValueEx(hkey,keyWords,NULL,NULL,(LPBYTE)getvalue,&ddwSize)==ERROR_SUCCESS){
			strcat((char *)resultvalue,(char *)getvalue);
			delete [] getvalue;
			return true;
		}else{
			delete [] getvalue;
			return false;
			}
		}else{
			delete [] getvalue;
			return false;
		}
}

bool autofindEnvAndAssign(char* keyEnvWords, char* vsVersion, char* cpuArch){
	unsigned char* result=new unsigned char[1000];     //  Automatically find the environment in the registery.
	unsigned char* result1=new unsigned char[1000];
	unsigned char* result2=new unsigned char[1000];
	unsigned char* dotnetname=new unsigned char[20];
	unsigned char* dotnetver=new unsigned char[20];
	if (strcmp((char *)keyEnvWords,"VS100COMNTOOLS")==0 || strcmp((char *)keyEnvWords,"VSINSTALLDIR")==0){
		strcpy((char *)result,"VS100COMNTOOLS=");
		strcpy((char *)result1,"VSINSTALLDIR=");
		if (isWow64()){
			if (registeryRead("HKLM","SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VS7",vsVersion,result)==true && registeryRead("HKLM","SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VS7",vsVersion,result1)==true){
				strcpy((char *)result,"Common7\\Tools");
				putenv((char *)result1);
				putenv((char *)result);
				delete [] result;
				delete [] result1;
				return true;
				}else{
					if (registeryRead("HKCU","SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VS7",vsVersion,result)==true && registeryRead("HKCU","SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VS7",vsVersion,result1)==true){
						strcpy((char *)result,"Common7\\Tools");
						putenv((char *)result1);
						putenv((char *)result);
						delete [] result;
						delete [] result1;
						return true;
						}else{
							delete [] result;
							delete [] result1;
							return false;
						}
					}
				}else{
					if (registeryRead("HKLM","SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7",vsVersion,result)==true && registeryRead("HKLM","SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7",vsVersion,result1)==true){
					strcat((char *)result,"Common7\\Tools");
					putenv((char *)result1);
					putenv((char *)result);
					delete [] result;
					delete [] result1;
					return true;
					}else{
						if (registeryRead("HKCU","SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7",vsVersion,result)==true && registeryRead("HKCU","SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7",vsVersion,result1)==true){
							strcat((char *)result,"Common7\\Tools");
							putenv((char *)result1);
							putenv((char *)result);
							delete [] result;
							delete [] result1;
							return true;
							}else{
								delete [] result;
								return false;
							}
						}
					}
	}
	if (strcmp(keyEnvWords,"WindowsSdkDir")==0){
		strcpy((char *)result,"WindowsSdkDir=");
			if (registeryRead("HKLM","SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v7.0A","InstallationFolder",result)==true){
				putenv((char *)result);
				delete [] result;
				delete [] result1;
				delete [] result2;
				delete [] dotnetname;
				delete [] dotnetver;
				return true;
				}else{
					if (registeryRead("HKCU","SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v7.0A","InstallationFolder",result)==true || registeryRead("HKLM","SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v7.0A","InstallationFolder",result)==true){
						putenv((char *)result);
						delete [] result;
						delete [] result1;
						delete [] result2;
						delete [] dotnetname;
						delete [] dotnetver;
						return true;
						}else{
							if (getenv("VCINSTALLDIR")!=0){
								strcat((char *)result,getenv("VCINSTALLDIR"));
								strcat((char *)result,"\\PlatformSDK");
								putenv((char *)result);
								delete [] result;
								delete [] result1;
								delete [] result2;
								delete [] dotnetname;
								delete [] dotnetver;
								return true;
								}else{
									delete [] result;
									delete [] result1;
									delete [] result2;
									delete [] dotnetname;
									delete [] dotnetver;
									return false;
								}
						}
				}
		}
	if (strcmp((char *)keyEnvWords,"VCINSTALLDIR")==0 || strcmp((char *)keyEnvWords,"FrameworkDir")==0){
		strcpy((char *)dotnetname,"FrameworkDir");
		strcpy((char *)dotnetver,"FrameworkVer");
		if (isWow64()){
			strcpy((char *)result,"VCINSTALLDIR=");
			strcpy((char *)result1,"FrameWorkDir");
			if (registeryRead("HKCU","SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VC7","10.0",result)==true || registeryRead("HKLM","SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VC7","10.0",result)==true){
				putenv((char *)result);
				delete [] result;
				if (strcmp((char *)cpuArch,"x86")==0 || strcmp((char *)cpuArch,"x86_amd64")==0 || strcmp((char *)cpuArch,"x86_ia64")==0){
				strcat((char *)dotnetname,"32");
				strcat((char *)dotnetver,"32");
				}else{
					strcat((char *)dotnetname,"64");
					strcat((char *)dotnetver,"64");
				}
				if (registeryRead("HKCU","SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VC7",(char *)dotnetname,result1)==true || registeryRead("HKCU","SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VC7",(char *)dotnetver,result2)==true || registeryRead("HKLM","SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VC7",(char *)dotnetname,result1)==true || registeryRead("HKLM","SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VC7",(char *)dotnetver,result2)==true){
					strcat((char *)dotnetname,"=");
					strcat((char *)dotnetver,"=");
					putenv((char *)result1);
					putenv((char *)result2);
					putenv((char *)dotnetname);
					putenv((char *)dotnetver);
					delete [] result1;
					delete [] result2;
					delete [] dotnetname;
					delete [] dotnetver;
					return true;
					}else{
					delete [] result1;
					delete [] result2;
					delete [] dotnetname;
					delete [] dotnetver;
					return false;
					}
				}else{
					delete [] result1;
					delete [] result2;
					delete [] dotnetname;
					delete [] dotnetver;
					return false;
			}
		}else{
			strcpy((char *)result,"VCINSTALLDIR=");
			strcpy((char *)result1,"FrameWorkDir=");
			if (registeryRead("HKCU","SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7","10.0",result)==true){
				putenv((char *)result);
				delete [] result;
				if (strcmp((char *)cpuArch,"x86")==0 || strcmp((char *)cpuArch,"x86_amd64")==0 || strcmp((char *)cpuArch,"x86_ia64")==0){
				strcat((char *)dotnetname,"32=");
				strcat((char *)dotnetver,"32=");
				}else{
					strcat((char *)dotnetname,"64=");
					strcat((char *)dotnetver,"64=");
				}
				if (registeryRead("HKCU","SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7",(char *)dotnetname,result1)==true && registeryRead("HKCU","SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7",(char *)dotnetver,result2)==true){
					putenv((char *)result1);
					putenv((char *)result2);
					putenv((char *)dotnetname);
					putenv((char *)dotnetver);
					delete [] result1;
					delete [] result2;
					delete [] dotnetname;
					delete [] dotnetver;
					return true;
				}else{
					delete [] result1;
					delete [] result2;
					delete [] dotnetname;
					delete [] dotnetver;
					return false;
				}
				}else{
					delete [] result1;
					delete [] result2;
					delete [] dotnetname;
					delete [] dotnetver;
					return false;
				}
		}
	}
	cout<<keyEnvWords<<" 未知错误。"<<endl;
	return false;
}
				
		
		
bool findEnvAndAssign(char** arg, char* keyEnvWords){   // Give the key words and the main functions' argv, this function will search the 
	char *tmpstr=new char[1000];						// key words in the arguments and accept it.
	char shorttmp[20]="\0";
	char shorttmpstr[20]="\0";
	int counter,counter_matrix=0;
	clrchararray(tmpstr);
	strcpy(tmpstr,keyEnvWords);
	strcat(tmpstr,"=");
	strcpy(shorttmp,tmpstr);
	while (1==1){
		if (arg[counter_matrix]!=0 && arg[counter_matrix]!=0){	// Search the key words. the next arguments was the arg_value, just assign it.
		if (strcmp(arg[counter_matrix],keyEnvWords)==0){
			strcat(tmpstr,arg[counter_matrix+1]);
		}else{
		}
		counter_matrix++;
		clrchararray(shorttmpstr);
	}else{
		break;
	}
	}
	if (strcmp(shorttmp,tmpstr)==0){
		putenv(tmpstr);											// Assign the environment path.
		delete [] tmpstr; 
		return false;
	}else{
		cout<<keyEnvWords<<" 设置成功。"<<endl;
		putenv(tmpstr);
		delete [] tmpstr;
		return true;
	}
}

int findStrInTheMatrix(char **matrix, char *keywords){  // find the current str in the double array(matrix) by first two character. Use this to search the strings in the argv
	int num=0;
	while (matrix[num]!=0){
		num++;
		if (matrix[num]!=0){
		if (matrix[num][0]==keywords[0] && matrix[num][1]==keywords[1]){
			return num;
		}else{
		}
	}else{
	}
	}
	return 65535;
} 

bool fileCouldbeused(char* filename){
	if (access(filename,00)==0 && access(filename,04)==0){
		return true;
	}else{
		return false;
	}
}

int findDotInThePath(char* filepath){
	int strpin,finalresult;
	int value=0;
	for (strpin=0;strpin<strlen(filepath)+1;strpin++){
		if (filepath[strpin]=='.') finalresult=strpin;
	}
	return finalresult;
}

void showhelp(char* argv0){
	cout<<"使用方法："<<argv0<<" [srcfile]-o[output]-a[arch][arg][arg_value]\n\n"<<endl;
	cout<<"[srcfile]\t输入文件(*.c;*.cpp;*.h;*.hpp;*.obj;*.o; etc)."<<endl;
	cout<<"[output]\t输出文件(*.exe;*.obj;*.dll; etc)."<<endl;
	cout<<"[arch]\tCPU架构。"<<endl;
	cout<<"\t可用值：x86,x86_amd64,amd64,x86_ia64"<<endl;
	cout<<"其他选项及其有效值（可选）:"<<endl;
	cout<<"\tCPMode: [Debug][Release][another], 设置编译器模式。"<<endl;
	cout<<"\tVCINSTALLDIR: [PATH],设置指向你的Visual C++安装地址。"<<endl;
	cout<<"\tVSINSTALLDIR: [PATH],设置指向你的Visual Studio安装地址。"<<endl;
	cout<<"\tVS100COMNTOOLS: [PATH],设置指向你的Visual C++ IDE安装地址。"<<endl;
	cout<<"\tWindowsSdkDir: [PATH],设置指向你的Windows SDK安装地址。"<<endl;
	cout<<"\tFrameworkDir: [PATH],设置指向你的.NET Framework安装地址。"<<endl;
	cout<<"\n反馈bug到lkphantom1995@gmail.com"<<endl;
	exit(0);    // Print the help message and execute the exit(). In someway execute showhelp() was equal to quit the program.
}

void version(void){
	cout<<"开源协议 GPLv3+: GNU GPL Version 3 or later (http://www.gnu.org/licenses/gpl.txt)"<<endl;
	cout<<"STARTCL 是一个自由软件，您可以自由修改代码并再发布，但您在做这些的同时必须遵守GNU GPLv3协议的各项条款。"<<endl;
	cout<<"STARTCL 作为自由软件没有担保。更多版权信息，请参阅COPYING。"<<endl;
	cout<<"\n\nbuilt: i386-VS2010"<<endl;
	exit(0);
}

int main(int argc, char* argv[]){
	char *outputf;
	char *outputfile=new char[255];
	char *outputfile_cl=new char[255];
	char *cl_tmpfile=new char[255];
	char *readargv;
	char* tmpchar;
	char* tmpstr;
	char *readcpuarch;
	char *filePoint;
	char *finalFile=new char[65500];
	bool inputFileisCpp=true;
	bool finalFile_Firstloop=true;
	bool isSdkDefined=true;
	char vcExec[255]="\0";
	char ldExec[255]="\0";
	int counter=1;      //  To verify the counter
	cout<<"CL-to-Dev-C++ 连接器 v0.01beta      by 1eekai"<<endl;
	cout<<"Copyright 2013 1eekai"<<endl;
	cout<<"查看 ReadME.MD 来获取帮助。\n"<<endl; 
	if (argv[1]==0){
		cout<<argv[0]<<": "<<"您必须设置一个合法的C/C++源码。编译中止。"<<endl;
		progra_exit(1);
	}else{
	if (strcmp(argv[1],"--version")==0){
		version();
		}
	if (strcmp(argv[1],"--help")==0){    // Line 102~135 brings the program down when user did not specified the argv[1~5].
		showhelp(argv[0]);				 // case of "argv[1~5]=NULL" will not even make the CL.exe not work well, it may make startcl dump core..........
	}else{
		if (findStrInTheMatrix(argv,"-o")==65535){
			cout<<argv[0]<<": "<<"您必须定义一个合法的输出文件地址(您是不是忘了使用\"-o\"选项？:)"<<endl;
			progra_exit(1);
		}else{
			outputf=&argv[findStrInTheMatrix(argv,"-o")+1][0];
			strcpy(outputfile,"/OUT:");
			strcpy(outputfile_cl,"/Fe");
			strcat(outputfile,outputf);
			strcat(outputfile_cl,outputf);
		}
	}
	for (counter=1;counter<findStrInTheMatrix(argv,"-o");counter++){
	if (fileCouldbeused(argv[counter])==false){
		cout<<argv[0]<<": "<<argv[counter]<<endl;
		cout<<argv[0]<<": "<<"文件不存在或者无法访问。"<<endl;
		progra_exit(1);
	}else{
		filePoint=&argv[counter][findDotInThePath(argv[counter])+1];
		if (strcmp(filePoint,"c")==0 || strcmp(filePoint,"h")==0 || strcmp(filePoint,"cpp")==0 || strcmp(filePoint,"hpp")==0 || strcmp(filePoint,"c++")==0 || strcmp(filePoint,"cxx")==0 || strcmp (filePoint,"cc")==0 || strcmp(filePoint,"cp")==0){
			inputFileisCpp=true;
		}else{
			inputFileisCpp=false;
		}
}
if (finalFile_Firstloop==true){
	strcpy(finalFile,argv[counter]);
	cout<<finalFile<<endl;
}else{
	strcat(finalFile," ");
	strcat(finalFile,argv[counter]);
	cout<<finalFile<<endl;
}
}
}
counter=1;
	if  (argv[findStrInTheMatrix(argv,"-o")+1]==0){
		cout<<argv[0]<<": "<<"您必须定义一个合法的输出文件地址。"<<endl;
		progra_exit(1);
	}
	
	if (findStrInTheMatrix(argv,"-a")==65535){
		cout<<argv[0]<<": "<<"您必须定义一个合法且被支持的CPU架构（E,g: x86,x86_amd64,amd64,x86_ia64）。"<<endl;
		progra_exit(1); 
	}else{
		readcpuarch=&argv[findStrInTheMatrix(argv,"-a")+1][0];
	}
		cout<<argv[0]<<": "<<"Setting runtime path for CL.exe."<<endl;
		tmpchar=(char *)new char[65535];
		tmpstr=(char *)new char[65535];  //Line 139~161 will set up the necessary environment of Visual Studio.
		if (findEnvAndAssign(argv,"VERSION")==false){
			cout<<argv[0]<<": VERSION项必须被定义。缺省使用10.0。"<<endl;
			putenv("VERSION=10.0");
			}
		if (findEnvAndAssign(argv,"VCINSTALLDIR")==false) 
			if (autofindEnvAndAssign("VCINSTALLDIR",getenv("VERSION"),readcpuarch)==false)
				cout<<argv[0]<<": VCINSTALLDIR 未定义或者自动配置错误！\n"<<argv[0]<<": 编译可能无法正常完成。"<<endl;
				else{
					clrchararray(vcExec);
					strcpy(vcExec,getenv("VCINSTALLDIR"));
					if (strcmp(readcpuarch,"x86")==0) strcat(vcExec,"bin\\cl.exe");
					if (strcmp(readcpuarch,"amd64")==0) strcat(vcExec,"bin\\amd64\\cl.exe");
					if (strcmp(readcpuarch,"x86_amd64")==0) strcat(vcExec,"bin\\x86_amd64\\cl.exe");
					if (strcmp(readcpuarch,"x86_ia64")==0) strcat(vcExec,"bin\\x86_ia64\\cl.exe");
					strcpy(ldExec,getenv("VCINSTALLDIR"));
					if (strcmp(readcpuarch,"x86")==0) strcat(ldExec,"bin\\link.exe");
					if (strcmp(readcpuarch,"amd64")==0) strcat(ldExec,"bin\\amd64\\link.exe");
					if (strcmp(readcpuarch,"x86_amd64")==0) strcat(ldExec,"bin\\x86_amd64\\link.exe");
					if (strcmp(readcpuarch,"x86_ia64")==0) strcat(ldExec,"bin\\x86_ia64\\link.exe");
				}
		if (findEnvAndAssign(argv,"VSINSTALLDIR")==false) 
			if (autofindEnvAndAssign("VSINSTALLDIR",getenv("VERSION"),readcpuarch)==false)
				cout<<argv[0]<<": VSINSTALLDIR 未定义或者自动配置错误！\n"<<argv[0]<<": 编译可能无法正常完成。"<<endl;
		if (findEnvAndAssign(argv,"VS100COMNTOOLS")==false) 
			if (autofindEnvAndAssign("VS100COMNTOOLS",getenv("VERSION"),readcpuarch)==false)
				cout<<argv[0]<<": VS100COMNTOOLS 未定义或者自动配置错误！\n"<<argv[0]<<": 编译可能无法正常完成。"<<endl;
		if (findEnvAndAssign(argv,"WindowsSdkDir")==false) 
			if (autofindEnvAndAssign("WindowsSdkDir",getenv("VERSION"),readcpuarch)==false){
				cout<<argv[0]<<": WindowsSdkDir 未定义或者自动配置错误！\n"<<argv[0]<<": 编译可能无法正常完成。"<<endl;
			isSdkDefined=false;
		}else{
			isSdkDefined=true;
		}
		if (findEnvAndAssign(argv,"FrameworkDir")==false) 
			if (autofindEnvAndAssign("FrameworkDir",getenv("VERSION"),readcpuarch)==false)
				cout<<argv[0]<<": 未定义或者自动配置错误！\n"<<argv[0]<<": 编译可能无法正常完成。"<<endl;
		if (findEnvAndAssign(argv,"CPMode")==false) {
		cout<<argv[0]<<": CPMode 未定义或者自动配置错误！编译中止。"<<endl;
		clrchararray(tmpchar);
		progra_exit(1);
		}else{
			if (strcmp(getenv("CPMode"),"Debug")==0){
				putenv("CPMode= ");
			}else{
			}
			if (strcmp(getenv("CPMode"),"Release")==0){
				putenv("CPMode=/MD /D \"NDEBUG\"");
			}else{
			}
		}
		strcpy(tmpchar,"LIB=");      // Load the library for VS.
		strcpy(tmpstr,"LIBPATH=");
		if (getenv("VCINSTALLDIR")!=0){
			strcat(tmpchar,getenv("VCINSTALLDIR"));
			strcat(tmpstr,getenv("VCINSTALLDIR"));
			strcat(tmpchar,"lib");
			strcat(tmpstr,"lib");
			if (strcmp(readcpuarch,"x86")!=0){
				strcat(tmpchar,"\\");
				strcat(tmpstr,"\\");
				strcat(tmpchar,readcpuarch);
				strcat(tmpstr,readcpuarch);
			}
			strcat(tmpchar,";");
			strcat(tmpstr,";");
			strcat(tmpchar,getenv("VCINSTALLDIR"));
			strcat(tmpstr,getenv("VCINSTALLDIR"));
			strcat(tmpchar,"ATLMFC\\lib");
			strcat(tmpstr,"ATLMFC\\lib");
			if (strcmp(readcpuarch,"x86")!=0){
				strcat(tmpchar,"\\");
				strcat(tmpstr,"\\");
				strcat(tmpchar,readcpuarch);
				strcat(tmpstr,readcpuarch);
			}
			strcat(tmpchar,";");
			strcat(tmpstr,";");
		}
		if (getenv("WindowsSdkDir")!=0){
			strcat(tmpchar,getenv("WindowsSdkDir"));
			strcat(tmpstr,getenv("WindowsSdkDir"));
			if (strcmp(readcpuarch,"x86")==0){
				strcat(tmpchar,"Lib;");
				strcat(tmpstr,"Lib;");
			}
			if (strcmp(readcpuarch,"amd64")==0){
				strcat(tmpchar,"lib\\x64;");
				strcat(tmpstr,"lib\\x64;");
			}
			if (strcmp(readcpuarch,"ia64")==0){
				strcat(tmpchar,"lib\\IA64;");
				strcat(tmpstr,"lib\\IA64;");
			}
		}
		putenv(tmpchar);
		putenv(tmpstr);
		clrchararray(tmpchar);
		clrchararray(tmpstr); 
		strcpy(tmpchar,"INCLUDE=");  // And the Include file path.
		if (getenv("VCINSTALLDIR")!=0){
			strcat(tmpchar,getenv("VCINSTALLDIR"));
			strcat(tmpchar,"INCLUDE;");
			strcat(tmpchar,getenv("VCINSTALLDIR"));
			strcat(tmpchar,"ATLMFC\\INCLUDE;");
		if (isSdkDefined==true){
			strcat(tmpchar,getenv("WindowsSdkDir"));
			strcat(tmpchar,"\\INCLUDE;");
			}
		}
		putenv(tmpchar);
		strcpy(tmpchar,"PATH=");         // Set the PATH file with VSINSTALLDIR.
		strcat(tmpchar,getenv("PATH"));
		strcat(tmpchar,";");
		if (getenv("VSINSTALLDIR")!=0){
		strcat(tmpchar,getenv("VSINSTALLDIR"));
		strcat(tmpchar,"Common7\\Tools;");
		strcat(tmpchar,getenv("VSINSTALLDIR"));
		strcat(tmpchar,"Common7\\IDE;");
		strcat(tmpchar,getenv("VSINSTALLDIR"));
		strcat(tmpchar,"VCPackages;");
		if (strcmp(readcpuarch,"x86")==0){
			if (getenv("VCINSTALLDIR")!=0) strcat(tmpchar,getenv("VCINSTALLDIR"));
			strcat(tmpchar,"\\bin;");
		}else{
			if (getenv("VCINSTALLDIR")!=0) strcat(tmpchar,getenv("VCINSTALLDIR"));
			strcat(tmpchar,"\\bin");
			strcat(tmpchar,readcpuarch);
			strcat(tmpchar,";");
		}
	}
		if (getenv("WindowsSdkDir")!=0){ 			// And the WindowsSdkDir.
			if (strcmp(readcpuarch,"x86")==0){
			strcat(tmpchar,getenv("VSINSTALLDIR"));
			strcat(tmpchar,"bin\\NETFX 4.0 Tools;");
			strcat(tmpchar,getenv("VSINSTALLDIR"));
			strcat(tmpchar,"bin;");
			}else{
			}
		}else{
		}
		if (getenv("WindowsSdkDir")!=0){
			if (strcmp(readcpuarch,"amd64")==0){
			strcat(tmpchar,getenv("VSINSTALLDIR"));
			strcat(tmpchar,"bin\\NETFX 4.0 Tools\\x64;");
			strcat(tmpchar,getenv("VSINSTALLDIR"));
			strcat(tmpchar,"bin\\x64;");
			}else{
			}
		}else{
		}
		if (getenv("VS100COMNTOOLS")!=0){
				strcat(tmpchar,getenv("VS100COMNTOOLS")); // And the VS100COMNTOOLS.
				tmpchar[strlen(tmpchar)-5]='\0';
				strcat(tmpchar,"IDE");
			}
		putenv(tmpchar);
		delete [] tmpstr;     // Free the heap memory.
		delete [] tmpchar;
		if (inputFileisCpp==true){
			cout<<argv[0]<<": 运行时环境设置完成，现在将调用CL.exe来编译您的代码。"<<endl;
			cout<<argv[0]<<": CL.exe 命令行输出：\n\n"<<endl;
		}else{
			cout<<argv[0]<<": 运行时环境设置完成，现在将调用Link.exe来连接您的二进制文件。"<<endl;
			cout<<argv[0]<<": LINK.exe 命令行输出：\n\n"<<endl;
		}
		if (fileCouldbeused(vcExec)==false){     // Prevent from program crash.
			cout<<argv[0]<<": "<<vcExec<<endl;
			cout<<argv[0]<<": "<<"文件不存在或无法访问！"<<endl;
			progra_exit(1);
		}else{
			if (inputFileisCpp==true){
			execl(vcExec,"cl.exe",getenv("CPMode"),finalFile,outputfile_cl,NULL);
		}else{
			execl(ldExec,"link.exe",finalFile,outputfile,NULL);
		}
		}
		delete [] outputfile;
		delete [] outputfile_cl;
		delete [] finalFile;
		return 0;
}
