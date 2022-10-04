# driver source files
SET(sources ash_rand.cpp)
# locate the necessary dependencies
# add the necessary include directories
INCLUDE_DIRECTORIES(../include)
# create the shared library
ADD_LIBRARY(ash_rand SHARED ${sources})
# link necessary libraries
INSTALL(TARGETS ash_rand
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
INSTALL(DIRECTORY ../include/ash_rand DESTINATION include)
INSTALL(FILES ../Findash_rand.cmake DESTINATION ${CMAKE_ROOT}/Modules/)
ADD_SUBDIRECTORY(examples)

