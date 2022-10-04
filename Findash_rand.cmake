#edit the following line to add the librarie's header files
FIND_PATH(ash_rand_INCLUDE_DIR ash_rand.h /usr/include/iridrivers /usr/local/include/iridrivers)

FIND_LIBRARY(ash_rand_LIBRARY
    NAMES ash_rand
    PATHS /usr/lib /usr/local/lib /usr/local/lib/iridrivers) 

IF (ash_rand_INCLUDE_DIR AND ash_rand_LIBRARY)
   SET(ash_rand_FOUND TRUE)
ENDIF (ash_rand_INCLUDE_DIR AND ash_rand_LIBRARY)

IF (ash_rand_FOUND)
   IF (NOT ash_rand_FIND_QUIETLY)
      MESSAGE(STATUS "Found ash_rand: ${ash_rand_LIBRARY}")
   ENDIF (NOT ash_rand_FIND_QUIETLY)
ELSE (ash_rand_FOUND)
   IF (ash_rand_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find ash_rand")
   ENDIF (ash_rand_FIND_REQUIRED)
ENDIF (ash_rand_FOUND)

