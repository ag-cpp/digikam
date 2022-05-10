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

#include "ShaderPage.h"

// Qt includes

#include <QLabel>
#include <QLayout>

// Local includes

#include "ConfigManager.h"

namespace QtAVPlayer
{

ShaderPage::ShaderPage(QWidget* const parent)
    : ConfigPageBase(parent)
{
    QVBoxLayout* const gl = new QVBoxLayout();
    setLayout(gl);
    gl->setSizeConstraint(QLayout::SetMaximumSize);

    const int mw          = 600;
    m_enable              = new QCheckBox(i18n("Enable"));
    gl->addWidget(m_enable);
    m_fbo                 = new QCheckBox(i18n("Intermediate FBO"));
    gl->addWidget(m_fbo);
    gl->addWidget(new QLabel(i18n("Fragment shader header")));
    m_header              = new QTextEdit();

    //m_header->setMaximumWidth(mw);

    m_header->setMaximumHeight(mw / 6);
    m_header->setToolTip(i18n("Additional header code"));
    gl->addWidget(m_header);
    gl->addWidget(new QLabel(i18n("Fragment shader texel sample function")));
    m_sample              = new QTextEdit();

    //m_sample->setMaximumWidth(mw);

    m_sample->setMaximumHeight(mw / 6);
    m_sample->setToolTip(QLatin1String("vec4 sample2d(sampler2D tex, vec2 pos, int p)"));
    gl->addWidget(m_sample);
    gl->addWidget(new QLabel(QLatin1String("Fragment shader RGB post process code")));
    m_pp                  = new QTextEdit();

    //m_pp->setMaximumWidth(mw);

    m_pp->setMaximumHeight(mw/6);
    gl->addWidget(m_pp);
    gl->addStretch();
}

QString ShaderPage::name() const
{
    return i18n("Shader");
}

void ShaderPage::applyToUi()
{
    m_enable->setChecked(ConfigManager::instance().userShaderEnabled());
    m_fbo->setChecked(ConfigManager::instance().intermediateFBO());
    m_header->setText(ConfigManager::instance().fragHeader());
    m_sample->setText(ConfigManager::instance().fragSample());
    m_pp->setText(ConfigManager::instance().fragPostProcess());
}

void ShaderPage::applyFromUi()
{
    ConfigManager::instance()
            .setUserShaderEnabled(m_enable->isChecked())
            .setIntermediateFBO(m_fbo->isChecked())
            .setFragHeader(m_header->toPlainText())
            .setFragSample(m_sample->toPlainText())
            .setFragPostProcess(m_pp->toPlainText())
    ;
}

} // namespace QtAVPlayer
