cmake_minimum_required( VERSION 3.23 )
project( ESPBasicWifi )

set( CMAKE_CXX_STANDARD 14 )

add_executable( ESPBasicWifi
		ESPBasicWifi.ino )

set_target_properties( ESPBasicWifi PROPERTIES LINKER_LANGUAGE CXX )
