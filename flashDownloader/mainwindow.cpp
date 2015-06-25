#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <unistd.h>
#include <QMetaMethod>

#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "task.h"

static QString _destination_path( const QString& e )
{
	Q_UNUSED( e ) ;
	return QDir::homePath() + "/" + e ;
	//return "/home/mtz/game of thrones/game of thrones s05e0xxxxxxx .mp4" ;
}

static QString _video_file_size( const QString& e )
{
	return Task::await<QString>( [ & ](){

		QProcess exe ;

		exe.start( "mplayer -vo null -ao null -frames 0 \"" + e + "\"" ) ;

		exe.waitForFinished() ;

		QStringList l = QString( exe.readAllStandardOutput() ).split( '\n',QString::SkipEmptyParts ) ;

		for( const auto& it : l ){

			if( it.startsWith( " datasize: " ) ){

				float e = it.split( ' ' ).last().toFloat() ;

				e = e / ( 1024 * 1024 ) ;

				return QString::number( e,'f',2 ) + " MB" ;
			}
		}

		return QString() ;
	} ) ;
}

MainWindow::MainWindow( QWidget * parent ) : QMainWindow( parent ),m_ui( new Ui::MainWindow )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;

	m_ui->pbStartDownloading->setEnabled( false ) ;

	m_ui->lineEditSourcePath->setEnabled( false ) ;
	m_ui->lineEditDestinationPath->setEnabled( false ) ;

	connect( m_ui->pbCheckForFile,SIGNAL( clicked() ),this,SLOT( pbCheckPlugin() ) ) ;
	connect( m_ui->pbStartDownloading,SIGNAL( clicked() ),this,SLOT( pbDownLoadFile() ) ) ;
	connect( m_ui->pbQuit,SIGNAL( clicked() ),this,SLOT( pbQuit() ) ) ;
	connect( m_ui->pbKillPlugin,SIGNAL( clicked() ),this,SLOT( killPlugin() ) ) ;
	connect( m_ui->pbRefresh,SIGNAL( clicked() ),this,SLOT( pbRefresh() ) ) ;

	connect( &m_timer,SIGNAL( timeout() ),&m_files,SLOT( transfer() ),Qt::QueuedConnection ) ;

	connect( &m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		 this,SLOT( TrayIconClicked( QSystemTrayIcon::ActivationReason ) ) ) ;

	m_files.setFunction( [ this ]( float z,float o,float n ){

		if( n > o ){

			float f = z / ( 1024 * 1024 ) ;

			float e = ( n - o ) / 1024 ;

			QString g ;

			if( m_fileSize.isEmpty() ){

				g = "Data copied: " + QString::number( f,'f',2 ) + " MB" ;
			}else{
				g = "Data copied: " + QString::number( f,'f',2 ) + " MB / " + m_fileSize ;
			}

			g += "\n Transfer rate: " + QString::number( e,'f',2 ) + " KB/s" ;

			m_ui->labelStatus->setText( g ) ;
		}
	 } ) ;

	QIcon icon( ":/icon.png" ) ;

	this->setWindowIcon( icon ) ;

	this->pbCheckPlugin() ;

	m_trayIcon.setIcon( icon ) ;

	m_trayIcon.show() ;
}

MainWindow::~MainWindow()
{
	delete m_ui ;
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->hide() ;
}

QString MainWindow::getFlashPID()
{
	QProcess exe ;

	exe.start( "pidof plugin-container" ) ;

	exe.waitForFinished() ;

	return exe.readAllStandardOutput() ;
}

void MainWindow::pbCheckPlugin()
{
	QString pid = this->getFlashPID() ;

	if( pid.isEmpty() ){

		this->setDefaults() ;
		m_ui->pbCheckForFile->setEnabled( true ) ;
	}else{
		m_ui->pbKillPlugin->setEnabled( true ) ;

		pid.remove( QChar( '\n' ) ) ;

		m_ui->labelFlashPID->setText( "Plugin Handle: " + pid ) ;

		QString e = QString( "/proc/%1/fd" ).arg( pid ) ;

		QDir dir( e ) ;

		auto _readLink = [ this ]( const QString& e ){

			char buffer[ 1024 ] = { '\0' } ;

			readlink( e.toLatin1().constData(),buffer,sizeof( buffer ) ) ;

			return QString( buffer ) ;
		} ;

		for( const auto& it : dir.entryList() ){

			QString fd = e + "/" + it ;

			QString q = _readLink( fd ) ;

			if( q.startsWith( "/tmp/" ) && q.endsWith( "(deleted)" ) ){

				q.remove( " (deleted)" ) ;

				m_ui->pbStartDownloading->setEnabled( true ) ;
				m_ui->lineEditDestinationPath->setEnabled( true ) ;

				QString fileName = q ;
				fileName.remove( "/tmp/" ) ;
				fileName += ".mp4" ;

				m_ui->lineEditDestinationPath->setText( _destination_path( fileName ) ) ;

				m_ui->lineEditSourcePath->setText( q ) ;

				m_ui->pbCheckForFile->setEnabled( m_files.sourceIsNotOpen() ) ;

				if( m_files.sourceIsNotOpen() ){

					m_files.setSource( fd ) ;
				}
			}
		}
	}
}

void MainWindow::pbQuit()
{
	QCoreApplication::quit() ;
}

void MainWindow::pbDownLoadFile()
{
	if( m_files.sourceIsOpen() ){

		QString e = m_ui->lineEditDestinationPath->text() ;

		if( QFile::exists( e ) ){

			m_ui->labelStatus->setText( "Destination Path Already Taken." ) ;
		}else{
			m_files.setDestination( e ) ;

			if( m_files.destinationIsOpen() ){

				m_ui->pbStartDownloading->setEnabled( false ) ;
				m_ui->lineEditDestinationPath->setEnabled( false ) ;

				m_timer.start( 1 * 1000 ) ;
				QMetaObject::invokeMethod( &m_files,"transfer",Qt::QueuedConnection ) ;

				m_fileSize = _video_file_size( e ) ;
			}else{
				m_ui->labelStatus->setText( "Failed To Open Destination For Writing." ) ;
			}
		}
	}else{
		m_ui->labelStatus->setText( "Failed To Open Source For Reading." ) ;
	}
}

void MainWindow::TrayIconClicked( QSystemTrayIcon::ActivationReason e )
{
	Q_UNUSED( e ) ;

	if( this->isHidden() ){

		this->show() ;
	}else{
		this->hide() ;
	}
}

void MainWindow::killPlugin()
{
	QString pid = this->getFlashPID() ;

	if( !pid.isEmpty() ){

		int id = pid.toInt() ;

		if( kill( id,SIGTERM ) == 0 ){

			m_ui->pbCheckForFile->setEnabled( true ) ;

			this->pbRefresh() ;
		}
	}
}

void MainWindow::pbRefresh()
{
	m_files.reset() ;

	this->pbCheckPlugin() ;
}

void MainWindow::setDefaults()
{
	m_ui->pbKillPlugin->setEnabled( false ) ;
	m_ui->pbStartDownloading->setEnabled( false ) ;
	m_ui->lineEditDestinationPath->setText( QString() ) ;
	m_ui->lineEditSourcePath->setText( QString() ) ;
	m_ui->labelFlashPID->setText( "Plugin Handle: -1" ) ;
	m_ui->labelStatus->setText( "Status: None." ) ;
}

FilePair::FilePair()
{
	try{
		m_bufferSize = SIZE * SIZE ;
		m_buffer_1 = new char[ m_bufferSize ] ;
		m_buffer = m_buffer_1 ;

	}catch( std::bad_alloc& a ){

		qDebug() << "nnggrrrrrrr!!!!" << a.what() ;
		m_buffer_1 = nullptr ;
		m_buffer = m_buffer_0 ;
		m_bufferSize = SIZE ;

	}catch( ... ){

		/*
		 *  ??????!!!!!!
		 */
	}
}

FilePair::~FilePair()
{
	this->closeSource() ;
	this->closeDestination() ;
	delete m_buffer_1 ;
}

void FilePair::setFunction( std::function< void( float,float,float ) > function )
{
	m_function = std::move( function ) ;
}

void FilePair::setSource( const QString& e )
{
	m_source = open( e.toLatin1().constData(),O_RDONLY ) ;
}

bool FilePair::sourceIsOpen()
{
	return m_source != -1 ;
}

bool FilePair::sourceIsNotOpen()
{
	return m_source == -1 ;
}

void FilePair::closeSource()
{
	if( this->sourceIsOpen() ){

		close( m_source ) ;
		m_source = -1 ;
	}
}

bool FilePair::sourceExists()
{
	return false ;
}

void FilePair::setDestination( const QString& e )
{
	m_dest = e ;
	m_destination = open( e.toLatin1().constData(),O_WRONLY | O_CREAT,0440 ) ;
}

bool FilePair::destinationIsOpen()
{
	return m_destination != -1 ;
}

void FilePair::closeDestination()
{
	if( this->destinationIsOpen() ){

		close( m_destination ) ;
		m_destination = -1 ;
	}
}

bool FilePair::destinationExists()
{
	return QFile::exists( m_dest ) ;
}

void FilePair::transfer()
{
	if( this->sourceIsOpen() ){

		while( true ){

			auto e = read( m_source,m_buffer,m_bufferSize ) ;

			if( e < 1 ){

				m_function( m_dataSize,m_dataSize,m_dataSize ) ;
				break ;
			}else{
				m_function( m_dataSize,m_dataSize,m_dataSize + e ) ;

				m_dataSize += e ;
				write( m_destination,m_buffer,e ) ;
			}
		}
	}
}

void FilePair::reset()
{
	m_dataSize = 0 ;
	this->closeDestination() ;
	this->closeSource() ;
}
