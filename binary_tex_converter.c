

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

int main(void)
{
	unsigned char buffer[] = { 244,255,13,15,250} ;
	char result[ 2 * sizeof( buffer ) + 1 ] = { '\0' } ;

	int s = sizeof( buffer ) ;

	_convert_binary_to_hex( buffer,s,result ) ;

	puts( result ) ;

	return 0 ;
}
