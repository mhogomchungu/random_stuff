
#include <iostream>
#include <cstdlib>

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

static char _convert_hex_to_char_buffer( char * buffer,const char * e,size_t skip,size_t block_size )
{
	size_t r = 0 ;

	while( *e ){

		*( buffer + r ) = _convert_hex_to_char( e + skip ) ;
		e += block_size ;
		r++ ;
	}

	*( buffer + r ) = '\0' ;
}

int main( int argc,char * argv[] )
{
	if( argc < 4 ){

		std::cout << "arguments: bytes_to_skip size_of_block hex_based_string\n" ;

		return 0 ;
	}else{
		int offset = atoi( *( argv + 1 ) ) ;

		int block_size = atoi( *( argv + 2 ) ) ;

		char * e = *( argv + 3 ) ;

		_convert_hex_to_char_buffer( e,e,offset,block_size ) ;

		std::cout << "\n\n" << e << "\n\n" ;

		return 0 ;
	}
}
