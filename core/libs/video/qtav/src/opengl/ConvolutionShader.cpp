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

#include "ConvolutionShader.h"

// Local includes

#include "VideoShader_p.h"

namespace QtAV
{

class Q_DECL_HIDDEN ConvolutionShaderPrivate : public VideoShaderPrivate
{
public:

    ConvolutionShaderPrivate()
        : VideoShaderPrivate(),
          u_Kernel(-1),
          radius(1)
    {
        kernel.resize((2 * radius + 1) * (2 * radius + 1));
        updateShaderCode();
    }

    void updateShaderCode()
    {
        const int ks = (2*radius+1)*(2*radius+1);
        header       = QStringLiteral("uniform float u_Kernel[%1];").arg(ks).toUtf8();
        QString s    = QStringLiteral("vec4 sample2d(sampler2D tex, vec2 pos, int p) { vec4 c = vec4(0.0);");
        const int kd = 2*radius+1;

        for (int i = 0; i < ks; ++i)
        {
            const int x = i % kd - radius;
            const int y = i / kd - radius;
            s += QStringLiteral("c += texture(tex, pos + u_texelSize[p]*vec2(%1.0,%2.0))*u_Kernel[%3];")
                    .arg(x).arg(y).arg(i);
        }

        s += QStringLiteral("c.a = texture(tex, pos).a;"
             "return c;}\n");

        sample_func = s.toUtf8();
    }

    int            u_Kernel;
    int            radius;
    QVector<float> kernel;
    QByteArray     header;
    QByteArray     sample_func;

private:

    Q_DISABLE_COPY(ConvolutionShaderPrivate);
};

ConvolutionShader::ConvolutionShader()
    : VideoShader(*new ConvolutionShaderPrivate())
{
}

ConvolutionShader::ConvolutionShader(ConvolutionShaderPrivate& d)
    : VideoShader(d)
{
}

int ConvolutionShader::kernelRadius() const
{
    return d_func().radius;
}

void ConvolutionShader::setKernelRadius(int value)
{
    DPTR_D(ConvolutionShader);

    if (d.radius == value)
        return;

    d.radius = value;
    d.kernel.resize(kernelSize());
    d.updateShaderCode();
    rebuildLater();
}

int ConvolutionShader::kernelSize() const
{
    return (2*kernelRadius() + 1)*(2*kernelRadius() + 1);
}

const char* ConvolutionShader::userShaderHeader(QOpenGLShader::ShaderType t) const
{
    if (t == QOpenGLShader::Vertex)
        return nullptr;

    return kernelUniformHeader().constData();
}

const char* ConvolutionShader::userSample() const
{
    return kernelSample().constData();
}

bool ConvolutionShader::setUserUniformValues()
{
    setKernelUniformValue();

    return true;
}

const QByteArray& ConvolutionShader::kernelUniformHeader() const
{
    return d_func().header;
}

const QByteArray& ConvolutionShader::kernelSample() const
{
    return d_func().sample_func;
}

void ConvolutionShader::setKernelUniformValue()
{
    program()->setUniformValueArray("u_Kernel", kernel(), kernelSize(), 1);
}

} // namespace QtAV
