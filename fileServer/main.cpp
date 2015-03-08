

#include "mainwindow.h"
#include <QApplication>

#include "fileserver.h"

#include "tray_application_type.h"

int main( int argc,char * argv[] )
{
#if USE_KDE_STATUS_NOTIFIER
	KAboutData aboutData( "fileServer",0,ki18n( "qt-update-notifier" ),"1.0.0",
			      ki18n( "an http server" ),
			      KAboutData::License_GPL_V2,
			      ki18n( "(c)2014,ink Francis\nemail:mhogomchungu@gmail.com" ),
			      ki18n( "mhogomchungu@gmail.com" ),
			      "rrrr","pppp" ) ;

	KCmdLineArgs::init( argc,argv,&aboutData ) ;

	if( KUniqueApplication::start() ){

		KUniqueApplication app ;

		fileServer e ;
		e.start();

		return app.exec() ;
	}else{
		return 0 ;
	}
#else
	QApplication app( argc,argv ) ;

	fileServer e ;
	e.start() ;

	return app.exec() ;
#endif
}
