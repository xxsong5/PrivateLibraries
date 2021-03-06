cmake_minimum_required(VERSION 2.8)
project(Viewer)

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

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

find_package(OpenCV 3.0 QUIET)
if(NOT OpenCV_FOUND)
   find_package(OpenCV 2.4.3 QUIET)
   if(NOT OpenCV_FOUND)
      message(FATAL_ERROR "OpenCV > 2.4.3 not found.")
   endif()
endif()

find_package(Eigen3 3.1.0 REQUIRED)
find_package(PCL 1.2 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)


include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include
${EIGEN3_INCLUDE_DIR}
${PCL_INCLUDE_DIRS}
${Boost_INCLUDE_DIRS}
)



add_executable(${PROJECT_NAME}
main.cpp
3DViewer_PCL.cpp
)
target_link_libraries(${PROJECT_NAME}
${OpenCV_LIBS}
${EIGEN3_LIBS}
${PCL_LIBRARIES}
${Boost_LIBRARIES}
)


add_executable( readPCD
main_file.cpp
3DViewer_PCL.cpp
)
target_link_libraries(readPCD
${OpenCV_LIBS}
${EIGEN3_LIBS}
${PCL_LIBRARIES}
${Boost_LIBRARIES}
)


add_executable( readPCD_filter
main_file_and_filter.cpp
3DViewer_PCL.cpp
)
target_link_libraries(readPCD_filter
${OpenCV_LIBS}
${EIGEN3_LIBS}
${PCL_LIBRARIES}
${Boost_LIBRARIES}
)
