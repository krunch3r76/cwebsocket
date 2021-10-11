#wsmessage
cmake_policy(SET CMP0091 NEW)
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
cmake_minimum_required(VERSION 3.15)

project(wsmessage LANGUAGES C)
set(TGT ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
add_library(${TGT} )
target_include_directories(${TGT} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include/)
target_include_directories(${TGT} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include/kwebsocket)

target_sources(${TGT} PRIVATE 
	# ${TGT}.c
	parseframe.c
	wsframe.c
	wsmask.c
	)

#include($ENV{K1_CMAKE_INCLUDES_PATH}/CMakeLists.txt)

# # #		mixin			# # #
#h__mixin(${TGT}
	#	${CMAKE_CURRENT_LIST_DIR}/HTTPMessage
#	${DIR_FLTK_HELPERS}/WidgetOperations WidgetOperations
#)
# # #		/mixin			# # #



# # #	 add internals		# # #
#[[
set(__add_internal_scope PUBLIC)
set(internals
	#	bb	SOCKETinit
	#	garage/mvc/v1/view	view
	#	2_a	DebugLog
	#	2_b	KString
	)
__add_internal(${internals})
]]
# # #	 /add internals		# # #
