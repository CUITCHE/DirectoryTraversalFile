#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H

#include <QObject>
#include <QSettings>
#include <QVariant>

class SystemConfig : public QObject
{
	Q_OBJECT
public:
	SystemConfig(QObject *parent);
	~SystemConfig();
	static QVariant getValue(const QString &key);
	static void setValue(const QString &key, const QVariant& value);
	static void Remove();
private:
	static QSettings* m_sysSetting;

};

#endif // SYSTEMCONFIG_H
