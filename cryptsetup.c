
#include <libcryptsetup.h>
#include <string.h>
#include <stdio.h>

static const char * header = "/home/mtz/header.img" ;
static const char * device = "/home/mtz/veraCrypt.img" ;

static const size_t key_size = 64 ;

static const char * password = "xxx" ;
static const size_t password_size = 3 ;

static const char * mapper_name = "vera_mapper" ;

static char buffer[] = "f044e7617a9ceccdc5e0acafc8326d04b707226268dceb17bc37de8eb88a72627b9eef26bf0330b4cab99f81397b97804a0b35a1b1571e1ede140c1f872fe220" ;

static char _convert_hex_to_binary_0( const char * e )
{
	char a = *e ;

	if( a >= 'A' && a <= 'F' ){

		return a - 'A' + 10 ;

	}else if( a >= 'a' && a <= 'f' ){

		return a - 'a' + 10 ;
	}else{
		return a - '0' ;
	}
}

static char _convert_hex_to_char( const char * e )
{
	return _convert_hex_to_binary_0( e ) * 16 + _convert_hex_to_binary_0( e + 1 ) ;
}

static void _convert_hex_to_binary( char * buffer,const char * e,size_t skip,size_t block_size )
{
	size_t r = 0 ;

	while( *e ){

		*( buffer + r ) = _convert_hex_to_char( e + skip ) ;
		e += block_size ;
		r++ ;
	}

	*( buffer + r ) = '\0' ;
}

static const char * _master_key()
{
	_convert_hex_to_binary( buffer,buffer,0,2 ) ;

	return buffer ;
}

/*
 * This function creates a LUKS header with a master key specified in "buffer"
 */
int main_0()
{
	struct crypt_device * cd ;
	struct crypt_params_luks1 params ;

	memset( &params,'\0',sizeof( struct crypt_params_luks1 ) ) ;

	crypt_init( &cd,header ) ;

	params.hash = "sha1" ;
	params.data_alignment = 0 ;
	params.data_device = device ;

	crypt_format( cd,CRYPT_LUKS1,"aes","xts-plain64",NULL,_master_key(),key_size,&params ) ;

	crypt_keyslot_add_by_volume_key( cd,CRYPT_ANY_SLOT,NULL,key_size,password,password_size ) ;

	return 0 ;
}

/*
 * This function reads a master key from a LUKS header and opens a plain dm-crypt volume using
 * the master key.
 */

int main_1()
{
	struct crypt_device * cd = NULL ;

	crypt_init( &cd,header ) ;

	crypt_load( cd,NULL,NULL ) ;

	struct crypt_params_plain plain ;

	plain.hash   = NULL ;
	plain.offset = 256 ;
	plain.skip   = 256 ;
	plain.size   = 0 ;

	char key[ 1024 ] ;

	size_t size = 1024 ;

	crypt_volume_key_get( cd,CRYPT_ANY_SLOT,key,&size,password,password_size ) ;

	crypt_free( cd ) ;

	crypt_init( &cd,device ) ;

	crypt_format( cd,CRYPT_PLAIN,"aes","xts-plain64",NULL,NULL,size,&plain ) ;

	crypt_activate_by_volume_key( cd,mapper_name,key,size,0 ) ;

	return 0 ;
}

int main()
{
	//return main_0() ;

	return main_1() ;
}
