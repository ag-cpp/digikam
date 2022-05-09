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

#include "MiscPage.h"

// Qt includes

#include <QLabel>
#include <QLayout>

// Local includes

#include "Config.h"

namespace QtAVPlayer
{

MiscPage::MiscPage()
{
    QGridLayout* const gl = new QGridLayout();
    setLayout(gl);
    gl->setSizeConstraint(QLayout::SetFixedSize);
    int r                 = 0;
    m_preview_on          = new QCheckBox(tr("Preview"));
    gl->addWidget(m_preview_on, r++, 0);
    m_preview_w           = new QSpinBox();
    m_preview_w->setRange(1, 1920);
    m_preview_h           = new QSpinBox();
    m_preview_h->setRange(1, 1080);
    gl->addWidget(new QLabel(QString::fromLatin1("%1 %2: ").arg(tr("Preview")).arg(tr("size"))), r, 0);
    QHBoxLayout* hb       = new QHBoxLayout();
    hb->addWidget(m_preview_w);
    hb->addWidget(new QLabel(QString::fromLatin1("x")));
    hb->addWidget(m_preview_h);
    gl->addLayout(hb, r, 1);

    r++;
    gl->addWidget(new QLabel(tr("Force fps")), r, 0);
    m_fps                 = new QDoubleSpinBox();
    m_fps->setMinimum(-m_fps->maximum());
    m_fps->setToolTip(QString::fromLatin1("<= 0: ") + tr("Ignore"));
    gl->addWidget(m_fps, r++, 1);

    gl->addWidget(new QLabel(tr("Progress update interval") + QString::fromLatin1("(ms)")), r, 0);
    m_notify_interval     = new QSpinBox();
    m_notify_interval->setEnabled(false);
    gl->addWidget(m_notify_interval, r++, 1);

    gl->addWidget(new QLabel(tr("Buffer frames")), r, 0);
    m_buffer_value        = new QSpinBox();
    m_buffer_value->setRange(-1, 32767);
    m_buffer_value->setToolTip(QString::fromLatin1("-1: auto"));
    gl->addWidget(m_buffer_value, r++, 1);

    gl->addWidget(new QLabel(QString::fromLatin1("%1(%2)").arg(tr("Timeout")).arg(tr("s"))), r, 0);
    m_timeout             = new QDoubleSpinBox();
    m_timeout->setDecimals(3);
    m_timeout->setSingleStep(1.0);
    m_timeout->setMinimum(-0.5);
    m_timeout->setToolTip(QString::fromLatin1("<=0: never"));
    m_timeout_abort       = new QCheckBox(tr("Abort"));
    hb                    = new QHBoxLayout();
    hb->addWidget(m_timeout);
    hb->addWidget(m_timeout_abort);
    gl->addLayout(hb, r++, 1);

    gl->addWidget(new QLabel(tr("OpenGL type")), r, 0);
    m_opengl              = new QComboBox();
    m_opengl->addItem(QString::fromLatin1("Auto"), Config::Auto);
    m_opengl->addItem(QString::fromLatin1("Desktop"), Config::Desktop);
    m_opengl->addItem(QString::fromLatin1("OpenGLES"), Config::OpenGLES);
    m_opengl->addItem(QString::fromLatin1("Software"), Config::Software);
    m_opengl->setToolTip(tr("Windows only") + QLatin1String(" Qt>=5.4 + dynamicgl") + QString::fromLatin1("\n") + tr("OpenGLES is Used by DXVA Zero Copy"));
    gl->addWidget(m_opengl, r, 1);
    m_angle_platform      = new QComboBox();
    m_angle_platform->setToolTip(tr("D3D9 has performance if ZeroCopy is disabled or for software decoders") + QString::fromLatin1("\n") + tr("RESTART REQUIRED"));
    m_angle_platform->addItems(QStringList() << QString::fromLatin1("D3D9") << QString::fromLatin1("D3D11") << QString::fromLatin1("AUTO") << QString::fromLatin1("WARP"));

#ifndef QT_OPENGL_DYNAMIC

    m_opengl->setEnabled(false);
    m_angle_platform->setEnabled(false);

#endif

    gl->addWidget(m_angle_platform, r++, 2);

    gl->addWidget(new QLabel(QLatin1String("EGL")), r, 0);
    m_egl                 = new QCheckBox();
    m_egl->setToolTip(tr("Currently only works for Qt>=5.5 XCB build"));

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0) || !defined(Q_OS_LINUX)

    m_egl->setEnabled(false);

#endif

    gl->addWidget(m_egl, r++, 1);
}

QString MiscPage::name() const
{
    return tr("Misc");
}

void MiscPage::applyFromUi()
{
    Config::instance().setPreviewEnabled(m_preview_on->isChecked())
            .setPreviewWidth(m_preview_w->value())
            .setPreviewHeight(m_preview_h->value())
            .setEGL(m_egl->isChecked())
            .setOpenGLType((Config::OpenGLType)m_opengl->itemData(m_opengl->currentIndex()).toInt())
            .setANGLEPlatform(m_angle_platform->currentText().toLower())
            .setForceFrameRate(m_fps->value())
            .setBufferValue(m_buffer_value->value())
            .setTimeout(m_timeout->value())
            .setAbortOnTimeout(m_timeout_abort->isChecked())
    ;
}

void MiscPage::applyToUi()
{
    m_preview_on->setChecked(Config::instance().previewEnabled());
    m_preview_w->setValue(Config::instance().previewWidth());
    m_preview_h->setValue(Config::instance().previewHeight());
    m_opengl->setCurrentIndex(m_opengl->findData(Config::instance().openGLType()));
    m_angle_platform->setCurrentIndex(m_angle_platform->findText(Config::instance().getANGLEPlatform().toUpper()));
    m_egl->setChecked(Config::instance().isEGL());
    m_fps->setValue(Config::instance().forceFrameRate());

    //m_notify_interval->setValue(Config::instance().avfilterOptions());

    m_buffer_value->setValue(Config::instance().bufferValue());
    m_timeout->setValue(Config::instance().timeout());
    m_timeout_abort->setChecked(Config::instance().abortOnTimeout());
}

} // namespace QtAVPlayer
