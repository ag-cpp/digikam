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


#include "VideoEQConfigPage.h"

#include <algorithm>

#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include <QVector>

#include "../Slider.h"


VideoEQConfigPage::VideoEQConfigPage(QWidget *parent) :
    QWidget(parent)
{
    mEngine = SWScale;
    QGridLayout *gl = new QGridLayout();
    setLayout(gl);

    QLabel *label = new QLabel();
    label->setText(tr("Engine"));
    mpEngine = new QComboBox();
    setEngines(QVector<Engine>(1, SWScale));
    connect(mpEngine, SIGNAL(currentIndexChanged(int)), SLOT(onEngineChangedByUI()));

    int r = 0, c = 0;
    gl->addWidget(label, r, c);
    gl->addWidget(mpEngine, r, c+1);
    r++;


    struct {
        QSlider **slider;
        QString text;
    } sliders[] = {
        { &mpBSlider, tr("Brightness") },
        { &mpCSlider, tr("Constrast") },
        { &mpHSlider, tr("Hue") },
        { &mpSSlider, tr("Saturation") },
        { 0, QString() }
    };
    for (int i = 0; sliders[i].slider; ++i) {
        QLabel *label = new QLabel(sliders[i].text);
        *sliders[i].slider = new Slider();
        QSlider *slider = *sliders[i].slider;
        slider->setOrientation(Qt::Horizontal);
        slider->setTickInterval(2);
        slider->setRange(-100, 100);
        slider->setValue(0);

        gl->addWidget(label, r, c);
        gl->addWidget(slider, r, c+1);
        r++;
    }
    mpGlobal = new QCheckBox(tr("Global"));
    mpGlobal->setEnabled(false);
    mpGlobal->setChecked(false);
    mpResetButton = new QPushButton(tr("Reset"));

    gl->addWidget(mpGlobal, r, c, Qt::AlignLeft);
    gl->addWidget(mpResetButton, r, c+1, Qt::AlignRight);
    r++;

    connect(mpBSlider, SIGNAL(valueChanged(int)), SIGNAL(brightnessChanged(int)));
    connect(mpCSlider, SIGNAL(valueChanged(int)), SIGNAL(contrastChanged(int)));
    connect(mpHSlider, SIGNAL(valueChanged(int)), SIGNAL(hueChanegd(int)));
    connect(mpSSlider, SIGNAL(valueChanged(int)), SIGNAL(saturationChanged(int)));
    connect(mpGlobal, SIGNAL(toggled(bool)), SLOT(onGlobalSet(bool)));
    connect(mpResetButton, SIGNAL(clicked()), SLOT(onReset()));
}

void VideoEQConfigPage::onGlobalSet(bool g)
{
    Q_UNUSED(g);
}

void VideoEQConfigPage::setEngines(const QVector<Engine> &engines)
{
    mpEngine->clear();
    QVector<Engine> es(engines);
    // FIXME std::sort(es);
    mEngines = es;
    foreach (Engine e, es) {
        if (e == SWScale) {
            mpEngine->addItem(QString::fromLatin1("libswscale"));
        } else if (e == GLSL) {
            mpEngine->addItem(QString::fromLatin1("GLSL"));
        } else if (e == XV) {
            mpEngine->addItem(QString::fromLatin1("XV"));
        }
    }
}

void VideoEQConfigPage::setEngine(Engine engine)
{
    if (engine == mEngine)
        return;
    mEngine = engine;
    if (!mEngines.isEmpty()) {
        mpEngine->setCurrentIndex(mEngines.indexOf(engine));
    }
    emit engineChanged();
}

VideoEQConfigPage::Engine VideoEQConfigPage::engine() const
{
    return mEngine;
}

qreal VideoEQConfigPage::brightness() const
{
    return (qreal)mpBSlider->value()/100.0;
}

qreal VideoEQConfigPage::contrast() const
{
    return (qreal)mpCSlider->value()/100.0;
}

qreal VideoEQConfigPage::hue() const
{
    return (qreal)mpHSlider->value()/100.0;
}

qreal VideoEQConfigPage::saturation() const
{
    return (qreal)mpSSlider->value()/100.0;
}

void VideoEQConfigPage::onReset()
{
    mpBSlider->setValue(0);
    mpCSlider->setValue(0);
    mpHSlider->setValue(0);
    mpSSlider->setValue(0);
}

void VideoEQConfigPage::onEngineChangedByUI()
{
    if (mpEngine->currentIndex() >= mEngines.size() || mpEngine->currentIndex() < 0)
        return;
    mEngine = mEngines.at(mpEngine->currentIndex());
    emit engineChanged();
}
