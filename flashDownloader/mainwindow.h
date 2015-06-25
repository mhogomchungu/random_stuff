#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QTimer>
#include <QCloseEvent>
#include <QSystemTrayIcon>

#include <functional>

namespace Ui {
class MainWindow;
}

static const int SIZE = 1024 ;

class FilePair : public QObject
{
	Q_OBJECT
public:
	FilePair() ;
	~FilePair() ;

	void setFunction( std::function< void( float,float,float) > function ) ;
	void setSource( const QString& ) ;
	bool sourceIsOpen() ;
	bool sourceIsNotOpen() ;
	void closeSource() ;
	bool sourceExists() ;

	void setDestination( const QString& ) ;
	bool destinationIsOpen() ;
	void closeDestination() ;
	bool destinationExists() ;

	void reset() ;

private slots:
	void transfer() ;
private:
	int m_source = -1 ;
	int m_destination = -1 ;
	char * m_buffer ;
	char * m_buffer_1 = nullptr ;
	char m_buffer_0[ SIZE ] ;
	int m_bufferSize ;
	QString m_dest ;
	quint64 m_dataSize = 0 ;
	std::function< void( float,float,float) > m_function ;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow( QWidget * parent = 0 ) ;
	~MainWindow() ;
	void closeEvent( QCloseEvent * ) ;
private slots:
	void pbCheckPlugin() ;
	void pbQuit() ;
	void pbDownLoadFile() ;
	void TrayIconClicked( QSystemTrayIcon::ActivationReason ) ;
	void killPlugin() ;
	void pbRefresh() ;
private:
	void setDefaults() ;
	QString getFlashPID() ;
	Ui::MainWindow * m_ui ;
	QTimer m_timer ;

	QSystemTrayIcon m_trayIcon ;
	quint64 m_dataSize = 0 ;

	FilePair m_files ;

	QString m_fileSize ;
};

#endif // MAINWINDOW_H
