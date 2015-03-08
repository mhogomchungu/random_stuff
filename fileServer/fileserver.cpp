#include "task.h"
#include "fileserver.h"
#include <QDebug>
#include <QMetaObject>
#include <QString>

static const char * onIcon  = "on.png" ;

static const char * offIcon = "off.png" ;

static const char * serverWorkPath = "/usr/bin/http_server" ;

static const char * serverName = "/usr/bin/http_server/bin/monkey" ;

static const char * header = "server address" ;

fileServer::fileServer() : statusicon()
{
	QCoreApplication::setApplicationName( "fileServer" ) ;

	m_exe.setWorkingDirectory( serverWorkPath ) ;
}

fileServer::~fileServer()
{
}

void fileServer::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void fileServer::run()
{
	statusicon::setStatus( statusicon::NeedsAttention ) ;

	statusicon::setCategory( statusicon::ApplicationStatus ) ;

	statusicon::setIcon( offIcon ) ;

	statusicon::addQuitAction() ;

	statusicon::setToolTip( offIcon,header,this->serverAddress() ) ;

	statusicon::clickActions actions ;

	actions.quitAction = [ this ](){

		if( m_exe.state() == QProcess::NotRunning ){

			QCoreApplication::quit() ;
		}
	} ;

	actions.onLeftClick = [ this ](){

		if( m_exe.state() == QProcess::NotRunning ){

			m_exe.start( serverName ) ;

			if( m_exe.state() != QProcess::NotRunning ){

				statusicon::setIcon( onIcon ) ;
				statusicon::setToolTip( onIcon,header,this->serverAddress() ) ;
			}
		}else{
			m_exe.terminate() ;

			statusicon::setIcon( offIcon ) ;
			statusicon::setToolTip( offIcon,header,this->serverAddress() ) ;
		}
	} ;

	statusicon::setIconClickedActions( std::move( actions ) ) ;
}

QString fileServer::serverAddress()
{
	return Task::await<QString>( [](){

		QProcess exe ;

		exe.start( "/sbin/ifconfig" ) ;
		exe.waitForFinished() ;

		QStringList e = QString( exe.readAll() ).split( '\n',QString::SkipEmptyParts ) ;

		QString g ;

		for( const auto& it : e ){

			if( it.contains( "inet addr:" ) ){

				QStringList z = it.split( ' ',QString::SkipEmptyParts ) ;

				QString q = z.at( 1 ) ;

				q.replace( "addr:","" ) ;

				if( q != "127.0.0.1" ){

					g = g + "\n" + q + ":2001" ;
				}
			}
		}

		return g ;
	} ) ;
}
