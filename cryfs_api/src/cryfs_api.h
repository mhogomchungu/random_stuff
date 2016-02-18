

#ifndef cryfs_api_h
#define cryfs_api_h

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

typedef struct cryfs_api_handle * cryfs_api_ctx ;

int cryfs_api_init( cryfs_api_ctx * ctx ) ;

void cryfs_api_uninit( cryfs_api_ctx * ctx ) ;

typedef struct{

	const char * cipher_text_folder_path ;
	const char * plain_text_folder_path ;

	const char * mount_options ;

	const char * password ;
	size_t       password_len ;

}cryfs_api_unlock_options ;

int cryfs_api_unlock_volume( cryfs_api_ctx,const cryfs_api_unlock_options * ) ;

int cryfs_api_lock_volume( const char * plain_text_folder_path ) ;

void cryfs_api_list_mounted_volumes( cryfs_api_ctx,char * const ** list,size_t * size  ) ;

#ifdef __cplusplus
}
#endif

#endif
