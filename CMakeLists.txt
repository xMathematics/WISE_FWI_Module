cmake_minimum_required (VERSION 3.13)
if (MSVC)
set (CMAKE_SYSTEM_VERSION 10.0.22000.0)
endif (MSVC)
project (FWI VERSION ${PROMETHEUS_VERSION} DESCRIPTION "FWI Calculation Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

SET(LOWLEVEL_INCLUDE_DIR "error" CACHE STRING "The path to the includes from HSS LowLevel")
SET(WTIME_INCLUDE_DIR "error" CACHE STRING "The path to the WTime libraries includes")
SET(MULTITHREAD_INCLUDE_DIR "error" CACHE STRING "The path to the includes from HSS Multithread")
SET(MATH_INCLUDE_DIR "error" CACHE STRING "The path to HSS_Math include files")
SET(THIRD_PARTY_INCLUDE_DIR "error" CACHE STRING "The path to third party include files")
SET(BOOST_INCLUDE_DIR "error" CACHE STRING "The path to boost include files")
SET(ERROR_CALC_INCLUDE_DIR "error" CACHE STRING "The path to the error calc include files")

find_library(FOUND_WTIME_LIBRARY_PATH NAMES WTime REQUIRED PATHS ${LOCAL_LIBRARY_DIR})

if (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -DPROTOBUF_USE_DLLS -DBOOST_ALL_DYN_LINK -D_CRT_SECURE_NO_WARNINGS /Zc:__cplusplus")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD -D_AFXDLL")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd -D_AFXDLL /W4")
else ()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
endif (MSVC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_NO_MFC")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")

add_library(fwi SHARED
    cpp/fwi.h
    cpp/fwi.cpp
    cpp/CWFGM_FWI.cpp
    include/FwiCom.h
)

target_include_directories(fwi
    PUBLIC ${WTIME_INCLUDE_DIR}
    PUBLIC ${MATH_INCLUDE_DIR}
    PUBLIC ${LOWLEVEL_INCLUDE_DIR}
    PUBLIC ${ERROR_CALC_INCLUDE_DIR}
    PUBLIC ${BOOST_INCLUDE_DIR}
    PUBLIC ${MULTITHREAD_INCLUDE_DIR}
    PUBLIC ${THIRD_PARTY_INCLUDE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(fwi PROPERTIES VERSION ${CMAKE_PROJECT_VERSION})
set_target_properties(fwi PROPERTIES SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR})
set_target_properties(fwi PROPERTIES DEFINE_SYMBOL "FWI_EXPORTS")

set_target_properties(fwi PROPERTIES
    PUBLIC_HEADER include/CWFGM_FWI.h
)

target_link_libraries(fwi ${FOUND_WTIME_LIBRARY_PATH})
if (MSVC)
else ()
target_link_libraries(fwi -lstdc++fs)
endif (MSVC)
