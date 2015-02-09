#include "systemconfig.h"
#include <QFile>
#include <QStringList>
#include <QCoreApplication>
QSettings* SystemConfig::m_sysSetting = NULL;

SystemConfig::SystemConfig(QObject *parent) : QObject(parent)
{

}

SystemConfig::~SystemConfig()
{
	if (m_sysSetting)
	{
		delete m_sysSetting;
		m_sysSetting = NULL;
	}
}

QVariant SystemConfig::getValue(const QString &key)
{
	if(!m_sysSetting)
	{ 
		QString runPath = QCoreApplication::applicationDirPath();
		runPath += "/1.cfg";
		m_sysSetting = new QSettings(runPath, QSettings::IniFormat);
		m_sysSetting->setIniCodec("UTF-8");
	}
	return m_sysSetting->value(key);
}

void SystemConfig::setValue(const QString &key, const QVariant& value)
{	
	if(!m_sysSetting)
	{
		QString runPath = QCoreApplication::applicationDirPath();
		runPath += "/1.cfg";
		m_sysSetting = new QSettings(runPath, QSettings::IniFormat);
		m_sysSetting->setIniCodec("UTF-8");
	}
	m_sysSetting->setValue(key, value);
}

void SystemConfig::Remove()
{
	QString runPath = QCoreApplication::applicationDirPath();
	runPath += "/1.cfg";
	
 	QFile file(runPath);;
	file.open(QFile::ReadOnly);
	QStringList strList;
	while (!file.atEnd())
	{
		strList.push_back(file.readLine());
	}
	file.close();
	QFile::remove(runPath);
	QFile f(runPath);
	f.open(QFile::WriteOnly);
	for (int i = 0;i < strList.size();++i)
	{
		f.write(strList.at(i).toStdString().c_str());
	}
	f.close();
	
// 	QTextStream ts(&file);
// 	ts <<"[General]" << endl;
// 	ts <<"SERVERIP=" << endl;
// 	ts <<"SERVERPORT=" << endl;
// 	ts <<"SERVERNAME=" << endl;
// 	ts <<"USERID=" << endl;
// 	ts <<"PASSWD=";
// 	file.close();
	
// 	QVariant ip(QString::fromStdString(SERVERSTR));
// 	QVariant port(SERVERPORT);
// 	QVariant name(QString::fromStdString(SERVERNAME));
// 	SystemConfig::setValue("SERVERIP",QString::fromStdString(SERVERSTR));
// 	SystemConfig::setValue("SERVERPORT",SERVERPORT);
// 	SystemConfig::setValue("SERVERNAME",QString::fromStdString(SERVERNAME));
/*	
	if(!m_sysSetting)
	{
		m_sysSetting = new QSettings("SystemConfig.1.cfg", QSettings::IniFormat);
		m_sysSetting->setIniCodec("UTF-8");
	}
	m_sysSetting->setValue("login/USERID","");
	m_sysSetting->setValue("login/PASSWD","");
*/	
// 	if(!m_sysSetting)
// 	{
// 		m_sysSetting = new QSettings("SystemConfig.1.cfg", QSettings::IniFormat);
// 		m_sysSetting->setIniCodec("UTF-8");
// 	}
// 	m_sysSetting->remove(key);
// 	QStringList keys = m_sysSetting->allKeys();
}
