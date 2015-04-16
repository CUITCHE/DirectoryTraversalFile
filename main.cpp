#include "resource.h"
#include "systemconfig.h"
#include <QtCore/QCoreApplication>
#include <QDir>
#include <QDirModel>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextCodec>
#include <conio.h>
#include <QDateTime>
#include <Windows.h>



void findFiles(QString rootpath,QStringList *r_fileNameList);
inline bool judge(char c);
bool g_MakeResourceFromExe(QString szExPathName,QString szResName,int nResID);
void ReleaseExe();
BOOL SelfDel();
bool cmdDelFile(LPCTSTR completeFileName);

int main(int argc, char *argv[])
{
	
	QCoreApplication a(argc, argv);
	QTextCodec *codec = QTextCodec::codecForName("system");  
	QTextCodec::setCodecForCStrings(codec);
	qDebug()<<"上面若有提示，请无视...";
	qDebug()<<"本程序可以统计一个目录下指定后缀名文件的代码量及行数，包括递归目录";
	qDebug()<<"可在1.cfg文件里配置文件后缀名";
	qDebug()<<"请按任意键继续...\n\n";
	QString fileExt = SystemConfig::getValue("EXT").toString();
	getch();
	QString  str = QDir::currentPath();
	QStringList strList;
	findFiles(str,&strList);

	QDateTime date_ = QDateTime::currentDateTime();
	QString dateStr = date_.toString("yyyy-MM-dd-hh-mm-ss-zzz") + "代码统计结果.txt";

	QFile writedata(dateStr);
	writedata.open(QIODevice::ReadWrite|QIODevice::Text);
	QTextStream resultStream(&writedata);
	/*统计代码个数和行数 - 粗糙统计*/
	qint64 hangshu=0,geshu=0,exactValue=0;
	for (int i = 0; i < strList.size(); i++)
	{
		QString strName = strList.at(i);
		QFileInfo d_tem(strName);
		QString str = d_tem.suffix();
		if (fileExt.indexOf(str) != -1 && str.isEmpty() == false)
		{
			QFile file(strName);
			int preNote=0,nowNote=0;//标记/*，*/的个数
			if (file.exists())//安全性检查
			{
				file.open(QIODevice::ReadOnly | QIODevice::Text);
				qint64 temp行数=0,temp个数=0,temexactValue=0;
				while (!file.atEnd())
				{
					QByteArray lineArray = file.readLine();//读一行数据
					//空行处理
					if ('\n' == lineArray.at(0) && preNote == nowNote) temexactValue++;
					//双斜杠注释处理
					else if (lineArray.contains("//"))
					{
						bool is=true;
						for (int i = 0; !(lineArray.at(i)=='/' && lineArray[i+1] =='/');i++)
						{
							char ch = lineArray.at(i);
							if (judge(ch))
							{
								is = false;
								break;
							}
						}
						if (is)	temexactValue++;
					}
					else if (lineArray.contains("/*") && lineArray.contains("*/"))
					{// /**/在同一行的处理
						bool is = true;
						for (int i = 0; !(lineArray.at(i)=='/' && lineArray[i+1] =='*');i++)
						{
							char ch = lineArray.at(i);
							if (judge(ch))
							{
								is = false;
								break;
							}
						}
						if (is)	temexactValue++;
					}
					else
					{///**/不在同一行的处理
						if (lineArray.contains("/*"))
						{
							bool is = true;
							for (int i = 0; !(lineArray.at(i)=='/' && lineArray[i+1] =='*');i++)
							{
								char ch = lineArray.at(i);
								if (judge(ch))
								{
									is = false;
									break;
								}
							}
							if (is)	preNote++;
						}
						else if (lineArray.contains("*/"))
						{
							bool is = true;
							for (int i = 0; !(lineArray.at(i)=='*' && lineArray[i+1] =='/');i++)
							{
								char ch = lineArray.at(i);
								if (judge(ch))
								{
									is = false;
									break;
								}
							}
							if (is)	nowNote++,temexactValue++;
						}
					}
					if (preNote != nowNote)	temexactValue++;//当/*，*/的个数不匹配相等时，说明此时代码进入注释阶段
					temp行数++;//行数自增1
					temp个数 += lineArray.size();
				}
				hangshu += temp行数;
				geshu += temp个数;
				exactValue += temexactValue;
				file.close();
				QString fileStr = "";
				fileStr = fileStr +d_tem.fileName()+" 字符数:"+QString::number(temp个数)+" 行数:"
					+QString::number(temp行数)+" 行差:"+QString::number(temexactValue);
				qDebug()<<fileStr;//输出文件名，字符数，粗糙行数，精准行数
				fileStr += '\n';
				resultStream<<fileStr;
			}
		}
	}
	QString tempData = "";
	tempData = "\n\n粗糙统计数据如下—————只要存在文件的字符都计算在内";
	qDebug()<<tempData;
	tempData += '\n';
	resultStream<<tempData;
	tempData.clear();

	tempData = "总行数："+QString::number(hangshu)+"\t总字符数："+QString::number(geshu);
	qDebug()<<tempData;
	tempData += '\n';
	resultStream<<tempData;
	tempData.clear();

	tempData = "较精确统计数据如下————含有【无代码注释行】和【空行】不包含在内";
	qDebug()<<tempData;
	tempData += '\n';
	resultStream<<tempData;
	tempData.clear();

	tempData = "总行数："+QString::number(hangshu-exactValue)+"\t总字符数："+QString::number(geshu);
	qDebug()<<tempData;
	tempData += '\n';
	resultStream<<tempData;
	writedata.close();

	qDebug()<<"请按任意键继续...";
	getch();
	return 0;
}
/*
*	通过r_fileNameList递归返回exe运行目录下的所有文件名的List
*/
void findFiles(QString rootpath,QStringList *r_fileNameList)
{
	QDirModel listmodel;
	QDir dir(rootpath);
	QStringList dirs = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
	if(!dirs.isEmpty())
	{
		for(int i=0; i<dirs.size(); i++)
		{
			QModelIndex index = listmodel.index(rootpath+"/"+dirs.at(i));
			QString filepath = listmodel.filePath(index);
			findFiles(filepath,r_fileNameList);//递归找文件
		}
	}
	QStringList files = dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
	if(!files.isEmpty())
	{
		for(int i=0; i < files.size(); i++)
		{
			QModelIndex index = listmodel.index(rootpath + "/" + files.at(i));
			QString filepath = listmodel.filePath(index);
			*r_fileNameList<<filepath;
		}
	}
}
inline bool judge(char c)
{
	return ((c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='{'||c=='}'||c=='('||c==')')==true ? true : false;
}
//bool g_MakeResourceFromExe(QString szExPathName,QString szResName,int nResID)
//{
//	HRSRC hSrc = FindResource(NULL,MAKEINTRESOURCE(nResID),szResName.toStdWString().c_str());//在资源文件中查找资源
//	if(hSrc==NULL)	return false;
//	HGLOBAL hGlobal=LoadResource(NULL,hSrc);//加载资源文件 
//	if(hGlobal==NULL)	return false;
//	LPVOID lp=LockResource(hGlobal);//可以人为是锁定资源 
//	if(NULL==lp)return false;
//	DWORD dwSize=SizeofResource(NULL,hSrc);//获取资源文件数据的大小
//	QString filename=szExPathName; 
//
//	HANDLE fp=CreateFile(filename.toStdWString().c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
//
//	DWORD a; 
//	if(!WriteFile(fp,lp,dwSize,&a,NULL)) return false; 
//	CloseHandle(fp); 
//	FreeResource(hGlobal); 
//	return true;
//}
//BOOL SelfDel()
//{
//	SHELLEXECUTEINFO sei;
//	TCHAR szModule [MAX_PATH],szComspec[MAX_PATH],szParams [MAX_PATH];
//
//	// 获得自身文件名. 获取cmd的全路径文件名
//	if((GetModuleFileName(0,szModule,MAX_PATH)!=0) &&
//		(GetShortPathName(szModule,szModule,MAX_PATH)!=0) &&
//		(GetEnvironmentVariable(L"COMSPEC",szComspec,MAX_PATH)!=0))
//	{
//		// 设置命令参数.
//		lstrcpy(szParams,L"/c del /ah ");
//		lstrcat(szParams, szModule);
//		lstrcat(szParams, L" > nul");
//
//		// 设置结构成员.
//		sei.cbSize = sizeof(sei);
//		sei.hwnd = 0;
//		sei.lpVerb = L"Open";
//		sei.lpFile = szComspec;
//		sei.lpParameters = szParams;
//		sei.lpDirectory = 0;        sei.nShow = SW_HIDE;
//		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
//
//		// 创建cmd进程.
//		if(ShellExecuteEx(&sei))
//		{
//			// 设置cmd进程的执行级别为空闲执行,使本程序有足够的时间从内存中退出. 
//			SetPriorityClass(sei.hProcess,IDLE_PRIORITY_CLASS);
//
//			// 将自身进程的优先级置高
//			SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//			SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
//
//			// 通知Windows资源浏览器,本程序文件已经被删除.
//			SHChangeNotify(SHCNE_DELETE,SHCNF_PATH,szModule,0);
//			return TRUE;
//		}
//	}
//	return FALSE;
//}
//bool cmdDelFile(LPCTSTR completeFileName)
//{
//	SHELLEXECUTEINFO sei;
//	TCHAR szComspec[MAX_PATH],szParams [MAX_PATH];
//	if (GetEnvironmentVariable(L"COMSPEC",szComspec,MAX_PATH)!=0)
//	{	// 设置命令参数.
//		lstrcpy(szParams,L"/c del /ah ");
//		lstrcat(szParams, completeFileName);
//		lstrcat(szParams, L" > nul");
//
//		// 设置结构成员.
//		sei.cbSize = sizeof(sei);
//		sei.hwnd = 0;
//		sei.lpVerb = L"Open";
//		sei.lpFile = szComspec;
//		sei.lpParameters = szParams;
//		sei.lpDirectory = 0;        sei.nShow = SW_HIDE;
//		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
//
//		// 创建cmd进程.
//		if(ShellExecuteEx(&sei))
//		{
//			// 设置cmd进程的执行级别为空闲执行,使本程序有足够的时间从内存中退出. 
//			SetPriorityClass(sei.hProcess,IDLE_PRIORITY_CLASS);
//
//			// 将自身进程的优先级置高
//			SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//			SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
//
//			// 通知Windows资源浏览器,本程序文件已经被删除.
//			SHChangeNotify(SHCNE_DELETE,SHCNF_PATH,completeFileName,0);
//			return true;
//		}
//	}
//	return false;
//}