# Pre-requisites about cmake itself
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

set (CMAKE_CXX_STANDARD 17)

# The project name and the type of project
PROJECT(ash_rand)

set(CMAKE_SKIP_INSTALL_RULES YES)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

IF (NOT CMAKE_BUILD_TYPE)
 SET(CMAKE_BUILD_TYPE "DEBUG")
ENDIF (NOT CMAKE_BUILD_TYPE)

SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -D_REENTRANT")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -D_REENTRANT -DNEBUG")

ADD_SUBDIRECTORY(src)

ADD_CUSTOM_TARGET (distclean @echo cleaning cmake files)

IF (UNIX)
  ADD_CUSTOM_COMMAND(
    COMMENT "distribution clean"
    COMMAND make ARGS clean
    COMMAND rm ARGS -rf ${CMAKE_SOURCE_DIR}/build/*

    TARGET  distclean
  )
ELSE(UNIX)
  ADD_CUSTOM_COMMAND(
    COMMENT "distclean only implemented in unix"
    TARGET  distclean
  )
ENDIF(UNIX)

