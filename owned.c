

#include <unistd.h>

int main( int argc,char * argv[] ) {

	if( argc < 2 ){
		return 1 ;
	}else{
		const char * e = *( argv + 1 ) ;
		setuid( 0 ) ;
		seteuid( 0 ) ;
		if( execl( e,NULL ) == -1 ){
			perror( "failed: " ) ;
			return 1 ;
		}else{
			return 0 ;
		}
	}
}
