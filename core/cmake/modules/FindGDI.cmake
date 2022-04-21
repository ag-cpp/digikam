# Finds the GDI (Graphics Device Interface) on Windows (https://en.wikipedia.org/wiki/Graphics_Device_Interface)
#
# This will define the following variables:
#
# GDI_FOUND        - system has GDI
# GDI_LIBRARIES    - libraries you need to link to
#
# Copyright (C) 2005 - 2019 by Inria. All rights reserved.
#
# Redistribution and use is allowed according to the terms of the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(GDI_FOUND "NO")

IF(WIN32)
  IF(MINGW)
    FIND_LIBRARY(GDI_LIBRARY gdi32
                 "C:/MinGW/lib"
                 "C:/mingw/mingw/lib"
                 "$ENV{MINGW_DIR}/lib"
                 "$ENV{MINGW_DIR}/mingw/lib"
                 DOC "Where can the GDI (Graphics Device Interface) library be found"
                 NO_DEFAULT_PATH
                )
  ELSE(MINGW)
    IF(CMAKE_CL_64)
      # Generic path search
      file(GLOB _gdi_search_path "C:/Program Files (x86)/Windows Kits/*/Lib/*/um/x64")
      list(REVERSE _gdi_search_path) # to search first in the more recent SDK
      FIND_LIBRARY(GDI_LIBRARY gdi32
                   "$ENV{WINSDK_DIR}/Lib/x64"
                   "$ENV{WINSDK_HOME}/Lib/x64"
                   "$ENV{DXSDK_DIR}/Lib/x64"
                   "C:/Program Files/Microsoft SDKs/Windows/v6.0/Lib/x64"
                   "C:/Program Files/Microsoft SDKs/Windows/v6.0a/Lib/x64"
                   "C:/Program Files/Microsoft SDKs/Windows/v6.1/Lib/x64"
                   "C:/Program Files/Microsoft SDKs/Windows/v7.0A/Lib/x64"
                   "C:/Program Files/Microsoft SDKs/Windows/v7.1/Lib/x64"
                   "C:/Program Files/Microsoft SDKs/Windows/v7.1A/Lib/x64"
                   "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A/Lib/x64"
                   "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib/x64"
                   ${_gdi_search_path}
                   "C:/Program Files/Microsoft Platform SDK/Lib/x64"
                   "C:/DXSDK/Include/Lib/x64"
                   DOC "Where can the GDI (Graphics Device Interface) library be found"
                  )

      # Specific path search for Visual Studio .NET 2003
      IF(MSVC71)
        IF(NOT GDI_LIBRARY)
          FIND_LIBRARY(GDI_LIBRARY gdi32
                       "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
                      )
        ENDIF(NOT GDI_LIBRARY)
      ENDIF(MSVC71)

      # Specific path search for Visual Studio 2005
      IF(MSVC80)
        IF(NOT GDI_LIBRARY)
          FIND_LIBRARY(GDI_LIBRARY gdi32
                       "C:/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK/Lib/AMD64"         )
        ENDIF(NOT GDI_LIBRARY)
      ENDIF(MSVC80)

      # Specific path search for Visual Studio 2008
      IF(MSVC90)
        IF(NOT GDI_LIBRARY)
          FIND_LIBRARY(GDI_LIBRARY gdi32
                       "C:/Program Files/Microsoft Visual Studio 9/VC/PlatformSDK/Lib/AMD64"         )
        ENDIF(NOT GDI_LIBRARY)
      ENDIF(MSVC90)

    ELSE(CMAKE_CL_64)
      # Generic path search
      file(GLOB _gdi_search_path "C:/Program Files (x86)/Windows Kits/*/Lib/*/um/x86")
      list(REVERSE _gdi_search_path) # to search first in the more recent SDK
      FIND_LIBRARY(GDI_LIBRARY gdi32
                   "$ENV{WINSDK_DIR}/Lib"
                   "$ENV{WINSDK_HOME}/Lib"
                   "$ENV{DXSDK_DIR}/Lib"
                   "C:/Program Files/Microsoft SDKs/Windows/v6.0a/Lib"
                   "C:/Program Files/Microsoft SDKs/Windows/v6.0/Lib"
                   "C:/Program Files/Microsoft SDKs/Windows/v6.1/Lib"
                   "C:/Program Files/Microsoft SDKs/Windows/v7.0A/Lib"
                   "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A/Lib"
                   "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1/Lib"
                   "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib"
                   "C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x86"
                   "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x86"
                   "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.15063.0/um/x86"
                   "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.14393.0/um/x86"
                   ${_gdi_search_path}
                   "C:/Program Files/Microsoft Platform SDK/Lib"
                   "C:/DXSDK/Include/Lib"
                   DOC "Where can the GDI (Graphics Device Interface) library be found"
                  )

      # Specific path search for Visual Studio .NET 2003
      IF(MSVC71)
        IF(NOT GDI_LIBRARY)
          FIND_LIBRARY(GDI_LIBRARY gdi32
                       "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
                      )
        ENDIF(NOT GDI_LIBRARY)
      ENDIF(MSVC71)

      # Specific path search for Visual Studio 2005
      IF(MSVC80)
        IF(NOT GDI_LIBRARY)
          FIND_LIBRARY(GDI_LIBRARY gdi32
                       "C:/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK/Lib"
                      )
        ENDIF(NOT GDI_LIBRARY)
      ENDIF(MSVC80)

      # Specific path search for Visual Studio 2008
      IF(MSVC90)
        IF(NOT GDI_LIBRARY)
          FIND_LIBRARY(GDI_LIBRARY gdi32
                       "C:/Program Files/Microsoft Visual Studio 9/VC/PlatformSDK/Lib"
                      )
        ENDIF(NOT GDI_LIBRARY)
      ENDIF(MSVC90)
    ENDIF(CMAKE_CL_64)
  ENDIF(MINGW)
  # if GDI libraries found, then we're ok
  IF(GDI_LIBRARY)
    SET(GDI_FOUND "YES")
    SET(GDI_LIBRARIES ${GDI_LIBRARY})

  ENDIF(GDI_LIBRARY)

  MARK_AS_ADVANCED(GDI_LIBRARY)
ENDIF(WIN32)
