# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# set(COMPILER_PREFIX "i686-w64-mingw32")
set(COMPILER_PREFIX "x86_64-w64-mingw32")

find_program(CMAKE_RC_COMPILER NAMES ${COMPILER_PREFIX}-windres)
find_program(CMAKE_C_COMPILER NAMES ${COMPILER_PREFIX}-gcc)
find_program(CMAKE_CXX_COMPILER NAMES ${COMPILER_PREFIX}-g++)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /usr/${COMPILER_PREFIX} ${USER_ROOT_PATH})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
