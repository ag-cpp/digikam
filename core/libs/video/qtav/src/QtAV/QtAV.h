/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTAV_H
#define QTAV_H

#include <QtAV/QtAV_Global.h>
#include <QtAV/QtAV_version.h>

#include <QtAV/AVError.h>
#include <QtAV/AVClock.h>
#include <QtAV/AVDecoder.h>
#include <QtAV/AVDemuxer.h>
#include <QtAV/AVMuxer.h>
#include <QtAV/AVOutput.h>
#include <QtAV/AVPlayer.h>
#include <QtAV/Packet.h>
#include <QtAV/Statistics.h>

#include <QtAV/AudioEncoder.h>
#include <QtAV/AudioDecoder.h>
#include <QtAV/AudioFormat.h>
#include <QtAV/AudioOutput.h>
#include <QtAV/AudioResampler.h>

#include <QtAV/Filter.h>
#include <QtAV/FilterContext.h>
#include <QtAV/GLSLFilter.h>
#include <QtAV/LibAVFilter.h>

#if (QT_VERSION == QT_VERSION_CHECK(5,0,0) && !defined(QT_NO_OPENGL)) || (QT_VERSION >= QT_VERSION_CHECK(6,0,0) && defined(QT_OPENGL_LIB))
#include <QtAV/Geometry.h>
#include <QtAV/GeometryRenderer.h>
#include <QtAV/VideoShader.h>
#include <QtAV/OpenGLVideo.h>
#include <QtAV/ConvolutionShader.h>
#include <QtAV/VideoShaderObject.h>
#endif

#include <QtAV/VideoCapture.h>
#include <QtAV/VideoEncoder.h>
#include <QtAV/VideoDecoder.h>
#include <QtAV/VideoFormat.h>
#include <QtAV/VideoFrame.h>
#include <QtAV/VideoFrameExtractor.h>
#include <QtAV/VideoRenderer.h>
#include <QtAV/VideoOutput.h>
//The following renderer headers can be removed
#include <QtAV/QPainterRenderer.h>
#if (QT_VERSION >= QT_VERSION_CHECK(5,4,0) && QT_VERSION < QT_VERSION_CHECK(6,0,0)) || (QT_VERSION >= QT_VERSION_CHECK(6,0,0) && defined(QT_OPENGL_LIB))
#include <QtAV/OpenGLWindowRenderer.h>
#endif
#include <QtAV/Subtitle.h>
#include <QtAV/SubtitleFilter.h>

#include <QtAV/MediaIO.h>

#endif // QTAV_H
