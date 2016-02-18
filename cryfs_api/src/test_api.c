 

#include "cryfs_api.h"

#include <string.h>
#include <stdio.h>

int main( int argc,char * argv[] )
{
	if( argc < 2 ){

		puts( "atleast one argument is required" ) ;
		return 1 ;
	}

	cryfs_api_ctx ctx ;

	if( cryfs_api_init( &ctx ) ){

		puts( "failed to initialize context" ) ;
		return 1 ;
	}

	cryfs_api_unlock_options opts ;

	memset( &opts,'\0',sizeof( opts ) ) ;

	opts.mount_options           = "-o rw" ;
	opts.cipher_text_folder_path = "/home/mtz/cipher" ;
	opts.plain_text_folder_path  = "/home/mtz/plain" ;
	opts.password                = "xxx" ;
	opts.password_len            = strlen( opts.password ) ;

	const char * opt = argv[ 1 ] ;

	int r = 1 ;

	if( strcmp( opt,"unlock" ) == 0 ){

		r = cryfs_api_unlock_volume( ctx,&opts ) ;

		if( r != 0 ){

			puts( "failed to unlock volume" ) ;
		}else{
			puts( "volume unlocked successfully" ) ;
		}

	}else if( strcmp( opt,"lock" ) == 0 ){

		r = cryfs_api_lock_volume( opts.plain_text_folder_path ) ;

	}else if( strcmp( opt,"print" ) == 0 ){

		char * const * e ;
		size_t s ;

		cryfs_api_list_mounted_volumes( ctx,&e,&s ) ;

		for( size_t i = 0 ; i < s ; i++ ){

			puts( *( e + i ) ) ;
		}
	}else{
		puts( "unknown option encountered" ) ;
		r = 1 ;
	}

	cryfs_api_uninit( &ctx ) ;

	return r ;
}
