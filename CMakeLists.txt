cmake_minimum_required(VERSION 2.8)
PROJECT(multiDimen)

FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
FIND_PACKAGE(OpenMP REQUIRED)

if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

if(Boost_FOUND)
  INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

  ADD_EXECUTABLE(${PROJECT_NAME} multiDimen.cpp ./filterbank.h ./filterbank.cpp
  ./imgCollection.cpp ./imgCollection.h ./modelBuild.cpp ./modelBuild.h
./imgFunctions.cpp ./imgFunctions.h ./dictCreation.cpp ./dictCreation.h
./novelImgTest.cpp ./novelImgTest.h)

  TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${Boost_FILESYSTEM_LIBRARY}
    ${BOOST_SYSTEM_LIBRARY}
    ${Boost_LIBRARIES}
    )
  ADD_DEFINITIONS(-std=c++11)
endif()
