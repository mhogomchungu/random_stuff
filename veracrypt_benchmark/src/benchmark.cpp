/*
 * copyright: 2015
 * name : Francis Banyikwa
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "benchmark.h"
#include "task.h"

#include <QDebug>
#include <QMetaObject>

#include <cstdio>
#include <cstring>

#include <QProcess>
#include <QCoreApplication>

#define CRYPTSETUP 0

#include <libcryptsetup.h>

benchmark::benchmark()
{
	connect( &m_timer,SIGNAL( timeout() ),this,SLOT( update() ) ) ;
}

void benchmark::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void benchmark::update()
{
	if( m_time < 60 ){
		printf( "\r%s\telapsed time: %.0f seconds",m_msg.toLatin1().constData(),m_time ) ;
	}else{
		printf( "\r%s\telapsed time: %.2f minutes",m_msg.toLatin1().constData(),m_time / 60 ) ;
	}
	fflush( stdout ) ;
	m_time++ ;
}

void benchmark::run()
{
	this->benchmarkCryptsetup() ;

	this->benchmarkVeraCrypt() ;

	QCoreApplication::quit() ;
}

void benchmark::startTimer( const char * m )
{
	printf( "\n" ) ;
	fflush( stdout ) ;

	m_msg = m ;
	m_time = 0 ;
	m_timer.start( 1000 ) ;
}

void benchmark::stopTimer()
{
	m_timer.stop() ;
	printf( "\n" ) ;
	fflush( stdout ) ;
}

void benchmark::benchmarkCryptsetup()
{
#if CRYPTSETUP
	auto _benchmark = []( const char * key,const char * path ){

		Task::await( [ & ](){

			struct crypt_device * cd = nullptr ;
			struct crypt_params_tcrypt params ;

			if( crypt_init( &cd,path ) == 0 ){

				memset( &params,'\0',sizeof( struct crypt_params_tcrypt ) ) ;

				params.passphrase       = key ;
				params.passphrase_size  = strlen( key ) ;

				params.flags = CRYPT_TCRYPT_VERA_MODES ;

				if( crypt_load( cd,CRYPT_TCRYPT,&params ) == 0 ){

					crypt_free( cd ) ;
				}else{
					params.flags |= CRYPT_TCRYPT_HIDDEN_HEADER ;

					if( crypt_load( cd,CRYPT_TCRYPT,&params ) == 0 ){

						crypt_free( cd ) ;
					}
				}
			}
		} ) ;
	} ;

	puts( "\nbenchmarking cryptsetup" ) ;

	this->startTimer( "testing sha512(normal)   " ) ;
	_benchmark( "xxx","sha512.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing sha512(hidden)   " ) ;
	_benchmark( "qqq","sha512.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing whirlpool(normal)" ) ;
	_benchmark( "xxx","whirlpool.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing whirlpool(hidden)" ) ;
	_benchmark( "qqq","whirlpool.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing sha256(normal)   " ) ;
	_benchmark( "xxx","sha256.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing sha256(hidden)   " ) ;
	_benchmark( "qqq","sha256.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing ripemd160(normal)" ) ;
	_benchmark( "xxx","ripemd160.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing ripemdi60(hidden)" ) ;
	_benchmark( "qqq","ripemd160.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing wrong password   " ) ;
	_benchmark( "zzz","ripemd160.img" ) ;
	this->stopTimer() ;
#endif
}

void benchmark::benchmarkVeraCrypt()
{
	auto _benchmark = []( const char * key,const char * path ){

		Task::await( [ & ](){

			QProcess exe ;

			exe.start( QString( "veracrypt -p %1 %2" ).arg( key,path ) ) ;

			if( exe.waitForFinished( -1 ) ){

				QProcess e ;

				e.start( "veracrypt -d /media/veracrypt1 " ) ;

				e.waitForFinished() ;
			}

			exe.waitForFinished() ;
		} ) ;
	} ;

	puts( "\nbenchmarking veracrypt" ) ;

	this->startTimer( "testing sha512(normal)   " ) ;
	_benchmark( "xxx","sha512.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing sha512(hidden)   " ) ;
	_benchmark( "qqq","sha512.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing whirlpool(normal)" ) ;
	_benchmark( "xxx","whirlpool.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing whirlpool(hidden)" ) ;
	_benchmark( "qqq","whirlpool.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing sha256(normal)   " ) ;
	_benchmark( "xxx","sha256.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing sha256(hidden)   " ) ;
	_benchmark( "qqq","sha256.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing ripemd160(normal)" ) ;
	_benchmark( "xxx","ripemd160.img" ) ;
	this->stopTimer() ;

	this->startTimer( "testing ripemdi60(hidden)" ) ;
	_benchmark( "qqq","ripemd160.img" ) ;
	this->stopTimer() ;

	//This test is skipped because its hard to test as VeraCrypt seems to generate a GUI
	//prompt on failed attempt
	//this->startTimer( "testing wrong password   " ) ;
	//_benchmark( "zzz","ripemd160.img" ) ;
	//this->stopTimer() ;
}
