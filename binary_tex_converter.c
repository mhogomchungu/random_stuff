

#include<stdio.h>
#include<string.h>

static void _convert_binary_to_hex( const unsigned char * source,size_t source_size,char * result )
{
	unsigned char e ;
	unsigned char f ;

	size_t k ;

	for( k = 0 ; k < source_size ; k++ ){

		e = *( source + k ) ;

		f = e / 16 ;

		if( f < 10 ){

			*( result + 2 * k ) = f + 48 ;
		}else{
			*( result + 2 * k ) = f + 87 ;
		}

		f = e % 16 ;

		if( f < 10 ){

			*( result + 2 * k + 1 ) = f + 48 ;
		}else{
			*( result + 2 * k + 1 ) = f + 87 ;
		}
	}
}

static char _convert_binary_to_hex_0( const char * e )
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
	return _convert_binary_to_hex_0( e ) * 16 + _convert_binary_to_hex_0( e + 1 ) ;
}

int main(void)
{
	unsigned char buffer[] = { 244,255,13,15,250 } ;
	char result[ 2 * sizeof( buffer ) + 1 ] = { '\0' } ;

	int s = sizeof( buffer ) ;

	_convert_binary_to_hex( buffer,s,result ) ;

	puts( result ) ;

	return 0 ;
}
