
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTCORE TRUE )
SET( QT_USE_QTGUI TRUE )

find_package(Qt4 REQUIRED)
SET(QT_USE_QTCORE TRUE)
SET(QT_USE_QTGUI TRUE)

add_definitions(-I${Qt4_INCLUDE_DIR})
add_definitions(-I${QT_HEADERS_DIR})

INCLUDE(${QT_USE_FILE})

INCLUDE(${QT_USE_FILE})

QT4_WRAP_CPP( MOC fileserver.h statusicon.h )
#QT4_WRAP_UI( UI src/walletmanager.ui src/addaccount.ui src/configurationoptionsdialog.ui )
QT4_ADD_RESOURCES( TRAY_RC_SRCS icons.qrc )

add_executable( fileServer main.cpp fileserver.cpp statusicon.cpp ${MOC} ${TRAY_RC_SRCS} )

TARGET_LINK_LIBRARIES( fileServer ${QT_LIBRARIES} -lkdeui)
