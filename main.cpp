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
	qDebug()<<"����������ʾ��������...";
	qDebug()<<"���������ͳ��һ��Ŀ¼��ָ����׺���ļ��Ĵ������������������ݹ�Ŀ¼";
	qDebug()<<"����1.cfg�ļ��������ļ���׺��";
	qDebug()<<"�밴���������...\n\n";
	QString fileExt = SystemConfig::getValue("EXT").toString();
	getch();
	QString  str = QDir::currentPath();
	QStringList strList;
	findFiles(str,&strList);

	QDateTime date_ = QDateTime::currentDateTime();
	QString dateStr = date_.toString("yyyy-MM-dd-hh-mm-ss-zzz") + "����ͳ�ƽ��.txt";

	QFile writedata(dateStr);
	writedata.open(QIODevice::ReadWrite|QIODevice::Text);
	QTextStream resultStream(&writedata);
	/*ͳ�ƴ������������ - �ֲ�ͳ��*/
	qint64 hangshu=0,geshu=0,exactValue=0;
	for (int i = 0; i < strList.size(); i++)
	{
		QString strName = strList.at(i);
		QFileInfo d_tem(strName);
		QString str = d_tem.suffix();
		if (fileExt.indexOf(str) != -1 && str.isEmpty() == false)
		{
			QFile file(strName);
			int preNote=0,nowNote=0;//���/*��*/�ĸ���
			if (file.exists())//��ȫ�Լ��
			{
				file.open(QIODevice::ReadOnly | QIODevice::Text);
				qint64 temp����=0,temp����=0,temexactValue=0;
				while (!file.atEnd())
				{
					QByteArray lineArray = file.readLine();//��һ������
					//���д���
					if ('\n' == lineArray.at(0) && preNote == nowNote) temexactValue++;
					//˫б��ע�ʹ���
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
					{// /**/��ͬһ�еĴ���
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
					{///**/����ͬһ�еĴ���
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
					if (preNote != nowNote)	temexactValue++;//��/*��*/�ĸ�����ƥ�����ʱ��˵����ʱ�������ע�ͽ׶�
					temp����++;//��������1
					temp���� += lineArray.size();
				}
				hangshu += temp����;
				geshu += temp����;
				exactValue += temexactValue;
				file.close();
				QString fileStr = "";
				fileStr = fileStr +d_tem.fileName()+" �ַ���:"+QString::number(temp����)+" ����:"
					+QString::number(temp����)+" �в�:"+QString::number(temexactValue);
				qDebug()<<fileStr;//����ļ������ַ������ֲ���������׼����
				fileStr += '\n';
				resultStream<<fileStr;
			}
		}
	}
	QString tempData = "";
	tempData = "\n\n�ֲ�ͳ���������¡���������ֻҪ�����ļ����ַ�����������";
	qDebug()<<tempData;
	tempData += '\n';
	resultStream<<tempData;
	tempData.clear();

	tempData = "��������"+QString::number(hangshu)+"\t���ַ�����"+QString::number(geshu);
	qDebug()<<tempData;
	tempData += '\n';
	resultStream<<tempData;
	tempData.clear();

	tempData = "�Ͼ�ȷͳ���������¡����������С��޴���ע���С��͡����С�����������";
	qDebug()<<tempData;
	tempData += '\n';
	resultStream<<tempData;
	tempData.clear();

	tempData = "��������"+QString::number(hangshu-exactValue)+"\t���ַ�����"+QString::number(geshu);
	qDebug()<<tempData;
	tempData += '\n';
	resultStream<<tempData;
	writedata.close();

	qDebug()<<"�밴���������...";
	getch();
	return 0;
}
/*
*	ͨ��r_fileNameList�ݹ鷵��exe����Ŀ¼�µ������ļ�����List
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
			findFiles(filepath,r_fileNameList);//�ݹ����ļ�
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
//	HRSRC hSrc = FindResource(NULL,MAKEINTRESOURCE(nResID),szResName.toStdWString().c_str());//����Դ�ļ��в�����Դ
//	if(hSrc==NULL)	return false;
//	HGLOBAL hGlobal=LoadResource(NULL,hSrc);//������Դ�ļ� 
//	if(hGlobal==NULL)	return false;
//	LPVOID lp=LockResource(hGlobal);//������Ϊ��������Դ 
//	if(NULL==lp)return false;
//	DWORD dwSize=SizeofResource(NULL,hSrc);//��ȡ��Դ�ļ����ݵĴ�С
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
//	// ��������ļ���. ��ȡcmd��ȫ·���ļ���
//	if((GetModuleFileName(0,szModule,MAX_PATH)!=0) &&
//		(GetShortPathName(szModule,szModule,MAX_PATH)!=0) &&
//		(GetEnvironmentVariable(L"COMSPEC",szComspec,MAX_PATH)!=0))
//	{
//		// �����������.
//		lstrcpy(szParams,L"/c del /ah ");
//		lstrcat(szParams, szModule);
//		lstrcat(szParams, L" > nul");
//
//		// ���ýṹ��Ա.
//		sei.cbSize = sizeof(sei);
//		sei.hwnd = 0;
//		sei.lpVerb = L"Open";
//		sei.lpFile = szComspec;
//		sei.lpParameters = szParams;
//		sei.lpDirectory = 0;        sei.nShow = SW_HIDE;
//		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
//
//		// ����cmd����.
//		if(ShellExecuteEx(&sei))
//		{
//			// ����cmd���̵�ִ�м���Ϊ����ִ��,ʹ���������㹻��ʱ����ڴ����˳�. 
//			SetPriorityClass(sei.hProcess,IDLE_PRIORITY_CLASS);
//
//			// ��������̵����ȼ��ø�
//			SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//			SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
//
//			// ֪ͨWindows��Դ�����,�������ļ��Ѿ���ɾ��.
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
//	{	// �����������.
//		lstrcpy(szParams,L"/c del /ah ");
//		lstrcat(szParams, completeFileName);
//		lstrcat(szParams, L" > nul");
//
//		// ���ýṹ��Ա.
//		sei.cbSize = sizeof(sei);
//		sei.hwnd = 0;
//		sei.lpVerb = L"Open";
//		sei.lpFile = szComspec;
//		sei.lpParameters = szParams;
//		sei.lpDirectory = 0;        sei.nShow = SW_HIDE;
//		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
//
//		// ����cmd����.
//		if(ShellExecuteEx(&sei))
//		{
//			// ����cmd���̵�ִ�м���Ϊ����ִ��,ʹ���������㹻��ʱ����ڴ����˳�. 
//			SetPriorityClass(sei.hProcess,IDLE_PRIORITY_CLASS);
//
//			// ��������̵����ȼ��ø�
//			SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//			SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
//
//			// ֪ͨWindows��Դ�����,�������ļ��Ѿ���ɾ��.
//			SHChangeNotify(SHCNE_DELETE,SHCNF_PATH,completeFileName,0);
//			return true;
//		}
//	}
//	return false;
//}