#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

message(STATUS "")
message(STATUS "--------------------------------------------------")
message(STATUS "digiKam MediaPlayer dependencies checks:")
message(STATUS "")

find_package(FFmpeg COMPONENTS AVCODEC
                               AVDEVICE
                               AVFILTER
                               AVFORMAT
                               AVUTIL
                               SWSCALE
                               AVRESAMPLE
                               SWRESAMPLE
)

find_package(ASS)
find_package(OpenAL)
find_package(Portaudio)
find_package(PulseAudio)
find_package(VAAPI)
find_package(uchardet)

include(MacroSSE)
CheckSSESupport()

if(ENABLE_MEDIAPLAYER)

    message(STATUS "FFMpeg AVCodec    : ${AVCODEC_FOUND}")
    message(STATUS "FFMpeg AVDevice   : ${AVDEVICE_FOUND}")
    message(STATUS "FFMpeg AVFilter   : ${AVFILTER_FOUND}")
    message(STATUS "FFMpeg AVFormat   : ${AVFORMAT_FOUND}")
    message(STATUS "FFMpeg AVUtil     : ${AVUTIL_FOUND}")
    message(STATUS "FFMpeg SWScale    : ${SWSCALE_FOUND}")
    message(STATUS "FFMpeg AVResample : ${AVRESAMPLE_FOUND}")
    message(STATUS "FFMpeg SWResample : ${SWRESAMPLE_FOUND}")

    if(${AVCODEC_FOUND}    AND
       ${AVDEVICE_FOUND}   AND
       ${AVFILTER_FOUND}   AND
       ${AVFORMAT_FOUND}   AND
       ${AVUTIL_FOUND}     AND
       ${SWSCALE_FOUND}    AND
       ${AVRESAMPLE_FOUND} AND
       ${SWRESAMPLE_FOUND}
      )

        include_directories(${FFMPEG_INCLUDE_DIRS})
        message(STATUS "MediaPlayer support is enabled                      : yes")

    else()

        set(ENABLE_MEDIAPLAYER OFF)
        set(FFMPEG_FOUND OFF)
        message(STATUS "MediaPlayer support is enabled                      : no")

    endif()

    if (${FFMPEG_FOUND})

        MACRO_BOOL_TO_01(ASS_FOUND        HAVE_LIBASS)
        MACRO_BOOL_TO_01(uchardet_FOUND   HAVE_LIBUCHARDET)
        MACRO_BOOL_TO_01(OPENAL_FOUND     HAVE_LIBOPENAL)
        MACRO_BOOL_TO_01(PORTAUDIO_FOUND  HAVE_LIBPORTAUDIO)
        MACRO_BOOL_TO_01(PULSEAUDIO_FOUND HAVE_LIBPULSEAUDIO)
        MACRO_BOOL_TO_01(VAAPI_FOUND      HAVE_LIBVAAPI)
        MACRO_BOOL_TO_01(AVCODEC_FOUND    HAVE_LIBAVCODEC)
        MACRO_BOOL_TO_01(AVDEVICE_FOUND   HAVE_LIBAVDEVICE)
        MACRO_BOOL_TO_01(AVFILTER_FOUND   HAVE_LIBAVFILTER)
        MACRO_BOOL_TO_01(AVFORMAT_FOUND   HAVE_LIBAVFORMAT)
        MACRO_BOOL_TO_01(AVUTIL_FOUND     HAVE_LIBAVUTIL)
        MACRO_BOOL_TO_01(SWSCALE_FOUND    HAVE_LIBSWSCALE)
        MACRO_BOOL_TO_01(AVRESAMPLE_FOUND HAVE_LIBAVRESAMPLE)
        MACRO_BOOL_TO_01(SWRESAMPLE_FOUND HAVE_LIBSWRESAMPLE)

        if(SSE4_1_FOUND)

            message(STATUS "MediaPlayer will be compiled with SSE4.1 support    : yes")
            set(MEDIAPLAYER_FLAGS -msse4.1)

        elseif(SSE2_FOUND)

            message(STATUS "MediaPlayer will be compiled with SSE2 support      : yes")
            set(MEDIAPLAYER_FLAGS -msse2)

        endif()

        # ---

        set(MEDIAPLAYER_LIBRARIES ${FFMPEG_LIBRARIES} ${CMAKE_DL_LIBS})

        if(ASS_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${ASS_LIBRARIES})
            message(STATUS "MediaPlayer will be compiled with LibASS support    : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with LibASS support    : no")

        endif()

        if(uchardet_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} uchardet)
            message(STATUS "MediaPlayer will be compiled with UCharDet support  : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with UCharDet support  : no")

        endif()

        if(OPENAL_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${OPENAL_LIBRARY})
            message(STATUS "MediaPlayer will be compiled with OpenAL support    : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with OpenAL support    : no")

        endif()

        if(PORTAUDIO_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${PORTAUDIO_LIBRARIES})
            message(STATUS "MediaPlayer will be compiled with PortAudio support : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with PortAudio support : no")

        endif()

        if(PULSEAUDIO_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${PULSEAUDIO_LIBRARIES})
            message(STATUS "MediaPlayer will be compiled with PulseAudio support: yes")

        else()

            message(STATUS "MediaPlayer will be compiled with PukseAudio support: no")

        endif()

        if(VAAPI_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${VAAPI_LIBRARIES})
            message(STATUS "MediaPlayer will be compiled with VaAPI support     : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with VaAPI support     : no")

        endif()

        if(OPENGL_FOUND)

            set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${OPENGL_LIBRARIES} Qt${QT_VERSION_MAJOR}::OpenGL)

            if(Qt6_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

            endif()

            message(STATUS "MediaPlayer will be compiled with OpenGL support    : yes")

        else()

            message(STATUS "MediaPlayer will be compiled with OpenGL support    : no")

        endif()


        # ---

        set(MEDIAPLAYER_DEFINITIONS -D__STDC_CONSTANT_MACROS
                                    -DQTAV_HAVE_GL=1             # For QtAVWidgets
                                    -DQTAV_HAVE_CAPI=1           # To load libass, vaapi, and EGL dynamically
                                    -DQTAV_HAVE_OPENSL=0         # OpenSLES is for Android only
                                    -DQTAV_HAVE_D3D11VA=0        # DirectX 3D for MSVC only
                                    -DQTAV_HAVE_DSOUND=0         # DirectX Sound for MSVC only
                                    -DQTAV_HAVE_DXVA=0           # DirectX VA for MSVC only
                                    -DQTAV_HAVE_XAUDIO2=0        # DirectX Audio for MSVC only
                                    -DQTAV_HAVE_X11=0            # Never enabled in QtAV (see below)
        )

        # This one is never used in QtAV
        #set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_X11=${HAVE_LIBX11})

        if(SSE4_1_FOUND)

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE4_1=1)
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE2=0)

        elseif(SSE2_FOUND)

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE4_1=0)
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SSE2=1)

        endif()

        if(ASS_FOUND)

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DCAPI_LINK_ASS)

        endif()

        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_LIBASS=${HAVE_LIBASS})

        if (uchardet_FOUND)

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DLINK_UCHARDET)

        endif()

        if(OPENAL_FOUND)

            if(APPLE)

                set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DHEADER_OPENAL_PREFIX)

            endif()

        endif()

        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_OPENAL=${HAVE_LIBOPENAL})
        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_PORTAUDIO=${HAVE_LIBPORTAUDIO})
        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_PULSEAUDIO=${HAVE_LIBPULSEAUDIO})

        if(APPLE)

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_VIDEOTOOLBOX=1)
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_CUDA=0)

        else()

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_VIDEOTOOLBOX=0)
            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_CUDA=1)

        endif()

        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_AVCODEC=${HAVE_LIBAVCODEC})
        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_AVDEVICE=${HAVE_LIBAVDEVICE})
        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_AVFILTER=${HAVE_LIBAVFILTER})
        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_AVUTIL=${HAVE_LIBAVUTIL})
        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SWSCALE=${HAVE_LIBSWSCALE})
        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_AVRESAMPLE=${HAVE_LIBAVRESAMPLE})
        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_SWRESAMPLE=${HAVE_LIBSWRESAMPLE})
        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_EGL_CAPI=${HAVE_LIBOPENGL_EGL})
        set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQTAV_HAVE_VAAPI=${HAVE_LIBVAAPI})

        # ---

        message(STATUS "MediaPlayer libraries  : ${MEDIAPLAYER_LIBRARIES}")
        message(STATUS "MediaPlayer definitions: ${MEDIAPLAYER_DEFINITIONS}")
        message(STATUS "MediaPlayer flags      : ${MEDIAPLAYER_FLAGS}")

    endif()

endif()
