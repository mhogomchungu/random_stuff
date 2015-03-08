#ifndef FILESERVER_H
#define FILESERVER_H
#include <QStringList>
#include <QProcess>
#include <QObject>
#include "statusicon.h"
#include "mainwindow.h"

class fileServer : public statusicon
{
	Q_OBJECT
public:
	fileServer() ;
	~fileServer() ;
	void start() ;
private slots:
	void run() ;
private:
	QProcess m_exe ;
	QString serverAddress() ;
};

#endif // FILESERVER_H
