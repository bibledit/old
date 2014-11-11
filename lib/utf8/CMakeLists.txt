cmake_minimum_required (VERSION 2.8)
project (UTF8Strings)

add_library (SharedTarget SHARED String.cpp Exception.cpp)

set_target_properties(SharedTarget PROPERTIES OUTPUT_NAME UTF8Strings-1)

INSTALL(TARGETS SharedTarget DESTINATION lib${LIB_SUFFIX})

INSTALL(FILES Exception.h String.h DESTINATION include/UTF8Strings-1)

