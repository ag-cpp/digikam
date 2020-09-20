# Some useful macros to manage compiler rules
#
# For a complete list of GCC and Clang compiler warnings options available
# depending of compiler version, check this url:
# https://github.com/Barro/compiler-warnings
#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# -------------------------------------------------------------------------

# Macro to remove GCC compilation option.

macro(REMOVE_GCC_COMPILER_WARNINGS COMPILATION_OPTION)

    if(CMAKE_COMPILER_IS_GNUCXX)

        message(STATUS "Remove GCC compiler option ${COMPILATION_OPTION} from ${CMAKE_CURRENT_SOURCE_DIR}")

        while(CMAKE_CXX_FLAGS MATCHES ${COMPILATION_OPTION})

            string(REPLACE ${COMPILATION_OPTION} "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

        endwhile()

    endif()

endmacro()

# -------------------------------------------------------------------------

# Macro to remove CLANG compilation option.

macro(REMOVE_CLANG_COMPILER_WARNINGS COMPILATION_OPTION)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

        message(STATUS "Remove CLANG compiler option ${COMPILATION_OPTION} from ${CMAKE_CURRENT_SOURCE_DIR}")

        while(CMAKE_CXX_FLAGS MATCHES ${COMPILATION_OPTION})

            string(REPLACE ${COMPILATION_OPTION} "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

        endwhile()

    endif()

endmacro()

# -------------------------------------------------------------------------

# Macro to remove GCC or CLANG compilation option which use the same syntax.

macro(REMOVE_COMPILER_WARNINGS COMPILATION_OPTION)

    REMOVE_GCC_COMPILER_WARNINGS(${COMPILATION_OPTION})
    REMOVE_CLANG_COMPILER_WARNINGS(${COMPILATION_OPTION})

endmacro()

# -------------------------------------------------------------------------

# Macro to disable GCC compilation option using -Wno* syntax.

macro(DISABLE_GCC_COMPILER_WARNINGS COMPILER_VERSION NO_COMPILATION_OPTION)

    if(CMAKE_COMPILER_IS_GNUCXX)

        exec_program(${CMAKE_CXX_COMPILER} ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion OUTPUT_VARIABLE GCC_VERSION)

        if (${GCC_VERSION} VERSION_GREATER ${COMPILER_VERSION})

            message(STATUS "Disable GCC compiler option ${NO_COMPILATION_OPTION} from ${CMAKE_CURRENT_SOURCE_DIR}")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NO_COMPILATION_OPTION}")

        endif()

    endif()

endmacro()

# -------------------------------------------------------------------------

# Macro to disable CLANG compilation option using -Wno* syntax.

macro(DISABLE_CLANG_COMPILER_WARNINGS COMPILER_VERSION NO_COMPILATION_OPTION)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

        exec_program(${CMAKE_CXX_COMPILER} ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion OUTPUT_VARIABLE CLANG_VERSION)

        if (${CLANG_VERSION} VERSION_GREATER ${COMPILER_VERSION})

            message(STATUS "Disable CLANG compiler option ${NO_COMPILATION_OPTION} from ${CMAKE_CURRENT_SOURCE_DIR}")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NO_COMPILATION_OPTION}")

        endif()

    endif()

endmacro()

# -------------------------------------------------------------------------

# Macro to enable ASAN and UBSAN sanitizers.

macro(ENABLE_SANITIZERS)

    # ASAN is available in gcc from 4.8 and UBSAN from 4.9
    # ASAN is available in clang from 3.1 and UBSAN from 3.3
    # UBSAN is not fatal by default, instead it only prints runtime errors to stderr
    # => make it fatal with -fno-sanitize-recover (gcc) or -fno-sanitize-recover=all (clang)
    # add -fno-omit-frame-pointer for better stack traces

    if(COMPILER_IS_GCC)

        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)

            set(SANITIZER_FLAGS "-fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover")

        elseif(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8)

            set(SANITIZER_FLAGS "-fno-omit-frame-pointer -fsanitize=address")

        endif()

    elseif(COMPILER_IS_CLANG )

        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)

            set(SANITIZER_FLAGS "-fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=all")

        elseif(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.4)

            set(SANITIZER_FLAGS "-fno-omit-frame-pointer -fsanitize=address,undefined")

        elseif(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.1)

            set(SANITIZER_FLAGS "-fno-omit-frame-pointer -fsanitize=address")

        endif()

    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${SANITIZER_FLAGS}")

endmacro()
