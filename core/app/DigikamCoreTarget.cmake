#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

### digiKam core object library #################################################################################################

set(DIGIKAMCORE_OBJECTS

            utils/digikam_debug.cpp
            utils/digikam_version.cpp

            $<TARGET_OBJECTS:dimg_src>
            $<TARGET_OBJECTS:dragdrop_src>
            $<TARGET_OBJECTS:dmetadata_src>
            $<TARGET_OBJECTS:jpegutils_src>
            $<TARGET_OBJECTS:progressmanager_src>
            $<TARGET_OBJECTS:threadimageio_src>
            $<TARGET_OBJECTS:pgfutils_src>
            $<TARGET_OBJECTS:dthread_src>
            $<TARGET_OBJECTS:versionmanager_src>
            $<TARGET_OBJECTS:kmemoryinfo_src>
            $<TARGET_OBJECTS:libraw_src>
            $<TARGET_OBJECTS:rawengine_src>
            $<TARGET_OBJECTS:dpluginsinterface_src>
            $<TARGET_OBJECTS:libwso2_src>

            $<TARGET_OBJECTS:libmd5_src>
            $<TARGET_OBJECTS:libxmp_src>
            $<TARGET_OBJECTS:libdng_src>
            $<TARGET_OBJECTS:dngwriter_src>

            # widgets
            $<TARGET_OBJECTS:digikamwidgetscore_src>
            $<TARGET_OBJECTS:digikamdialogscore_src>
            $<TARGET_OBJECTS:itemproperties_src>
            $<TARGET_OBJECTS:digikamgenericmodels_src>
            $<TARGET_OBJECTS:notificationmanager_src>

            # utilities
            $<TARGET_OBJECTS:setupshowfoto_src>
            $<TARGET_OBJECTS:slideshow_src>
            $<TARGET_OBJECTS:imageeditor_src>
            $<TARGET_OBJECTS:digikamlibtransitionmngr_src>
            $<TARGET_OBJECTS:timeadjust_src>

            $<TARGET_OBJECTS:digikamdatabasecore_src>
            $<TARGET_OBJECTS:digikamfacesengine_src>
)

if(ENABLE_MEDIAPLAYER)

    set(DIGIKAMCORE_OBJECTS
        ${DIGIKAMCORE_OBJECTS}
        $<TARGET_OBJECTS:videotools_src>
    )

endif()

if(Marble_FOUND)

    set(DIGIKAMCORE_OBJECTS
        ${DIGIKAMCORE_OBJECTS}
        $<TARGET_OBJECTS:geoiface_src>
        $<TARGET_OBJECTS:geomapwrapper_src>
    )

endif()

if(KF5FileMetaData_FOUND)

    set(DIGIKAMCORE_OBJECTS
        ${DIGIKAMCORE_OBJECTS}
        $<TARGET_OBJECTS:baloowrap_src>
    )

endif()

if(KF5AkonadiContact_FOUND)

    set(DIGIKAMCORE_OBJECTS
        ${DIGIKAMCORE_OBJECTS}
        $<TARGET_OBJECTS:akonadiiface_src>
    )

endif()

add_library(digikamcore
            SHARED
            ${DIGIKAMCORE_OBJECTS}
)

set_target_properties(digikamcore PROPERTIES
                      VERSION ${DIGIKAM_VERSION_SHORT}
                      SOVERSION ${DIGIKAM_VERSION_SHORT}
)

target_compile_definitions(digikamcore
                           PRIVATE
                           digikamcore_EXPORTS
)

add_dependencies(digikamcore digikam-gitversion)

# All codes from this target are exported with digikam_core_export.h header and DIGIKAM_EXPORT macro.
generate_export_header(digikamcore
                       BASE_NAME digikam
                       EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/utils/digikam_core_export.h"
)

# Note all this target dependencies must be private and not exported to prevent inherited external plugins dependencies.

target_link_libraries(digikamcore

                      PRIVATE

                      Qt5::Core
                      Qt5::Gui
                      Qt5::Xml
                      Qt5::XmlPatterns
                      Qt5::Widgets
                      Qt5::Sql
                      Qt5::PrintSupport
                      Qt5::Concurrent

                      KF5::Solid
                      KF5::WindowSystem
                      KF5::ConfigGui
                      KF5::XmlGui
                      KF5::I18n
                      KF5::Service

                      # Required by CImg which use pthread internally.

                      ${CMAKE_THREAD_LIBS_INIT}
                      ${EXPAT_LIBRARY}

                      ${LCMS2_LIBRARIES} # filters

                      ${TIFF_LIBRARIES}
                      PNG::PNG
                      ${JPEG_LIBRARIES}
                      exiv2lib

                      ${FFMPEG_LIBRARIES}

                      ${OPENMP_LDFLAGS}
)

if(ENABLE_QWEBENGINE)

    target_link_libraries(digikamcore
                          PRIVATE
                          Qt5::WebEngineWidgets
    )

else()

    target_link_libraries(digikamcore
                          PRIVATE
                          Qt5::WebKitWidgets
    )

endif()

if(ENABLE_DBUS)

    target_link_libraries(digikamcore
                          PRIVATE
                          Qt5::DBus
    )

endif()

if(KF5IconThemes_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::IconThemes
    )

endif()

if(KF5KIO_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::KIOCore
                          KF5::KIOWidgets
    )

endif()

if(KF5Notifications_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::Notifications
    )

endif()

if(KF5NotifyConfig_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::NotifyConfig
    )

endif()

if(Marble_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${MARBLE_LIBRARIES}
    )

endif()

if(X11_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          Qt5::X11Extras
                          ${X11_LIBRARIES}
    )

endif()

if(Jasper_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${JASPER_LIBRARIES}
    )

endif()

# LibLqr-1 library rules for content-aware filter
if(Lqr-1_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${LQR-1_LIBRARIES}
    )

endif()

if(LensFun_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${LENSFUN_LIBRARIES}
    )

endif()

if(ImageMagick_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${ImageMagick_LIBRARIES}
    )

endif()

# for nrfilter
if(OpenCV_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${OpenCV_LIBRARIES}
    )

endif()

if(KF5FileMetaData_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::FileMetaData
    )

endif()

if(KF5AkonadiContact_FOUND)

    target_link_libraries(digikamcore
                          PRIVATE
                          KF5::AkonadiContact
    )

endif()

if(APPLE)

    target_link_libraries(digikamcore
                          PRIVATE
                          /System/Library/Frameworks/AppKit.framework
    )

endif()

if(WIN32)

    target_link_libraries(digikamcore
                          PRIVATE
                          wsock32
                          ws2_32
    )

endif()

if(CMAKE_SYSTEM_NAME STREQUAL FreeBSD)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${KVM_LIBRARY}
    )

endif()

if(ENABLE_MEDIAPLAYER)

    target_link_libraries(digikamcore
                          PRIVATE
                          ${QTAV_LIBRARIES}
    )

endif()

### Install Rules ###############################################################################################################

install(TARGETS digikamcore EXPORT DigikamCoreConfig ${INSTALL_TARGETS_DEFAULT_ARGS})
install(EXPORT DigikamCoreConfig DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DigikamCore" NAMESPACE Digikam::)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/DigikamCoreConfigVersion.cmake
                                 VERSION ${DIGIKAM_VERSION_SHORT}
                                 COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DigikamCoreConfigVersion.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DigikamCore")
