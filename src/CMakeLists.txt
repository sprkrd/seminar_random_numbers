# driver source files
SET(sources ash_rand.cpp)
# locate the necessary dependencies
# add the necessary include directories
INCLUDE_DIRECTORIES(../include)
# create the shared library
ADD_LIBRARY(ash_rand SHARED ${sources})
# link necessary libraries
ADD_SUBDIRECTORY(examples)

