/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

// Local includes

#include "QtAV_Global.h"
#include "QtAV_Version.h"

#include "AVError.h"
#include "AVClock.h"
#include "AVDecoder.h"
#include "AVDemuxer.h"
#include "AVMuxer.h"
#include "AVOutput.h"
#include "AVPlayer.h"
#include "Packet.h"
#include "QtAV_Statistics.h"

#include "AudioEncoder.h"
#include "AudioDecoder.h"
#include "AudioFormat.h"
#include "AudioOutput.h"
#include "AudioResampler.h"

#include "Filter.h"
#include "FilterContext.h"
#include "GLSLFilter.h"
#include "LibAVFilter.h"

#if (QT_VERSION == QT_VERSION_CHECK(5,0,0) && !defined(QT_NO_OPENGL)) || (QT_VERSION >= QT_VERSION_CHECK(6,0,0) && defined(QT_OPENGL_LIB))
#   include "Geometry.h"
#   include "GeometryRenderer.h"
#   include "VideoShader.h"
#   include "OpenGLVideo.h"
#   include "ConvolutionShader.h"
#   include "VideoShaderObject.h"
#endif

#include "VideoCapture.h"
#include "VideoEncoder.h"
#include "VideoDecoder.h"
#include "VideoFormat.h"
#include "VideoFrame.h"
#include "VideoFrameExtractor.h"
#include "VideoRenderer.h"
#include "VideoOutput.h"

// The following renderer headers can be removed

#include "QPainterRenderer.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5,4,0) && QT_VERSION < QT_VERSION_CHECK(6,0,0)) || (QT_VERSION >= QT_VERSION_CHECK(6,0,0) && defined(QT_OPENGL_LIB))
#   include "OpenGLWindowRenderer.h"
#endif

#include "Subtitle.h"
#include "SubtitleFilter.h"

#include "MediaIO.h"

#endif // QTAV_H
