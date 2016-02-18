
#include "cryfs_api.h"

#include "string/String.h"
#include "string/StringList.h"

#include "process/process.h"

#include <sys/stat.h>

#include <stdio.h>

#include <unistd.h>

struct cryfs_api_handle
{
	const char * exe ;
	stringList_t mounted_list ;
	char * const * list ;
	size_t list_size ;
} ;

int cryfs_api_init( cryfs_api_ctx * ctx )
{
	struct stat st ;

	const char * exe ;

	*ctx = NULL ;

	if( stat( "/usr/bin/cryfs",&st ) == 0 ){

		exe = "/usr/bin/cryfs" ;

	}else if( stat( "/usr/local/bin/cryfs",&st ) == 0 ){

		exe = "/usr/local/bin/cryfs" ;
	}else{
		return 1 ;
	}

	cryfs_api_ctx h = malloc( sizeof( struct cryfs_api_handle ) ) ;

	memset( h,'\0',sizeof( struct cryfs_api_handle ) ) ;

	h->exe = exe ;

	*ctx = h ;

	return 0 ;
}

void cryfs_api_uninit( cryfs_api_ctx * ctx )
{
	cryfs_api_ctx h = *ctx ;

	if( h ){

		StringListDelete( &h->mounted_list ) ;
		free( ( void * ) h->list ) ;
		free( h ) ;
		*ctx = NULL ;
	}
}

static void _add_entry( stringList_t tmp,stringList_t * stx,char * const ** entry,size_t * entry_len )
{
	string_t st ;

	u_int64_t e ;

	StringListStringArray_1( entry,entry_len,tmp ) ;

	const char * device       = *( *entry + *entry_len - 2 ) ;
	const char * mountPoint   = *( *entry + 4 ) ;
	const char * fileSystem   = *( *entry + *entry_len - 3 ) ;
	const char * mountOptions = *( *entry + 5 ) ;

	if( StringsAreEqual( fileSystem,"fuse.cryfs" ) ){

		st = StringListStringAt( tmp,*entry_len - 2 ) ;

		StringReset( st ) ;

		e = StringJenkinsOneAtATimeHash( mountPoint ) ;

		device = StringAppendInt( st,e ) ;

		st = String_1( device," ",mountPoint," ",mountOptions,NULL ) ;

		StringListAppendString_1( stx,&st ) ;
	}
}

static stringList_t _volumeList()
{
	char * const * entry = NULL ;

	size_t entry_len = 0 ;

	stringList_t tmp ;
	stringList_t stx = StringListVoid ;
	stringList_t stl ;

	StringListIterator it  ;
	StringListIterator end ;

	string_t st = StringGetFromVirtualFile( "/proc/self/mountinfo" ) ;

	stl = StringListStringSplit( st,'\n' ) ;

	StringDelete( &st ) ;

	StringListGetIterators( stl,&it,&end ) ;

	while( it != end ){

		tmp = StringListStringSplit( *it,' ' ) ;

		it++ ;

		_add_entry( tmp,&stx,&entry,&entry_len ) ;

		StringListDelete( &tmp ) ;
	}

	StringFree( entry ) ;

	StringListDelete( &stl ) ;

	return stx ;
}

static void _update_mounted_list( cryfs_api_ctx ctx )
{
	StringListDelete( &ctx->mounted_list ) ;

	ctx->mounted_list = _volumeList() ;
}

void cryfs_api_list_mounted_volumes( cryfs_api_ctx ctx,char * const ** list,size_t * size )
{
	_update_mounted_list( ctx ) ;

	StringListStringArray_1( &ctx->list,&ctx->list_size,ctx->mounted_list ) ;

	*list = ctx->list ;
	*size = ctx->list_size ;
}

static int _volume_unlocked( cryfs_api_ctx ctx,const char * path )
{
	_update_mounted_list( ctx ) ;

	string_t st = String_1( " ",path," ",NULL ) ;

	int r = StringListHasSequence( ctx->mounted_list,StringContent( st ) ) ;

	StringDelete( &st ) ;

	return r ;
}

static int _cryfs_api_unlock_volume( cryfs_api_ctx ctx,const cryfs_api_unlock_options * opts )
{
	const char * m_options ;

	if( opts->mount_options == NULL || opts->mount_options[ 0 ] == '\0' ){

		m_options = "-o rw" ;
	}else{
		m_options = opts->mount_options ;
	}

	process_t p = Process( ctx->exe,
			       opts->cipher_text_folder_path,
			       opts->plain_text_folder_path,
			       "--",
			       m_options,
			       NULL ) ;

	ProcessStart( p ) ;

	ProcessWrite( p,opts->password,opts->password_len ) ;

	ProcessWrite( p,"\n",1 ) ;

	//ProcessCloseStdWrite( p ) ;

	return ProcessWaitUntilFinished( &p ) ;
}

int cryfs_api_unlock_volume( cryfs_api_ctx ctx,const cryfs_api_unlock_options * opts )
{
	struct stat st ;

	if( stat( opts->cipher_text_folder_path,&st ) != 0 ){

		return 1 ;
	}

	if( stat( opts->plain_text_folder_path,&st ) != 0 ){

		return 1 ;
	}

	for( int i = 0 ; i < 5 ; i++ ){

		printf( "unlocking attempt: %d\n",i + 1 ) ;

		if( _cryfs_api_unlock_volume( ctx,opts ) == 0 ){

			/*
			 * Do not know why yet but sometimes,cryfs reports success while the volume is
			 * not unlocked,problem is probably on my side since QProcess doesnt show this behavior.
			 * Will look more into it later on.
			 */

			if( _volume_unlocked( ctx,opts->plain_text_folder_path ) != -1 ){

				return 0 ;
			}
		}else{
			return 1 ;
		}
	}

	return 1 ;
}

int cryfs_api_lock_volume( const char * plain_text_folder_path )
{
	return ProcessExecute( "/usr/bin/fusermount","-u",plain_text_folder_path,NULL ) ;
}
