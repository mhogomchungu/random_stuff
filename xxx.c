
#include <zuluCrypt/libzuluCryptPluginManager.h>
#include <string.h>
#include <stdio.h>

typedef struct{
	size_t size ;
	const char * key ;
}Key;

static void getKey( const char * path,Key * k )
{
	size_t s = strlen( path ) ;

	const char * e = path + s - 1 ;

	size_t r = 0 ;

	char c ;

	if( s > 1 ){

		while( 1 ){

			if( e == path && *e != '/' ){

				r++ ;
				break ;
			}else{

				c = *e ;

				e-- ;

				if( c == '/' ){

					break ;
				}else{
					r++ ;
				}
			}
		}

		k->size = r ;
		k->key  = path + s - r ;
	}
}

int main( int argc,char * argv[] )
{
	const char * key  = argv[ 0 ] ;
	const char * addr = argv[ 3 ] ;

	Key k = { 0,NULL } ;

	getKey( key,&k ) ;

	void * handle = zuluCryptPluginManagerOpenConnection( addr ) ;

	if( handle ){
		zuluCryptPluginManagerSendKey( handle,k.key,k.size ) ;
		zuluCryptPluginManagerCloseConnection( handle ) ;
		return 0 ;
	}else{
		return 1 ;
	}
}
