cmake_minimum_required(VERSION 2.8)
project(mysockets)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=native -msse2 -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall   -O3 -march=native -msse2 -fopenmp")

# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   add_definitions(-DCOMPILEDWITHC11)
   message(STATUS "Using flag -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   add_definitions(-DCOMPILEDWITHC0X)
   message(STATUS "Using flag -std=c++0x.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()



include_directories(
${PROJECT_SOURCE_DIR}
)

set(LIBRARY_OUTPUT_PATH lib)
set(EXECUTABLE_OUTPUT_PATH bin)


add_library(${PROJECT_NAME} SHARED
client/client.cpp
log/log.cpp
proxy/CBase64.cpp
proxy/Proxy.cpp
server/server.cpp
socket/socket.cpp
)
target_link_libraries(${PROJECT_NAME}
)


add_executable( client
executable/client.cpp
)
target_link_libraries( client
${PROJECT_NAME}
)

add_executable( server
executable/server.cpp
)
target_link_libraries( server
${PROJECT_NAME}
)

add_executable( winUI 
executable/WinUI.cpp
)
target_link_libraries( winUI 
${PROJECT_NAME}
)