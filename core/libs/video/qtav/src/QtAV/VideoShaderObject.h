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

#ifndef QTAV_VIDEO_SHADER_OBJECT_H
#define QTAV_VIDEO_SHADER_OBJECT_H

#include <QObject>
#include <QVector>

// Local includes

#include "VideoShader.h"

namespace QtAV
{

// check and auto update properties in shader

class VideoShaderObjectPrivate;

/*!
 * \brief The VideoShaderObject class
 * User defined uniform names are bound to class meta properties (property signals are required)
 * and object dynamic properties.
 * Property value type T is limited to float, int, unsigned(ES3.0) and QVector<T>
 */
class Q_AV_EXPORT VideoShaderObject : public QObject, public VideoShader
{
    DPTR_DECLARE_PRIVATE(VideoShaderObject)
    Q_OBJECT

public:

    VideoShaderObject(QObject* parent = 0);

protected:

    VideoShaderObject(VideoShaderObjectPrivate &d, QObject* parent = 0);
    bool event(QEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:

    void propertyChanged(int id);

private:

    void programReady() Q_DECL_OVERRIDE Q_DECL_FINAL;
};

class DynamicShaderObjectPrivate;

/*!
 * \brief The DynamicShaderObject class
 * Able to set custom shader code
 */
class Q_AV_EXPORT DynamicShaderObject : public VideoShaderObject
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(DynamicShaderObject)
    Q_PROPERTY(QString header READ header WRITE setHeader NOTIFY headerChanged)
    Q_PROPERTY(QString sample READ sample WRITE setSample NOTIFY sampleChanged)
    Q_PROPERTY(QString postProcess READ postProcess WRITE setPostProcess NOTIFY postProcessChanged)

public:

    DynamicShaderObject(QObject* parent = 0);
    QString header() const;
    void setHeader(const QString& text);
    QString sample() const;
    void setSample(const QString& text);
    QString postProcess() const;
    void setPostProcess(const QString& text);

Q_SIGNALS:

    void headerChanged();
    void sampleChanged();
    void postProcessChanged();

protected:

    DynamicShaderObject(DynamicShaderObjectPrivate &d, QObject* parent = 0);

private:

    const char* userShaderHeader(QOpenGLShader::ShaderType st) const Q_DECL_OVERRIDE;
    const char* userSample() const Q_DECL_OVERRIDE;
    const char* userPostProcess() const Q_DECL_OVERRIDE;
};

} // namespace QtAV

#endif //QTAV_VIDEO_SHADER_OBJECT_H
